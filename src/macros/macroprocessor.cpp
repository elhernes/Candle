/***************************************************
 * file: github/elh/Candle/src/macros/macroprocessor.cpp
 *
 * @file    macroprocessor.cpp
 * @author  Eric L. Hernes
 * @version V1.0
 * @born_on   Sunday, April 16, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C++ module
 *
 */

#include "macroprocessor.h"

#include "../libs/libMathExpr/MathExpr.h"

#include <map>
#include <mutex>
#include <QRegExp>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

static bool
readTextFromUrl(QString &text, const QUrl &url) {
  bool rv=false;
  if (url.isLocalFile()) {
    QFile f(url.toLocalFile());
    if (f.open(QIODevice::ReadOnly|QFile::Text) == false) {
      return false;
    }
    QTextStream fstream(&f);
    text = fstream.readAll();
    rv = true;
  }
  return rv;
}

struct MacroProcessor::Privates {
  void debug_step(const QString &path, const QString &lineIn, const QString &out, const QString &vars);
  MathExpr me;
  MachineControl *mc;
  unsigned cmdNumber;
  bool debug;
  QStringList list;
  std::mutex mx;
};

void
MacroProcessor::Privates::debug_step(const QString &path, const QString &lineIn, const QString &out, const QString &vars) {
  if (this->debug) {
    QString msg;
    msg += "Command#: " + QString::number(this->cmdNumber);
    msg += "\nPath: " + path;
    msg += "\n----------------------------------------\n";
    msg += "Input Line:\n";
    msg += lineIn;
    msg += "\n----------------------------------------\n";
    msg += "Output Line\n";
    msg += out;
    QMessageBox msgBox;
    msgBox.setText(msg);

    QString details;
    details += "Variables:\n";
    details += vars;
    details += "\n";
    msgBox.setDetailedText(details);

    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    switch (msgBox.exec()) {
    case QMessageBox::Ok:
      break;
    case QMessageBox::Cancel: // hare kari
      break;
    }
  }
}

MacroProcessor::MacroProcessor(MachineControl *mc) : m_p(new Privates) {
  m_p->mc = mc;
  m_p->cmdNumber = 0;
  m_p->debug = false;
#if 0
  connect(m_p->mc, SIGNAL(noCommandsPending()),
	  this, SLOT(onNoCommandsPending()));
#endif
}


MacroProcessor::~MacroProcessor() {
#if 0
  disconnect(m_p->mc, SIGNAL(noCommandsPending()),
	     this, SLOT(onNoCommandsPending()));
#endif
  if (m_p) {
    delete m_p;
    m_p=nullptr; // for debug
  }
}

static const std::map<std::string,std::function<QString(MacroProcessor::Privates &p, const QStringList &cmd)>> skDispatchTable = {
  { "%wait", [](MacroProcessor::Privates &, const QStringList &) -> QString {
      // from cncjs GrblController.js:
      // G4 P0 or P with a very small value will empty the planner queue and then
      // respond with an ok when the dwell is complete. At that instant, there will
      // be no queued motions, as long as no more commands were sent after the G4.
      // This is the fastest way to do it without having to check the status reports.
      return "G4 P0.5";
    } },

  { "%save-modal-state", [](MacroProcessor::Privates &p, const QStringList &) -> QString {
      QString ps = p.mc->storeModalState();
      return "; save-modal-state {" + ps + "}";
    } },

  { "%restore-modal-state", [](MacroProcessor::Privates &p, const QStringList &) -> QString {
      p.mc->restoreModalState();
      return "; modal-parser-state";
    } },

  { "%message", [](MacroProcessor::Privates &p, const QStringList &sl) -> QString {
      QMessageBox msgBox;
      msgBox.setText(sl.join(" "));
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.exec();
      return "";
    } },

  { "%include-macro", [](MacroProcessor::Privates &p, const QStringList &sl) -> QString {
      QString text;
      if (!readTextFromUrl(text, QUrl(sl[0]))) {
	QString macrotext;
	p.mc->macroText(macrotext, sl[0]);
	if (!readTextFromUrl(text, QUrl(macrotext))) {
	  text = macrotext;
	}
      }
      if (text != "") {
	QStringList list = text.split("\n");
	list += p.list;
	p.list = list;
      }
    } },
};

bool
MacroProcessor::process(const QString &macroText) {
  QString text;
  if (!readTextFromUrl(text, QUrl(macroText))) {
    text = macroText;
  }

  m_p->list = text.split("\n");
  onNoCommandsPending();

  return true;
}

void
MacroProcessor::onNoCommandsPending() {
  if (m_p != nullptr) {
    bool aborted=false;
    if(auto lock = std::unique_lock<std::mutex>(m_p->mx, std::try_to_lock)) {
      while (!aborted && (m_p->mc->commandsPending() == 0) && processNext()) {
	if (m_p->mc->status() == "Alarm") {
	  QMessageBox msgBox;
	  msgBox.setText("Grbl Alarm");
	  msgBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Retry);

	  switch (msgBox.exec()) {
	  case QMessageBox::Abort:
	    aborted = true;
	    break;
	  case QMessageBox::Retry:
	    m_p->mc->sendCommand("$X", -1, false);
	    break;
	  }
	}
      }
    }
    if (aborted) {
      emit finished(); // will destroy this
    }
  }
}

bool
MacroProcessor::processNext() {
  if (m_p->list.isEmpty()) {
    emit finished();
    return false;
  }
  auto cmd = m_p->list.takeFirst();
  QString out;

  QString vars = QString::fromStdString(m_p->me.dumpVariables());
  QString path = "raw";
  if (cmd.length() > 1) {
    QStringList argv = cmd.split(" ");
    QString arg0 = argv.first();
    argv.pop_front();
    auto const &de = skDispatchTable.find(arg0.toStdString());
    if (de != skDispatchTable.end()) {
      path = "dispatch";
      out = de->second(*m_p, argv);
    } else if (cmd.at(0) == '%' && cmd.at(1) != '{') {
      path = "eval";
      out = eval(cmd);
    } else {
      path = "expand";
      out = expand(cmd);
    }
  } else {
    out = cmd;
  }
  m_p->debug_step(path, cmd, out, vars);
  m_p->cmdNumber++;
  m_p->mc->sendCommand(out, -1, true);
  return true;
}

QString
MacroProcessor::eval(const QString &qexpr) {
  static QRegExp vxp("^%([^;]*);*");
  QString qrv;
  if (vxp.indexIn(qexpr) != -1) {
    std::string expr = vxp.cap(1).toStdString();
    std::string result = m_p->me.parse(expr.c_str());
    //    qrv = "; eval: " + QString::fromStdString(result);
  } else {
    qrv = qexpr;
  }
  return qrv;
}

QString
MacroProcessor::expand(QString expr) {
  static QRegExp vxp("([^%]*)%\\{([^}]*)\\}(.*)");
  while (vxp.indexIn(expr) != -1) {
    expr = vxp.cap(1) + QString::number(getVariable(vxp.cap(2))) + vxp.cap(3);
  }
  return expr;
}

bool
MacroProcessor::setVariable(const QString &qvar, double val) {
  std::string var = qvar.toStdString();
  m_p->me.setVariable(var.c_str(), val);
  return true;
}

double
MacroProcessor::getVariable(const QString &qvar) {
  std::string var = qvar.toStdString();
  return m_p->me.getVariable(var.c_str());
}

/* end of github/elh/Candle/src/macros/macroprocessor.cpp */

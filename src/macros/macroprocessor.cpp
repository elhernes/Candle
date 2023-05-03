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

#include "frmmain.h"
#include "macroprocessor.h"

#include "../libs/libMathExpr/MathExpr.h"

#include <map>
#include <mutex>
#include <QRegExp>
#include <QStringList>

struct MacroProcessor::Privates {
  void debug_step(const QString &path, const QString &lineIn, const QString &out, const QString &vars);
  MathExpr me;
  frmMain *frmmain;
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

MacroProcessor::MacroProcessor(frmMain *frm) : m_p(new Privates) {
  m_p->frmmain = frm;
  m_p->cmdNumber = 0;
  m_p->debug = false;
  connect(m_p->frmmain, SIGNAL(grblStatusChanged(const QString&)),
	  this, SLOT(onGrblStatusChanged(const QString&)));
  connect(m_p->frmmain, SIGNAL(noCommandsPending()),
	  this, SLOT(onNoCommandsPending()));
}


MacroProcessor::~MacroProcessor() {
  disconnect(m_p->frmmain, SIGNAL(grblStatusChanged(const QString&)),
	     this, SLOT(onGrblStatusChanged(const QString&)));
  disconnect(m_p->frmmain, SIGNAL(noCommandsPending()),
	     this, SLOT(onNoCommandsPending()));
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

  { "%save-parser-state", [](MacroProcessor::Privates &p, const QStringList &) -> QString {
      QString ps = p.frmmain->storeParserState();
      return "; save-parser-state {" + ps + "}";
    } },

  { "%restore-parser-state", [](MacroProcessor::Privates &p, const QStringList &) -> QString {
      p.frmmain->restoreParserState();
      return "; restore-parser-state";
    } },

  { "%message", [](MacroProcessor::Privates &p, const QStringList &sl) -> QString {
      QMessageBox msgBox;
      msgBox.setText(sl.join(" "));
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.exec();
      return "";
    } },
};

bool
MacroProcessor::process(const QString &macroText) {
  QUrl url(macroText);
  if (url.isLocalFile()) {
    QFile f(url.toLocalFile());
    if (f.open(QIODevice::ReadOnly|QFile::Text) == false) {
      // error or quietly fail? we pick the second
      return false;
    }
    QTextStream fstream(&f);
    QString fileText = fstream.readAll();
    m_p->list = fileText.split("\n");

  } else {
    m_p->list = macroText.split("\n");
  }

  onNoCommandsPending();

  return true;
}

void
MacroProcessor::onGrblStatusChanged(const QString &status) {
  if (status == "Alarm") {
  }
}

void
MacroProcessor::onNoCommandsPending() {
  bool aborted=false;
  if(auto lock = std::unique_lock<std::mutex>(m_p->mx, std::try_to_lock)) {
    while (!aborted && (m_p->frmmain->commandsPending() == 0) && processNext()) {
      if (m_p->frmmain->status() == "Alarm") {
	QMessageBox msgBox;
	msgBox.setText("Grbl Alarm");
	msgBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Retry);

	switch (msgBox.exec()) {
	case QMessageBox::Abort:
	  aborted = true;
	  break;
	case QMessageBox::Retry:
	  m_p->frmmain->sendCommand("$X", -1, false);
	  break;
	}
      }
    }
  }
  if (aborted) {
    emit finished(); // will destroy this
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
  m_p->frmmain->sendCommand(out, -1, true);
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

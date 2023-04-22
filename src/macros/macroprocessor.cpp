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
#include <QRegExp>
#include <QStringList>

struct MacroProcessor::Privates {
  void debug_step(const QString &path, const QString &lineIn, const QString &out, const QString &vars);
  MathExpr me;
  bool terminated;
  frmMain *frmmain;
  unsigned cmdNumber;
  bool debug;
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
      this->terminated = true;
    }
  }
}

MacroProcessor::MacroProcessor(frmMain *frm) : m_p(new Privates) {
  m_p->terminated = false;
  m_p->frmmain = frm;
  m_p->cmdNumber = 0;
  m_p->debug = false;
}

MacroProcessor::~MacroProcessor() {
  if (m_p) {
    delete m_p;
  }
}

bool
MacroProcessor::terminated() {
  return m_p->terminated;
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

  { "%terminate-macroproc", [](MacroProcessor::Privates &p, const QStringList &) -> QString {
      p.terminated = true;
      return "; terminate-macroproc";
    } },
};

void
MacroProcessor::terminate() {
  m_p->terminated = true;
}

bool
MacroProcessor::process(QString &out, const QString &cmd) {
  QString vars = QString::fromStdString(m_p->me.dumpVariables());
  QString path = "raw";
  if (cmd.length() > 1) {
    QStringList sl = cmd.split(" ");
    auto const &de = skDispatchTable.find(cmd.toStdString());
    if (de != skDispatchTable.end()) {
      path = "dispatch";
      out = de->second(*m_p, sl);
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

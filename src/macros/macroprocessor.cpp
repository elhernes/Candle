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
  MathExpr me;
  bool terminated;
  frmMain *frmmain;
  bool verbose;
};

MacroProcessor::MacroProcessor(frmMain *frm) : m_p(new Privates) {
  m_p->terminated = false;
  m_p->frmmain = frm;
  m_p->verbose = false;
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
      return "";
    } },

  { "%save-parser-state", [](MacroProcessor::Privates &p, const QStringList &) -> QString {
      p.frmmain->storeParserState();
      return "";
    } },

  { "%restore-parser-state", [](MacroProcessor::Privates &p, const QStringList &) -> QString {
      p.frmmain->restoreParserState();
      return "";
    } },

  { "%terminate-macroproc", [](MacroProcessor::Privates &p, const QStringList &) -> QString {
      p.terminated = true;
      return "";
    } },
};

QString
MacroProcessor::process(const QString &cmd) {
  QString out;
  if (cmd.length() > 1) {
    QStringList sl = cmd.split(" ");
    auto const &de = skDispatchTable.find(cmd.toStdString());
    if (de != skDispatchTable.end()) {
      out = de->second(*m_p, sl);
    } else if (cmd.at(0) == '%' && cmd.at(1) != '{') {
      out = eval(cmd);
    } else {
      out = expand(cmd);
    }
  } else {
    out = cmd;
  }
  return out;
}

QString
MacroProcessor::eval(const QString &qexpr) {
  static QRegExp vxp("^%([^;]*);*");
  QString qrv;
  if (vxp.indexIn(qexpr) != -1) {
    std::string expr = vxp.cap(1).toStdString();
    std::string result = m_p->me.parse(expr.c_str());
    qrv = "; eval: " + QString::fromStdString(result);
    if (m_p->verbose) {
      printf("%s: %s\n", __func__, expr.c_str());
      printf("  (%s)\n", result.c_str());
      const class Error *em=nullptr;
      m_p->me.getError(em);
      if (em) {
	printf("err: %s\n", em->get_msg());
      }
      m_p->me.dumpVariables();
      fflush(stdout);
    }
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

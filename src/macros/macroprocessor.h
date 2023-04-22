/***************************************************
 * file: github/elh/Candle/src/macros/macroprocessor.h
 *
 * @file    macroprocessor.h
 * @author  Eric L. Hernes
 * @born_on   Sunday, April 16, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C/C++ header
 *
 * $Id$
 */

#pragma once

#include <QString>

class MacroProcessor {
 public:
  MacroProcessor(frmMain *frm);
  ~MacroProcessor();

  // request termination at the earliest convenience;
  void terminate();

  bool process(QString &out, const QString &line); // handle anything, dispatch 'eval' or 'expand' appropriately
  QString eval(const QString &expr);  // evaluate an expression, line begins with %
  QString expand(QString expr); // expand variables, syntax is %{var}

  bool setVariable(const QString &symbol, double value);
  double getVariable(const QString &symbol);

  bool terminated();
  struct Privates;

 private:
  Privates *m_p;
};

/* end of github/elh/Candle/src/macros/macroprocessor.h */

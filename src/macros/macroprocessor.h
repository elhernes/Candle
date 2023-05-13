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

#include "../machinecontrol.h"

#include <QObject>
#include <QString>

class MacroProcessor : public QObject {
  Q_OBJECT
 public:
  MacroProcessor(MachineControl *mc);
  ~MacroProcessor();

  bool process(const QString &macroText);

  bool setVariable(const QString &symbol, double value);
  double getVariable(const QString &symbol);

  void onNoCommandsPending();

  struct Privates;

 signals:
  void finished();


 private:
  bool processNext();
  QString eval(const QString &expr);  // evaluate an expression, line begins with %
  QString expand(QString expr); // expand variables, syntax is %{var}
  Privates *m_p;
};

/* end of github/elh/Candle/src/macros/macroprocessor.h */

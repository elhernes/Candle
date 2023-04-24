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

#include <QObject>
#include <QString>
class frmMain;

class MacroProcessor : public QObject {
  Q_OBJECT
 public:
  MacroProcessor(frmMain *frm);
  ~MacroProcessor();

  //  bool process(QString &out, const QString &line); // handle anything, dispatch 'eval' or 'expand' appropriately
  bool process(const QString &macroText);

  bool setVariable(const QString &symbol, double value);
  double getVariable(const QString &symbol);

  struct Privates;

 signals:
  void finished();

  private slots:
    void onStatusChanged(const QString &newStatus);

 private:
  bool processNext();
  QString eval(const QString &expr);  // evaluate an expression, line begins with %
  QString expand(QString expr); // expand variables, syntax is %{var}
  Privates *m_p;
};

/* end of github/elh/Candle/src/macros/macroprocessor.h */

/***************************************************
 * file: github/elh/Candle/src/tests/testmc.h
 *
 * @file    testmc.h
 * @author  Eric L. Hernes
 * @born_on   Saturday, May 13, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C/C++ header
 *
 * $Id$
 */

#pragma once

#include <QString>
#include <QVector3D>

#include "../machinecontrol.h"

class TestMC : public QObject, public MachineControl {
  Q_OBJECT
 public:
  TestMC();
  ~TestMC();

  // public methods for macro processor
  virtual QString getModalState() override;
  virtual void setModalState() override;

  virtual QVector3D workPos() override;
  virtual void setWorkPos(const QVector3D &pos) override;
  virtual QVector3D machinePos() override;

  virtual void goAbsoluteWork(const QVector3D &pos, bool jogp=true) override;
  virtual void goAbsoluteMachine(const QVector3D &pos, bool jogp=true) override;
    
  virtual void goRelative(const QVector3D &pos, bool jogp=true) override; // machine and work relative are the same...

  virtual void setSpindle(double speed) override;
  virtual double spindle() override;
  virtual void setJogFeed(double feed) override;
  virtual double jogFeed() override;

  virtual bool macroText(QString &text, const QString &name) override;

  virtual const QString status() override;
  virtual size_t commandsPending() override;
  virtual void sendCommand(QString command, int tableIndex = -1, bool showInConsole = true) override;

  struct Privates;
 signals:
  void controllerIdle();

 private:
  Privates *m_p;
};


/* end of github/elh/Candle/src/tests/testmc.h */

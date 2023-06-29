/***************************************************
 * file: github/elh/Candle/src/machinecontrol.h
 *
 * @file    machinecontrol.h
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

#include <QObject>
#include <QVector3D>

/*
 * Define this separately so we can use it for tests outside of Candle's frmMain 
 */

class MachineControl {
 public:
  // pseudo-slots, called on events from outside
  void onNoCommandsPending();

  // pure virtual methods
  virtual QString getModalState() =0;;
  virtual void setModalState(const QString &state) =0;

  virtual QVector3D workPos() =0;
  virtual void setWorkPos(const QVector3D &pos) =0;
  virtual QVector3D machinePos() =0;

  virtual void goAbsoluteWork(const QVector3D &pos, bool jogp=true) =0;
  virtual void goAbsoluteMachine(const QVector3D &pos, bool jogp=true) =0;
    
  virtual void goRelative(const QVector3D &pos, bool jogp=true) =0; // machine and work relative are the same...

  virtual void setSpindle(double speed) =0;
  virtual double spindle() =0;
  virtual void setJogFeed(double feed) =0;
  virtual double jogFeed() =0;

    /*
     * name is:
     *   z-touch
     *   xyz-probe
     *   safe-z
     *   user0
     *   user1
     *   user2
     *   user3
     */
  virtual bool macroText(QString &text, const QString &name) =0;

  virtual const QString status() =0;
  virtual size_t commandsPending() =0;
  virtual void sendCommand(QString command, int tableIndex = -1, bool showInConsole = true) =0;

};

/* end of github/elh/Candle/src/machinecontrol.h */

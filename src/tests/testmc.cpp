/***************************************************
 * file: github/elh/Candle/src/tests/testmc.cpp
 *
 * @file    testmc.cpp
 * @author  Eric L. Hernes
 * @version V1.0
 * @born_on   Saturday, May 13, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C++ module
 *
 */

#include "testmc.h"
#include <QDebug>

struct TestMC::Privates {
  QVector3D machPos;
  QVector3D workPos;
  double feed;
  double spindle;
};

TestMC::TestMC() {
  m_p = new Privates;
  m_p->machPos = { 0, 0, 0 };
  m_p->workPos = { 0, 0, 0 };
  m_p->feed = 1000;
  m_p->spindle = 10000;
}

TestMC::~TestMC() {
  delete m_p;
}

QString
TestMC::getModalState() {
  return "stored modal state";
}

void
TestMC::setModalState(const QString &state) {
}

QVector3D
TestMC::workPos() {
  return m_p->workPos;
}

void
TestMC::setWorkPos(const QVector3D &pos) {
  m_p->workPos = pos;
}

QVector3D
TestMC::machinePos() {
  return m_p->machPos;
}

void
TestMC::goAbsoluteWork(const QVector3D &pos, bool jogp) {
}

void
TestMC::goAbsoluteMachine(const QVector3D &pos, bool jogp) {
}
    
void
TestMC::goRelative(const QVector3D &pos, bool jogp) {
}

void
TestMC::setSpindle(double speed) {
  m_p->spindle = speed;
}

double
TestMC::spindle() {
  return m_p->spindle;
}

void
TestMC::setJogFeed(double feed) {
  m_p->feed = feed;
}

double
TestMC::jogFeed() {
  return m_p->feed;
}

bool
TestMC::macroText(QString &text, const QString &name) {
  return false;
}

const QString
TestMC::status() {
  return "Idle";
}

size_t
TestMC::commandsPending() {
  return 0;
}

void
TestMC::sendCommand(QString command, int tableIndex, bool showInConsole) {
  qDebug()  << "sendCommand: '" << command << "' (" << tableIndex << ")";
  emit noCommandsPending();
}


/* end of github/elh/Candle/src/tests/testmc.cpp */

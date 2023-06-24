/***************************************************
 * file: Candle/src/rpn-lang/rpn-cnc.cpp
 *
 * @file    rpn-cnc.cpp
 * @author  Eric L. Hernes
 * @version V1.0
 * @born_on   Friday, May 19, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C++ module
 *
 */

#include "../rpn.h"
#include "../rpn-cnc.h"

struct rpn::MachineInterface::Privates : public WordContext {
  Privates(rpn::Interp &rpn, MachineControl &mc);

  rpn::Interp &_rpn;
  MachineControl &_mc;
};

rpn::MachineInterface::MachineInterface(rpn::Interp &rpn, MachineControl &mc) {
  m_p = new Privates(rpn, mc);
}

rpn::MachineInterface::~MachineInterface() {
  delete m_p;
}

NATIVE_WORD_DECL(cnc, MPOS_push) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  auto pos = p->_mc.machinePos();
  rpn.stack.push(StVec3(pos.x(), pos.y(), pos.z()));
  return rv;
}

NATIVE_WORD_DECL(cnc, WPOS_push) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  auto pos = p->_mc.workPos();
  rpn.stack.push(StVec3(pos.x(), pos.y(), pos.z()));
  return rv;
}

NATIVE_WORD_DECL(cnc, set_WPOS) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  auto obj = rpn.stack.pop();
  const auto &pos = POP_CAST(StVec3,obj);
  p->_mc.setWorkPos(QVector3D(pos._x, pos._y, pos._z));
  return rv;
}

NATIVE_WORD_DECL(cnc, SPEED_push) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  rpn.stack.push_double(p->_mc.spindle());
  return rv;
}

NATIVE_WORD_DECL(cnc, set_SPEED) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  double speed = rpn.stack.pop_as_double();
  p->_mc.setSpindle(speed);
  return rv;
}

NATIVE_WORD_DECL(cnc, FEED_push) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  rpn.stack.push_double(p->_mc.jogFeed());
  return rv;
}

NATIVE_WORD_DECL(cnc, set_FEED) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  double feed = rpn.stack.pop_as_double();
  p->_mc.setJogFeed(feed);
  return rv;
}

NATIVE_WORD_DECL(cnc, JOG_rel) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  auto obj = rpn.stack.pop();
  const auto &pos = POP_CAST(StVec3,obj);
  p->_mc.goRelative(QVector3D(pos._x, pos._y, pos._z));
  return rv;
}

NATIVE_WORD_DECL(cnc, JOG_work) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  auto obj = rpn.stack.pop();
  const auto &pos = POP_CAST(StVec3,obj);
  p->_mc.goAbsoluteWork(QVector3D(pos._x, pos._y, pos._z));
  return rv;
}

NATIVE_WORD_DECL(cnc, JOG_mach) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  auto obj = rpn.stack.pop();
  const auto &pos = POP_CAST(StVec3,obj);
  p->_mc.goAbsoluteMachine(QVector3D(pos._x, pos._y, pos._z));
  return rv;
}

NATIVE_WORD_DECL(cnc, PROBE) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::implementation_error;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  return rv;
}

NATIVE_WORD_DECL(cnc, MODALSTATE_push) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::implementation_error;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  //  QString ms = p->_mc.getModalState();
  // rpn.stack.push_string(ms.toStdString());
  return rv;
}

NATIVE_WORD_DECL(cnc, set_MODALSTATE) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::implementation_error;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string ms = rpn.stack.pop_string();
  //  p->_mc.setModalState(QString::fromStdString(ms));
  return rv;
}

NATIVE_WORD_DECL(cnc, GSEND) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::implementation_error;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  return rv;
}

rpn::MachineInterface::Privates::Privates(rpn::Interp &rpn, MachineControl &mc)  : _rpn(rpn), _mc(mc) {
  rpn.addDefinition("MPOS->", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, MPOS_push, this));
  rpn.addDefinition("WPOS->", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, WPOS_push, this));
  rpn.addDefinition("->WPOS", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, set_WPOS, this));
  rpn.addDefinition("SPEED->", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, SPEED_push, this));
  rpn.addDefinition("->SPEED", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, set_SPEED, this));
  rpn.addDefinition("FEED->", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, FEED_push, this));
  rpn.addDefinition("->FEED", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, set_FEED, this));

  rpn.addDefinition("JOG-REL", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, JOG_rel, this));
  rpn.addDefinition("JOG-WORK", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, JOG_work, this));
  rpn.addDefinition("JOG-MACH", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, JOG_mach, this));

  rpn.addDefinition("PROBE", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, PROBE, this));

  rpn.addDefinition("MODAL->", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, MODALSTATE_push, this));
  rpn.addDefinition("->MODAL", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_string, set_MODALSTATE, this));

  rpn.addDefinition("GSEND", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_string, GSEND, this));

}

/* end of Candle/src/rpn-lang/rpn-cnc.cpp */

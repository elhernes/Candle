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

#include "rpn-cnc.h"

struct rpn::MachineInterface::Privates : public WordContext {
  Privates(rpn::Interp &rpn, MachineControl &mc);

  rpn::Interp &_rpn;
  MachineControl &_mc;
  rpn::KeypadController *_keypad;
};

rpn::MachineInterface::MachineInterface(rpn::Interp &rpn, MachineControl &mc) {
  m_p = new Privates(rpn, mc);
}

rpn::MachineInterface::~MachineInterface() {
  delete m_p;
}

void
rpn::MachineInterface::setKeypad(rpn::KeypadController *keypad) {
  m_p->_keypad = keypad;
  if (m_p->_keypad != nullptr) {

    m_p->_keypad->assignMenu("Keys", "jog-keys", "Jog");
    m_p->_keypad->assignMenu("Keys", "probe-keys", "Probe");
    m_p->_rpn.eval("jog-keys");
  }
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

NATIVE_WORD_DECL(cnc, FEED_xy_push) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  rpn.stack.push_double(p->_mc.xyJogFeed());
  return rv;
}

NATIVE_WORD_DECL(cnc, set_FEED_xy) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  double feed = rpn.stack.pop_as_double();
  p->_mc.setXYJogFeed(feed);
  return rv;
}

NATIVE_WORD_DECL(cnc, FEED_z_push) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  rpn.stack.push_double(p->_mc.zJogFeed());
  return rv;
}

NATIVE_WORD_DECL(cnc, set_FEED_z) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  double feed = rpn.stack.pop_as_double();
  p->_mc.setZJogFeed(feed);
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

NATIVE_WORD_DECL(cnc, JOG_KEYS) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  rpn::WordDefinition::Result rv = (p->_keypad != nullptr) ? rpn::WordDefinition::Result::ok : rpn::WordDefinition::Result::eval_error;
  if (p->_keypad != nullptr) {
    p->_keypad->clearAssignedButtons();
    p->_keypad->assignButton(1,2, "MPOS->");
    p->_keypad->assignButton(1,3, "WPOS->");
    p->_keypad->assignButton(1,4, "->WPOS");
    p->_keypad->assignButton(1,5, "SPEED->");
    p->_keypad->assignButton(2,5, "->SPEED");
    p->_keypad->assignButton(1,6, "xyFEED->");
    p->_keypad->assignButton(2,6, "->xyFEED");
    p->_keypad->assignButton(1,7, "zFEED->");
    p->_keypad->assignButton(2,7, "->zFEED");
    p->_keypad->assignButton(1,9, "JOG-REL", "JOG-R");
    p->_keypad->assignButton(1,10, "JOG-WRK", "JOG-W");

    p->_keypad->assignButton(2,2, "JOG-MAC", "JOG-M");
    p->_keypad->assignButton(2,3, "MODAL->", "MODL->");
    p->_keypad->assignButton(2,4, "->MODL", "->MODL");
    p->_keypad->assignButton(2,5, "GSEND");
  }
  return rv;
}

NATIVE_WORD_DECL(cnc, PROBE_KEYS) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  rpn::WordDefinition::Result rv = (p->_keypad != nullptr) ? rpn::WordDefinition::Result::ok : rpn::WordDefinition::Result::eval_error;
  if (p->_keypad != nullptr) {
    p->_keypad->clearAssignedButtons();
#if 0
    p->_keypad->assignButton(1,2, "PROBE-NWi", "↖");  p->_keypad->assignButton(2,2, "PROBE-Ni", "⬆");  p->_keypad->assignButton(3,2, "PROBE-NEi", "↗");
    p->_keypad->assignButton(1,3, "PROBE-Wi", "⬅");   p->_keypad->assignButton(2,3, "PROBE-C", "X");  p->_keypad->assignButton(3,3, "PROBE-Ei", "➡");
    p->_keypad->assignButton(1,4, "PROBE-SWi", "↙");  p->_keypad->assignButton(2,4, "PROBE-Si", "⬇");  p->_keypad->assignButton(3,4, "PROBE-SEi", "↘");

    p->_keypad->assignButton(1,6, "PROBE-NWo", "↘");  p->_keypad->assignButton(2,6, "PROBE-No", "⬇");  p->_keypad->assignButton(3,6, "PROBE-NEo", "↙");
    p->_keypad->assignButton(1,7, "PROBE-Wo", "➡");   p->_keypad->assignButton(2,7, "PROBE-C", "X");   p->_keypad->assignButton(3,7, "PROBE-Eo", "⬅");
    p->_keypad->assignButton(1,8, "PROBE-SWo", "↗");  p->_keypad->assignButton(2,8, "PROBE-So", "⬆");  p->_keypad->assignButton(3,8, "PROBE-SEo", "↖");
#endif
    p->_keypad->assignButton(4,2, "PROBE-XYZ", "P->XYZ");

    p->_keypad->assignButton(1,1, "PROBE-NWi", "↖");  p->_keypad->assignButton(2,1, "PROBE-NEi", "↗");
    p->_keypad->assignButton(1,2, "PROBE-SWi", "↙");  p->_keypad->assignButton(2,2, "PROBE-SEi", "↘");

    p->_keypad->assignButton(1,4, "PROBE-NWo", "↘");  p->_keypad->assignButton(2,4, "PROBE-NEo", "↙");
    p->_keypad->assignButton(1,5, "PROBE-SWo", "↗");  p->_keypad->assignButton(2,5, "PROBE-SEo", "↖");

                                                       p->_keypad->assignButton(2,7, "PROBE-N", "⬆");
    p->_keypad->assignButton(1,8, "PROBE-W", "⬅");   p->_keypad->assignButton(2,8, "PROBE-Z", "Z");  p->_keypad->assignButton(3,8, "PROBE-E", "➡");
                                                       p->_keypad->assignButton(2,9, "PROBE-S", "⬇");

  }

  return rv;
}

rpn::MachineInterface::Privates::Privates(rpn::Interp &rpn, MachineControl &mc)  : _rpn(rpn), _mc(mc), _keypad(nullptr) {
  _rpn.addDefinition("MPOS->", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, MPOS_push, this));
  _rpn.addDefinition("WPOS->", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, WPOS_push, this));
  _rpn.addDefinition("->WPOS", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, set_WPOS, this));
  _rpn.addDefinition("SPEED->", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, SPEED_push, this));
  _rpn.addDefinition("->SPEED", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, set_SPEED, this));
  _rpn.addDefinition("xyFEED->", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, FEED_xy_push, this));
  _rpn.addDefinition("->xyFEED", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_double, set_FEED_xy, this));
  _rpn.addDefinition("->xyFEED", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_integer, set_FEED_xy, this));
  _rpn.addDefinition("zFEED->", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, FEED_z_push, this));
  _rpn.addDefinition("->zFEED", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_double, set_FEED_z, this));
  _rpn.addDefinition("->zFEED", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_integer, set_FEED_z, this));

  _rpn.addDefinition("JOG-REL", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, JOG_rel, this));
  _rpn.addDefinition("JOG-WROK", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, JOG_work, this));
  _rpn.addDefinition("JOG-MACH", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_vec3, JOG_mach, this));

  _rpn.addDefinition("PROBE", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d2_vec3_double, PROBE, this));

  _rpn.addDefinition("MODAL->", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, MODALSTATE_push, this));
  _rpn.addDefinition("->MODAL", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_string, set_MODALSTATE, this));

  _rpn.addDefinition("GSEND", NATIVE_WORD_WDEF(cnc, rpn::StrictTypeValidator::d1_string, GSEND, this));

  _rpn.addDefinition("jog-keys", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, JOG_KEYS, this));
  _rpn.addDefinition("probe-keys", NATIVE_WORD_WDEF(cnc, rpn::StackSizeValidator::zero, PROBE_KEYS, this));

  _rpn.eval(R"(

: PROBE-XYZ ;

: PROBE-NWi ;
: PROBE-NEi ;
: PROBE-SWi ;
: PROBE-SEi ;
: PROBE-NWo ;
: PROBE-NEo ;
: PROBE-SWo ;
: PROBE-SEo ;

: PROBE-N ;
: PROBE-W ;
: PROBE-Z ;
: PROBE-E ;
: PROBE-S ;
)");  

}

/* end of Candle/src/rpn-lang/rpn-cnc.cpp */

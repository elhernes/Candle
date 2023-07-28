/***************************************************
 * file: qinc/rpn-lang/rpn-cnc.h
 *
 * @file    rpn-cnc.h
 * @author  Eric L. Hernes
 * @born_on   Friday, May 19, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C/C++ header
 *
 * $Id$
 */

#pragma once

#include "rpn-lang/rpn.h"
#include "rpn-lang/ui/qtkeypad.h"
#include "machinecontrol.h"

// machine control wrapper / adapter
namespace rpn {
  class MachineInterface {
  public:
    MachineInterface(rpn::Interp &rpn, MachineControl &mc);
    ~MachineInterface();
    void setKeypad(rpn::KeypadController *keypad);

    struct Privates;
  private:
    Privates *m_p;
  };
}

/* end of qinc/rpn-lang/rpn-cnc.h */

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

#if 0

NATIVE_WORD_DECL(cnc, word) {
  rpn::MC *mc = dynamic_cast<rpn::MC*>(ctx);
}

NATIVE_WORD_IMPL(MPOS_to) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  rpn.stack_push(Vec3(3.4, 4.5, 5.6));
  return 0;
}

NATIVE_WORD_IMPL(WPOS_to) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  rpn.stack_push(Vec3(1.2, 2.3, 3.4));
  return 0;
}

NATIVE_WORD_IMPL(to_WPOS) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  Vec3 np = rpn.stack_pop_as_vec3();
  printf("->WPOS: popped '%s' from stack\n", to_string(np).c_str());
  return 0;
}

NATIVE_WORD_IMPL(SPEED_to) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string::size_type rv = 0;
  rpn.stack_push(10000);
  return rv;
}

NATIVE_WORD_IMPL(to_SPEED) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string::size_type rv = 0;
  return rv;
}

NATIVE_WORD_IMPL(FEED_to) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string::size_type rv = 0;
  return rv;
}

NATIVE_WORD_IMPL(to_FEED) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string::size_type rv = 0;
  return rv;
}

NATIVE_WORD_IMPL(JOG_R) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string::size_type rv = 0;
  return rv;
}

NATIVE_WORD_IMPL(JOG_WA) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string::size_type rv = 0;
  return rv;
}

NATIVE_WORD_IMPL(JOG_MA) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string::size_type rv = 0;
  return rv;
}

NATIVE_WORD_IMPL(PROBE) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string::size_type rv = 0;
  return rv;
}

NATIVE_WORD_IMPL(MODALSTATE_to) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string::size_type rv = 0;
  return rv;
}

NATIVE_WORD_IMPL(to_MODALSTATE) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string::size_type rv = 0;
  return rv;
}

NATIVE_WORD_IMPL(SEND) {
  rpn::MachineInterface::Privates *p = dynamic_cast<rpn::MachineInterface::Privates*>(ctx);
  std::string::size_type rv = 0;
  printf("instance is %p\n", p);
  return rv;
}

#define PRIVATE_NATIVE_WORD_FN(sym) NATIVE_WORD_FN(sym), this

rpn::MachineInterface::Privates *rpn::MachineInterface::Privates::s_instance = nullptr;
#endif

rpn::MachineInterface::Privates::Privates(rpn::Interp &rpn, MachineControl &mc)  : _rpn(rpn), _mc(mc) {
}

#if 0
  rpn.addDictionary({
      //
      // Machine control words
      //
    { "MPOS->", {
	"Push Machine Position to the stack. ( -- mpos )", {
	  WORD_PARAMS_0(),
	    },
	  PRIVATE_NATIVE_WORD_FN(MPOS_to)
      } },

    { "WPOS->", {
	"Push Work Position to the stack. ( -- wpos )", {
	  WORD_PARAMS_0(),
	    },
	  PRIVATE_NATIVE_WORD_FN(WPOS_to)
      } },

    { "->WPOS", {
	"Set Work Position ( wpos -- )", {
	  { { "newpos", st_vec3 } },
	    },
	  PRIVATE_NATIVE_WORD_FN(to_WPOS)
      } },

    { "SPEED->", {
	"Push Spindle Speed to the stack. ( -- speed )", {
	  WORD_PARAMS_0(),
	    },
	  PRIVATE_NATIVE_WORD_FN(SPEED_to)
      } },

    { "->SPEED", {
	"Set Spindle Speed ( speed -- )", {
	  { { "speed", st_number } },
	    },
	  PRIVATE_NATIVE_WORD_FN(to_SPEED)
      } },

    { "FEED->", {
	"Push jog feed rate to the stack. ( -- feed )", {
	  WORD_PARAMS_0(),
	    },
	  PRIVATE_NATIVE_WORD_FN(FEED_to)
      } },

    { "->FEED", {
	"Set jog feed rate ( feed -- )", {
	  { { "feed", st_number } },
	    },
	  PRIVATE_NATIVE_WORD_FN(to_FEED)
      } },

    { "JOG-R", {
	"Jog to relative position ( pos -- )", {
	  { { "offset", st_vec3 } },
	    },
	  PRIVATE_NATIVE_WORD_FN(JOG_R)
      } },

    { "JOG-WA", {
	"Jog to absolute work position ( wpos -- )", {
	  { { "wpos", st_vec3 } },
	    },
	  PRIVATE_NATIVE_WORD_FN(JOG_WA)
      } },

    { "JOG-MA", {
	"Jog to absolute machine position ( mpos -- )", {
	  { { "mpos", st_vec3 } },
	    },
	  PRIVATE_NATIVE_WORD_FN(JOG_MA)
      } },

      { "PROBE", {
	"Probe machine (target feed -- )", {
	  { { "target", st_vec3 }, { "feed" , st_number } }, // primitive G38.2
	    },
	  PRIVATE_NATIVE_WORD_FN(PROBE)
      } },

      { "MODAL-STATE->", {
	  "Push machine's modal state on the stack ( -- state )", {
	    WORD_PARAMS_0(),
	      },
	  PRIVATE_NATIVE_WORD_FN(MODALSTATE_to)
	} },

    { "->MODAL-STATE", {
	"Send modal state to the machine ( state -- )", {
	  { { "state", st_string } },
	    },
	  PRIVATE_NATIVE_WORD_FN(to_MODALSTATE)
      } },


    { "SEND", {
	"Send command", {
	  { { "g-code", st_string } },
	    },
	  PRIVATE_NATIVE_WORD_FN(SEND)
      } }
    });
}
#endif

/* end of Candle/src/rpn-lang/rpn-cnc.cpp */

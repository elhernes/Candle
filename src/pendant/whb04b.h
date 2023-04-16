/***************************************************
 * file: Candle/src/pendant/whb04b.h
 */
/**
 * @file    whb04b.h
 * @author  Eric L. Hernes
 * @born_on   Monday, April 3, 2023
 * @copyright (C) Copyright Q, Inc 2023
 *
 * @brief   An Eric L. Hernes Signature Series C/C++ header
 *
 */

#pragma once

#include <QThread>
#include <string>

class WHB04B : public QThread {
  Q_OBJECT

 public:
  WHB04B();
  ~WHB04B();

  enum keycode_t {
    key_none = 0x00,
    key_reset = 0x01,
    key_stop = 0x02,
    key_start_pause = 0x03,
    key_feed_up = 0x04,
    key_feed_down = 0x05,
    key_spindle_up = 0x06,
    key_spindle_down = 0x07,
    key_m_home = 0x08,
    key_safe_z = 0x09,
    key_w_home = 0x0a,
    key_s_on_off = 0x0b,
    key_fn = 0x0c,
    key_probe_z = 0x0d,
    key_continuous = 0x0e,
    key_step = 0x0f
  };

  // axis controller
  enum axis_t {
    axis_off = 0x06,
    axis_x = 0x11,
    axis_y = 0x12,
    axis_z = 0x13,
    axis_a = 0x14,
    axis_clr = 0x17,
  };

  // step/con controller
  enum stepcon_t {
    stepcon_0_001 = 0x0d, // "D_STEP_0_001"
    stepcon_0_01 = 0x0e,  // "D_STEP_0_01"
    stepcon_0_1 = 0x0f,   // "D_STEP_0_1"
    stepcon_1 = 0x10,     // "D_STEP_1"
    stepcon_60 = 0x1a,    // "D_STEP_60"
    stepcon_100 = 0x1b,   // "D_STEP_100"
    stepcon_load = 0x9b,  // "D_STEP_LOAD"
  };

  bool display(bool wm_coord, double axis1, double axis2, double axis3, double feed, double spindle);
  void stop();

 signals:
  //  void event(WHB04B::keycode_t button1, WHB04B::keycode_t button2, WHB04B::axis_t axis, WHB04B::stepcon_t stepcon, int count);
  void event(quint8 b1, quint8 b2, quint8 axis, quint8 stepcon, int count);


 private:
  void run() override;

 private:
  void ll_event(WHB04B::keycode_t button1, WHB04B::keycode_t button2, WHB04B::axis_t axis, WHB04B::stepcon_t stepcon, int count);
  struct Privates;
  Privates *m_p;
  friend Privates;
};

/* end of Candle/src/pendant/whb04b.h */

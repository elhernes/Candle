/***************************************************
 * file: Candle/src/pendant/whb04b.cpp
 *
 * @file    whb04b.cpp
 * @author  Eric L. Hernes
 * @version V1.0
 * @born_on   Thursday, April 13, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C++ module
 *
 */

#include <iostream>
#include <cerrno>
#include <cstdio>
#include <iomanip>
#include <stdlib.h>
#include <cstring>
#include <cmath>

#include "whb04b.h"
#include "hidapi/hidapi/hidapi.h"
#include <unistd.h>
#include <sys/time.h>

#define WHB04B_VENDOR 0x10CE
#define WHB04B_PRODUCT 0xEB93

struct WHB04B::Privates {
  void hidloop(hid_device*, WHB04B*);
  bool display_encode(uint8_t ind_flags, bool wm_coord, double axis1, double axis2, double axis3, double feed, double spindle);
  bool process(const uint8_t *data, size_t len, WHB04B *whbp);

  uint8_t m_ldata[8]; // last data from device

  uint8_t m_displaydata[3*8];
  bool m_displaydataChanged;

  bool m_verbose;
  bool m_stop;
};

WHB04B::WHB04B() {
  m_p = new Privates;
  m_p->m_displaydataChanged = false;
  memset(m_p->m_displaydata, 0, 3*8);
  memset(m_p->m_ldata, 0, 8);
  m_p->m_verbose=false;
  m_p->m_stop = false;
}

WHB04B::~WHB04B() {
  if (m_p) {
    delete m_p;
  }
}

bool
WHB04B::display(bool wm_coord, double axis1, double axis2, double axis3, double feed, double spindle) {
  return m_p->display_encode(0, wm_coord, axis1, axis2, axis3, feed, spindle);
}

void
WHB04B::run() {
  hid_device *handle;

  bool showmessage = true;
  while(m_p->m_stop == false) {
    if (hid_init()) {
      std::cerr << "Unable to initialize HIDAPI " << std::strerror(errno) << std::endl;
    } else {
      handle = hid_open(WHB04B_VENDOR, WHB04B_PRODUCT, NULL);
      if (!handle) {
	if (showmessage) {
	  fprintf(stderr, "Unable to open WHB04B device V:0x%04x P:0x%04x - %s\n", WHB04B_VENDOR, WHB04B_PRODUCT, std::strerror(errno));
	  showmessage=false;
	}
      } else {
        // Process HID messages
        m_p->hidloop(handle, this);
        hid_close(handle);
	showmessage=true;
      }

      // Clean up
      hid_exit();
    }

    // Delay before trying again
    usleep(1000*1000);
  }

}

void
WHB04B::stop() {
  m_p->m_stop = true;
}

void
WHB04B::ll_event(WHB04B::keycode_t button1, WHB04B::keycode_t button2, WHB04B::axis_t axis, WHB04B::stepcon_t stepcon, int count) {
  emit pendant_event(button1, button2, axis, stepcon, count);
  //  printf("WHB04B::%s [b1 0x%02x] [b2 0x%02x] [stepcon %02x] [axis %02x] [count %d]\n", __func__,
  //	 button1, button2, stepcon, axis, count);
}

void
WHB04B::Privates::hidloop(hid_device *handle, WHB04B *whb04b) {
  int res;
  unsigned char buf[256];

  // Make HID reads nonblocking
  hid_set_nonblocking(handle, 1);

  // Loop until read fails
  while (m_stop == false) {
    memset(buf, 0, sizeof(buf));
    res = hid_read_timeout(handle, buf, sizeof(buf), 1000);
    if( res == 0 ) {
      continue;
    }

    if( res < 0 ) {
      return;
    }

    if (process(buf,res, whb04b)) {
    }

    if (m_displaydataChanged) {
      int bytes = hid_write(handle, m_displaydata, 3*8);
      if (bytes<0) {
	printf("Unable to write(): %ls\n", hid_error(handle));
      } else {
	m_displaydataChanged = false;
      }
    }
  }
}

bool
WHB04B::Privates::process(const uint8_t *data, size_t len, WHB04B *whbp) {
  if (len != 8) {
    return false;
  }

  if (memcmp(m_ldata, data, len) == 0) {

    return false;

  } else {

    timeval now;
    gettimeofday(&now, NULL);
    double ts = now.tv_sec + now.tv_usec/1000000.;

    uint8_t b1 = data[2]; // function
    uint8_t b2 = data[3]; // code
    uint8_t stepcon = data[4];
    uint8_t axis = data[5];
    int8_t count = (int8_t)data[6];
    uint8_t checksum = data[7]; // keycode or 0x10 indicating non-key action (?)

    whbp->ll_event((keycode_t)b1, (keycode_t)b2, (axis_t)axis, (stepcon_t)stepcon, count);

    memcpy(m_ldata, data, len);
  }
  return true;
}

static int
xhc_encode_float(float v, unsigned char *buf) {
  unsigned int int_v = (int)(fabs(v) * 10000.0);
  unsigned short int_part = int_v / 10000;
  unsigned short fract_part = int_v % 10000;
  if (v < 0) fract_part = fract_part | 0x8000;
  *(short *)buf = int_part;
  *((short *)buf+1) = fract_part;
  return 4;
}

static int
xhc_encode_s16(int v, unsigned char *buf) {
  *(short *)buf = v;
  return 2;
}

bool
WHB04B::Privates::display_encode(uint8_t ind_flags, bool wm_coord, double axis1, double axis2, double axis3, double feed, double spindle) {
  uint8_t data[3*8];
  unsigned char buf[3*7];
  unsigned char *p = buf;

  memset(buf, 0, sizeof(buf));
  memset(data, 0, 3*8);

  *p++ = 0xFE; // 0
  *p++ = 0xFD; // 1
  *p++ = 0x0C; // 2 seed
  *p++ = (((ind_flags&0x3) << 0) | // indicator flags (0 = continuous, 1=step, 2=XX%, 3=??)
	  ((0x00 &0xF) << 2) |  // unknown (==0)
	  ((0x00 &0x1) << 6) | // unknown (buggy, reset =0)
	  ((wm_coord &0x1) << 7)); // 1==workpiece coords; 0==machine coords

  p += xhc_encode_float(std::round(1000 * (axis1)) / 1000, p); // 4,5,6,7
  p += xhc_encode_float(std::round(1000 * (axis2)) / 1000, p); // 8,9,10,11
  p += xhc_encode_float(std::round(1000 * (axis3)) / 1000, p); // 12,13,14,15

  p += xhc_encode_s16((int)std::round(feed), p); // 16, 17
  p += xhc_encode_s16((int)std::round(spindle), p); // 18, 19

  // Multiplex to 1+7 byte USB transactions

  p = buf;
  for (int packet=0; packet<3; packet++) {
    data[packet*8] = 0x06; // 0, 8, 16
    memcpy(data+(packet*8)+1, // 1,9,17
	   buf+(packet*7), 7); // 0, 7, 14
  }

  if (memcmp(data, m_displaydata, 3*8)!=0) {
    memcpy(m_displaydata, data, 3*8);
    m_displaydataChanged = true;
  }
  return m_displaydataChanged;
}

/* end of Candle/src/pendant/whb04b.cpp */


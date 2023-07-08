/***************************************************
 * file: qinc/rpn-lang/src/color-dictionary.cpp
 *
 * @file    color-dictionary.cpp
 * @author  Eric L. Hernes
 * @version V1.0
 * @born_on   Monday, May 22, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C++ module
 *
 */

datatypes:
Cie_XYZ;
Cie_xyY;
Eotf

class RgbSpace {
  Cie_xyY _red;
  Cie_xyY _green;
  Cie_xyY _blue;
  Cie_xyY _white;
  Eotf &_eotf;
};

class ReferenceRgb {
  double _red;
  double _green;
  double _blue;

  enum class State {
    signal_linear,
    light_linear,
  };
  State _state;

  RgbSpace &_space;
}

Words:


/* end of qinc/rpn-lang/src/color-dictionary.cpp */

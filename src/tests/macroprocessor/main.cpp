/***************************************************
 * file: Candle/src/tests/macroprocessor/main.cpp
 *
 * @file    main.cpp<macroprocessor>
 * @author  Eric L. Hernes
 * @version V1.0
 * @born_on   Saturday, May 13, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C++ module
 *
 */

#include <QApplication>
#include <QStack>
#include "../../macros/macroprocessor.h"
#include "../testmc.h"

int
main(int ac, char **av) {
  QApplication app(ac, av);

  TestMC tmc;
  QStack<float> stack;
  MacroProcessor mproc(&tmc);

  QObject::connect(&mproc, &MacroProcessor::finished, []() {
      exit(0);
    });

  QObject::connect(&tmc, &TestMC::noCommandsPending,
		   [&mproc]() {  mproc.onNoCommandsPending(); } );

  QVector3D mpos = tmc.machinePos();
  mproc.setVariable("mposx", mpos.x());
  mproc.setVariable("mposy", mpos.y());
  mproc.setVariable("mposz", mpos.z());

  QVector3D wpos = tmc.workPos();
  mproc.setVariable("posx", wpos.x());
  mproc.setVariable("posy", wpos.y());
  mproc.setVariable("posz", wpos.z());

  QVector3D axisMin = {0, 0, 0};
  QVector3D axisMax = {700, 700, 50};

  mproc.setVariable("xmin", axisMin.x());
  mproc.setVariable("xmax", axisMax.x());
  mproc.setVariable("ymin", axisMin.y());
  mproc.setVariable("ymax", axisMax.x());
  mproc.setVariable("zmin", axisMin.z());
  mproc.setVariable("zmax", axisMax.x());


  mproc.process("file:///Volumes/CSData/Development/github/elh/Candle/examples/xyz-probe.txt");

  return app.exec();
}

/* end of Candle/src/tests/macroprocessor/main.cpp */

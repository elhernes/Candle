#
# $ CMAKE_PREFIX_PATH=/usr/local/Cellar/qt@5/5.15.10_1/lib/cmake cmake -GXcode -Bx
#

cmake_minimum_required(VERSION 3.5.0)

project(candle VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(Qt5 COMPONENTS Widgets OpenGL Network REQUIRED)

add_executable(Candle MACOSX_BUNDLE
    src/main.cpp
    src/frmmain.cpp
    src/frmsettings.cpp
    src/frmabout.cpp
    src/drawers/gcodedrawer.cpp
    src/drawers/heightmapborderdrawer.cpp
    src/drawers/heightmapgriddrawer.cpp
    src/drawers/heightmapinterpolationdrawer.cpp
    src/drawers/origindrawer.cpp
    src/drawers/shaderdrawable.cpp
    src/drawers/tooldrawer.cpp
    src/parser/arcproperties.cpp
    src/parser/gcodeparser.cpp
    src/parser/gcodepreprocessorutils.cpp
    src/parser/gcodeviewparse.cpp
    src/parser/linesegment.cpp
    src/parser/pointsegment.cpp
    src/rpn-cnc.cpp
    src/rpn-lang/src/logic-dict.cpp
    src/rpn-lang/src/math-dict.cpp
    src/rpn-lang/src/rpn-interp.cpp
    src/rpn-lang/src/rpn-stack.cpp
    src/rpn-lang/src/stack-dict.cpp
    src/rpn-lang/src/types-dict.cpp
    src/rpn-lang/src/keypad-dict.cpp
    src/rpn-lang/ui/qtkeypad.cpp
    src/tables/gcodetablemodel.cpp
    src/tables/heightmaptablemodel.cpp
    src/widgets/colorpicker.cpp
    src/widgets/combobox.cpp
    src/widgets/groupbox.cpp
    src/widgets/scrollarea.cpp
    src/widgets/styledtoolbutton.cpp
    src/widgets/widget.cpp
    src/widgets/glwidget.cpp
    src/widgets/slider.cpp
    src/widgets/sliderbox.cpp
    src/drawers/selectiondrawer.cpp
    src/widgets/comboboxkey.cpp
    src/pendant/whb04b.cpp
    src/macros/macroprocessor.cpp
    src/main.cpp

    src/frmmain.ui
    src/frmsettings.ui
    src/frmabout.ui
    src/rpn-lang/ui/qtkeypad.ui
    src/widgets/sliderbox.ui

    src/libs/libMathExpr/error.cpp
    src/libs/libMathExpr/functions.cpp
    src/libs/libMathExpr/parser.cpp
    src/libs/libMathExpr/variablelist.cpp

)


if (${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
   target_sources(Candle PUBLIC
       src/pendant/hidapi/mac/hid.c
   )

  find_library(cff CoreFoundation)
  find_library(iok IOKit)
  find_library(ak AppKit)

  list(APPEND sys_libs ${cff} ${iok} ${ak})

  set_target_properties(Candle PROPERTIES
    BUNDLE True
    MACOSX_BUNDLE_GUI_IDENTIFIER tv.qinc.Candle
    MACOSX_BUNDLE_BUNDLE_NAME Candle
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
#    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Candle.plist.in
)
endif ()


if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)
   target_sources(Candle PUBLIC
       pendant/hidapi/windows/hid.c
   )
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL Linux)
   target_sources(Candle PUBLIC
       pendant/hidapi/linux/hid.c
   )
endif()

target_include_directories(Candle PUBLIC
  src
)
target_compile_definitions(Candle PUBLIC
  sNan="65536"
  APP_VERSION=\"$$VERSION\"
  UNIX
)

#qt5_add_translations(Candle TS_FILES
#  src/translations/candle_en.ts
#  src/translations/candle_ru.ts
#  src/translations/candle_es.ts
#  src/translations/candle_fr.ts
#  src/translations/candle_pt.ts
#)

qt5_use_modules(Candle Widgets Network OpenGL SerialPort)

target_link_libraries(Candle ${sys_libs})

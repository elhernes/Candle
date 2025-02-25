#-------------------------------------------------
#
# Project created by QtCreator 2014-05-23T17:51:21
#
#-------------------------------------------------

QT       = core gui opengl serialport network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

win32: {
    QT += winextras
    DEFINES += WINDOWS
#    QMAKE_LFLAGS += "-Wl,--large-address-aware"
    QMAKE_CXXFLAGS_DEBUG += -g3 -pg
    QMAKE_LFLAGS_DEBUG += -pg -lgmon
}

unix:!macx {
    DEFINES += UNIX #GL_GLEXT_PROTOTYPES
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/libs\'" -ludev
}

message(QT_CONFIG: $$QT_CONFIG)
#contains(QT_CONFIG, opengles.) {
#    warning("GL ES detected. VAO will be disabled.")
#    DEFINES += GLES
#    INSTALLS += target
#    target.path = /home/pi
#}

TARGET = Candle
TEMPLATE = app
VERSION = 1.2.0
RC_ICONS += images/candle.ico

ICON = candle.icns

DEFINES += sNan=\"65536\"
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

TRANSLATIONS += translations/candle_en.ts translations/candle_ru.ts translations/candle_es.ts translations/candle_fr.ts translations/candle_pt.ts

SOURCES += main.cpp\
        frmmain.cpp \
    frmsettings.cpp \
    frmabout.cpp \
    drawers/gcodedrawer.cpp \
    drawers/heightmapborderdrawer.cpp \
    drawers/heightmapgriddrawer.cpp \
    drawers/heightmapinterpolationdrawer.cpp \
    drawers/origindrawer.cpp \
    drawers/shaderdrawable.cpp \
    drawers/tooldrawer.cpp \
    parser/arcproperties.cpp \
    parser/gcodeparser.cpp \
    parser/gcodepreprocessorutils.cpp \
    parser/gcodeviewparse.cpp \
    parser/linesegment.cpp \
    parser/pointsegment.cpp \
    rpncalc/rpncalc.cpp \
    tables/gcodetablemodel.cpp \
    tables/heightmaptablemodel.cpp \
    widgets/colorpicker.cpp \
    widgets/combobox.cpp \
    widgets/groupbox.cpp \
    widgets/scrollarea.cpp \
    widgets/styledtoolbutton.cpp \
    widgets/widget.cpp \
    widgets/glwidget.cpp \
    widgets/slider.cpp \
    widgets/sliderbox.cpp \
    drawers/selectiondrawer.cpp \
    widgets/comboboxkey.cpp \
    pendant/whb04b.cpp \
    macros/macroprocessor.cpp

macx: {
SOURCES += pendant/hidapi/mac/hid.c
}

unix:!macx { # linux
SOURCES += pendant/hidapi/linux/hid.c
}

win32: {
SOURCES += pendant/hidapi/windows/hid.c
}

SOURCES += \
    libs/libMathExpr/error.cpp \
    libs/libMathExpr/functions.cpp \
    libs/libMathExpr/parser.cpp \
    libs/libMathExpr/variablelist.cpp


# QMAKE_APPLE_DEVICE_ARCHS=arm64

HEADERS  += frmmain.h \
    frmsettings.h \
    frmabout.h \
    drawers/gcodedrawer.h \
    drawers/heightmapborderdrawer.h \
    drawers/heightmapgriddrawer.h \
    drawers/heightmapinterpolationdrawer.h \
    drawers/origindrawer.h \
    drawers/shaderdrawable.h \
    drawers/tooldrawer.h \
    macros/macroprocessor.h \
    parser/arcproperties.h \
    parser/gcodeparser.h \
    parser/gcodepreprocessorutils.h \
    parser/gcodeviewparse.h \
    parser/linesegment.h \
    parser/pointsegment.h \
    pendant/whb04b.h \
    rpncalc/rpncalc.h \
    tables/gcodetablemodel.h \
    tables/heightmaptablemodel.h \
    utils/interpolation.h \
    utils/util.h \
    widgets/colorpicker.h \
    widgets/combobox.h \
    widgets/groupbox.h \
    widgets/scrollarea.h \
    widgets/styledtoolbutton.h \
    widgets/widget.h \
    widgets/glwidget.h \
    widgets/slider.h \
    widgets/sliderbox.h \
    drawers/selectiondrawer.h \
    widgets/comboboxkey.h

FORMS    += frmmain.ui \
    frmsettings.ui \
    frmabout.ui \
    rpncalc/rpncalc.ui \
    widgets/sliderbox.ui

DEFINES += _USE_MATH_DEFINES

RESOURCES += \
    shaders.qrc \
    images.qrc

CONFIG += c++11

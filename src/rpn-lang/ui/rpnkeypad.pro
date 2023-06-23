QT       = core gui uitools

TARGET = rpn-keypad-test
TEMPLATE = app

SOURCES += \
    main.cpp \
    $$PWD/../testmc.cpp \
    $$PWD/../../rpnkeypad/rpnkeypad.cpp

HEADERS  += \
    $$PWD/../testmc.h \
    $$PWD/../../rpnkeypad/rpnkeypad.h

FORMS += \
    $$PWD/../../rpnkeypad/rpnkeypad.ui


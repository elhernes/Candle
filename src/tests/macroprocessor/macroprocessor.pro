QT       = core gui uitools

TARGET = macro-test
TEMPLATE = app

SOURCES += \
    main.cpp \
    $$PWD/../testmc.cpp \
    $$PWD/../../macros/macroprocessor.cpp \
    $$PWD/../../libs/libMathExpr/error.cpp \
    $$PWD/../../libs/libMathExpr/functions.cpp \
    $$PWD/../../libs/libMathExpr/parser.cpp \
    $$PWD/../../libs/libMathExpr/variablelist.cpp

HEADERS  += \
    $$PWD/../testmc.h \
    $$PWD/../../macros/macroprocessor.h



QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        CORE/parameter.cpp \
        CORE/parameterlist.cpp \
        GE/cprogram.cc \
        GE/doublestack.cc \
        GE/fparser.cc \
        GE/fpoptimizer.cc \
        GE/grammargenetic.cpp \
        GE/integeranneal.cpp \
        GE/program.cc \
        GE/rule.cc \
        GE/symbol.cc \
        INTERVAL/interval.cpp \
        INTERVAL/intervalproblem.cpp \
        INTERVAL/problem.cpp \
        MLMODELS/dataset.cpp \
        MLMODELS/mlpproblem.cpp \
        MLMODELS/model.cpp \
        MLMODELS/rbfproblem.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    CORE/parameter.h \
    CORE/parameterlist.h \
    GE/cprogram.h \
    GE/doublestack.h \
    GE/fparser.hh \
    GE/fpconfig.hh \
    GE/fptypes.hh \
    GE/grammargenetic.h \
    GE/integeranneal.h \
    GE/program.h \
    GE/rule.h \
    GE/symbol.h \
    INTERVAL/interval.h \
    INTERVAL/intervalproblem.h \
    INTERVAL/problem.h \
    MLMODELS/dataset.h \
    MLMODELS/mlpproblem.h \
    MLMODELS/model.h \
    MLMODELS/rbfproblem.h

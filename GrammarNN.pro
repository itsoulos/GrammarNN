QT -= gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
QMAKE_CXXFLAGS_RELEASE += -O4 -fopenmp -march=native -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -ffast-math -fopt-info -Ofast
QMAKE_CFLAGS_RELEASE += -O4 -fopenmp -march=native -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native -ffast-math -Ofast

QMAKE_CXXFLAGS_RELEASE += -std=c++11
QMAKE_CFLAGS_RELEASE += -std=c++11

QMAKE_CXXFLAGS += -O4 -fopenmp -march=native -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native -ffast-math -Ofast
QMAKE_CFLAGS += -O4 -fopenmp -march=native -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -ffast-math -Ofast

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
        INTERVAL/intervalde.cpp \
        INTERVAL/intervalproblem.cpp \
        INTERVAL/intervalpso.cpp \
        INTERVAL/problem.cpp \
        METHODS/adam.cpp \
        METHODS/armijosearch.cpp \
        METHODS/bfgs.cpp \
        METHODS/collection.cpp \
        METHODS/doublebox.cpp \
        METHODS/editlogger.cpp \
        METHODS/fibonaccisearch.cpp \
        METHODS/filelogger.cpp \
        METHODS/genetic.cpp \
        METHODS/goldensearch.cpp \
        METHODS/gradientdescent.cpp \
        METHODS/ipso.cpp \
        METHODS/lbfgs.cpp \
        METHODS/linesearch.cpp \
        METHODS/logger.cpp \
        METHODS/mean.cpp \
        METHODS/optimizer.cpp \
        METHODS/similarity.cpp \
        MLMODELS/dataset.cpp \
        MLMODELS/mlpproblem.cpp \
        MLMODELS/model.cpp \
        MLMODELS/rbfproblem.cpp \
        SAMPLER/kmeanssampler.cpp \
        SAMPLER/maxwellsampler.cpp \
        SAMPLER/neuralsampler.cpp \
        SAMPLER/problemsampler.cpp \
        SAMPLER/rbfsampler.cpp \
        SAMPLER/triangularsampler.cpp \
        SAMPLER/uniformsampler.cpp \
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
    INTERVAL/intervalde.h \
    INTERVAL/intervalproblem.h \
    INTERVAL/intervalpso.h \
    INTERVAL/problem.h \
    METHODS/adam.h \
    METHODS/armijosearch.h \
    METHODS/bfgs.h \
    METHODS/collection.h \
    METHODS/doublebox.h \
    METHODS/editlogger.h \
    METHODS/fibonaccisearch.h \
    METHODS/filelogger.h \
    METHODS/genetic.h \
    METHODS/goldensearch.h \
    METHODS/gradientdescent.h \
    METHODS/ipso.h \
    METHODS/lbfgs.h \
    METHODS/linesearch.h \
    METHODS/logger.h \
    METHODS/mean.h \
    METHODS/optimizer.h \
    METHODS/similarity.h \
    MLMODELS/dataset.h \
    MLMODELS/mlpproblem.h \
    MLMODELS/model.h \
    MLMODELS/rbfproblem.h \
    SAMPLER/kmeanssampler.h \
    SAMPLER/maxwellsampler.h \
    SAMPLER/neuralsampler.h \
    SAMPLER/problemsampler.h \
    SAMPLER/rbfsampler.h \
    SAMPLER/triangularsampler.h \
    SAMPLER/uniformsampler.h

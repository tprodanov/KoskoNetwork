QT       += xml

TARGET = KoskoNetwork
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    Neuron.cpp \
    KoskoNetwork.cpp

HEADERS += \
    Neuron.h \
    KoskoNetwork.h

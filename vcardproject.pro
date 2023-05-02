TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        libs/QRCodeGen/qrcodegen.cpp \
        libs/TinyPNGOut/TinyPngOut.cpp \
        main.cpp \
        qrcode.cpp \
        vcard.cpp

HEADERS += \
    libs/QRCodeGen/qrcodegen.hpp \
    libs/TinyPNGOut/TinyPngOut.hpp \
    qrcode.h \
    vcard.h

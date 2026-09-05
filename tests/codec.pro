QT = core
CONFIG += console c++17
CONFIG -= app_bundle
TEMPLATE = app
TARGET = codec_test
SOURCES += codec_test.cpp ../hfutzip.cpp ../node.cpp
HEADERS += ../hfutzip.h ../node.h ../safe_codec.h
QMAKE_CXXFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
QMAKE_LFLAGS += -fsanitize=address,undefined

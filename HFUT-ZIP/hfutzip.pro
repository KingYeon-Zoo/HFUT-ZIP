QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 项目信息
TARGET = HUFFMAN
VERSION = 2.0.0
QMAKE_TARGET_COMPANY = "HFUT"
QMAKE_TARGET_PRODUCT = "HFUT 哈夫曼压缩工具"
QMAKE_TARGET_DESCRIPTION = "基于哈夫曼编码的文件压缩工具"
QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2024"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    fileedit.cpp \
    hfutzip.cpp \
    main.cpp \
    mainwindow.cpp \
    node.cpp

HEADERS += \
    fileedit.h \
    hfutzip.h \
    mainwindow.h \
    node.h

FORMS += \
    mainwindow.ui

# 其他文件
OTHER_FILES += \
    styles.qss \
    README.md

# 资源文件（如果需要将QSS嵌入到可执行文件中）
# RESOURCES += resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

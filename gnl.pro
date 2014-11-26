TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

HEADERS += gnl_json.h \
           gnl_zip.h \
           gnl_filesystem.h

QMAKE_CXXFLAGS = -std=c++11

include(deployment.pri)
qtcAddDeployment()

OTHER_FILES += \
    README.md


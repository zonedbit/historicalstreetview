QT += xml network svg core gui network script scriptTools

TARGET = historicalstreetview
TEMPLATE = app 
INCLUDEPATH += ../../src/global \
               ../../src/location \
               ../../src/location/maps
SOURCES += main.cpp \
           mainwindow.cpp \
    Poi.cpp \
    InfoPoi.cpp \
    MapWidget.cpp
           
HEADERS += \
           mainwindow.h \
    Poi.h \
    InfoPoi.h \
    MapWidget.h

CONFIG += mobility11
MOBILITY = systeminfo location

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}

FORMS += \
    InfoPoi.ui

RESOURCES += \
    qtresources.qrc

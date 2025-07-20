QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    csv.cpp \
    main_qt.cpp \
    mainwindow.cpp \
    network.cpp \
    scheduled_trip.cpp \
    stoptimestablemodel.cpp

HEADERS += \
    config.h \
    csv.h \
    mainwindow.h \
    network.h \
    scheduled_trip.h \
    stoptimestablemodel.h \
    types.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
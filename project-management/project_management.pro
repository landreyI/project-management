QT       += core gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    QProject.cpp \
    QTask.cpp \
    QUser.cpp \
    main.cpp \
    mainwindow.cpp \
    qdialoglogin.cpp

HEADERS += \
    QProject.h \
    QTask.h \
    QUser.h \
    mainwindow.h \
    qdialoglogin.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    project_management_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

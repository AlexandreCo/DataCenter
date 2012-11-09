# -------------------------------------------------
# Project created by QtCreator 2010-01-26T13:40:44
# -------------------------------------------------
TARGET = viewer
TEMPLATE = app
DESTDIR = .
DEFINES += _TTY_POSIX_
unix:UI_DIR = ui
unix:MOC_DIR = moc
unix:OBJECTS_DIR = obj
SOURCES += main.cpp \
    mainwindow.cpp \
    threadrs.cpp \
    csv.cpp \
    samples.cpp
INCLUDEPATH += /usr/local/qwt-5.2.0/include/ \
    /usr/local/qextserialport-1.1/include/ \
    ./
HEADERS += mainwindow.h \
    threadrs.h \
    csv.h \
    samples.h
LIBS += -L/usr/lib \
    -lqwt \
    -lqextserialport
FORMS += mainwindow.ui
RESOURCES += resource.qrc

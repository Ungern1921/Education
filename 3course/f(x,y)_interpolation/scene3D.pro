HEADERS       = scene3D.h \
    interpolation2.h
SOURCES       = main.cpp \
                scene3D.cpp
QT += widgets
QT += core gui opengl

unix:!macx: LIBS += -L$$PWD/./ -linter2 -Wl,-rpath=.

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

DISTFILES += \
    libinter2.so

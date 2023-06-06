QMAKE_CXXFLAGS += -Werror -Wl,-rpath='$ORIGIN' -L. -linter
HEADERS       = window.h  \
    interpolation.h
SOURCES       = main.cpp \
                window.cpp



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -linter
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -linter
else:unix: LIBS += -L$$PWD/./ -linter

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

QT += widgets

DISTFILES += \
    libinter.so

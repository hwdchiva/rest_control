#=========================================================================================
#  __   __  ___        ___  ______    _____   __   __   ________  ___      ___  _______
# |  | |  | \  \  /\  /  / |   _  \  |   __| |  | |  | |__    __| \  \    /  / /   _   \
# |  |_|  |  \  \/  \/  /  |  | |  | |  |    |  |_|  |    |  |     \  \  /  /  |  | |  |
# |   _   |   \        /   |  | |  | |  |    |   _   |    |  |      \  \/  /   |  |_|  |
# |  | |  |    \  /\  /    |  |_|  | |  |__  |  | |  |  __|  |__     \    /    |   _   |
# |__| |__|     \/  \/     |______/  |_____| |__| |__| |________|     \__/     |__| |__|
#
#=========================================================================================

QT += core gui widgets network gamepad

DESTDIR = bin
TARGET = rest_control
TEMPLATE = app

INCLUDEPATH+= libs/SDL2-2.0.8/include src

SOURCES = \
    src/main.cpp         \
    src/MainWindow.cpp \
    src/HttpRequestWorker.cpp \
    src/joystick/qjoystick.cpp

HEADERS = \
    src/MainWindow.h \
    src/HttpRequestWorker.h \
    src/joystick/qjoystick.h

FORMS   = \
    src/MainWindow.ui

LIBS += -L./ \
        -L../../libs/SDL2-2.0.8/lib/x64 -lSDL2


win32 {
DEFINES += SDL_WIN
DEFINES += QT_NO_DEBUG_OUTPUT
}

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD/../ffmpeg-win32-dev/include \
               $$PWD/../SDL2-2.0.4/include
DEPENDPATH += $$PWD/../ffmpeg-win32-dev/include

LIBS += $$PWD/../ffmpeg-win32-dev/lib/avcodec.lib \
        $$PWD/../ffmpeg-win32-dev/lib/avdevice.lib \
        $$PWD/../ffmpeg-win32-dev/lib/avfilter.lib \
        $$PWD/../ffmpeg-win32-dev/lib/avformat.lib \
        $$PWD/../ffmpeg-win32-dev/lib/avutil.lib \
        $$PWD/../ffmpeg-win32-dev/lib/postproc.lib \
        $$PWD/../ffmpeg-win32-dev/lib/swresample.lib \
        $$PWD/../ffmpeg-win32-dev/lib/swscale.lib

LIBS += -LD:/Code/QTProject/SDL2-2.0.4/lib/x86 -lSDL2 \
        -LD:/Code/QTProject/SDL2-2.0.4/lib/x86 -lSDL2main \
        -LD:/Code/QTProject/SDL2-2.0.4/lib/x86 -lSDL2test

SOURCES += main.cpp \
    showbmp.cpp

HEADERS += \
    showbmp.h


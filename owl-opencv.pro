TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += C:\OpenCV31\build\include $$PWD/include # Was previously 'c:/opencv31/release/install/include'

# Linux only
#LIBS += -LC:\opencv31\build\install\x86\mingw\lib
#LIBS += -LC:\opencv31\build\install\x86\mingw\bin

LIBS += -LC:\opencv31\build\lib
LIBS += \
    -lopencv_core310 \
    -lopencv_highgui310 \
    -lopencv_imgproc310 \
    -lopencv_features2d310 \
    -lopencv_calib3d310 \
    -lopencv_videoio310 \
    -lws2_32

SOURCES += \
    src/main.cpp \
    src/run.cpp \
    src/comm.cpp

HEADERS += \
    include/comm.hpp \
    include/pwm.hpp \
    include/run.hpp \
    include/correl.hpp

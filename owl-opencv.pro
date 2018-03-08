TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += C:\OpenCV31\build\include $$PWD/include ### was c:/opencv31/release/install/include

#LIBS += -LC:\opencv31\build\install\x86\mingw\lib
#LIBS += -LC:\opencv31\build\install\x86\mingw\bin
LIBS += -LC:\opencv31\build\lib
LIBS += -lopencv_core310 \
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

files = $$files($$PWD/src)
win32:files ~= s|\\\\|/|g
for(file, files):!exists($$file/*):contains($$file, ".cpp"):SOURCES += $$file

files = $$files($$PWD/include)
win32:files ~= s|\\\\|/|g
for(file, files):!exists($$file/*):contains($$file, ".hpp"):HEADERS += $$file

#HEADERS += \
#    ../../Sources/Owl-1/owl-comms.h \
#    ../../Sources/Owl-1/owl-cv.h \
#    ../../Sources/Owl-1/owl-pwm.h \
#    ../../Sources/Owl-1/run.hpp \
#    ../../Sources/Owl-1/comm.hpp \
#    ../../Sources/Owl-1/pwm.hpp \
#    ../../Sources/Owl-1/include/comm.hpp \
#    ../../Sources/Owl-1/include/pwm.hpp \
#    ../../Sources/Owl-1/include/run.hpp

#DISTFILES += \
#    .DS_Store \
#    ../../Sources/Owl-1/Owl-demo1.pdf \
#    ../../Sources/Owl-1/Owl-RNDIS.pdf \
#    ../../Sources/Owl-1/Owl-demo1.odt \
#    ../../Sources/Owl-1/Owl-RNDIS.odt \
#    ../../Sources/Owl-1/Owl-L.png \
#    ../../Sources/Owl-1/Owl-R.png \
#    ../../Sources/Owl-1/PFCsocket.py

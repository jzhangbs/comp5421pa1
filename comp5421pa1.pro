#-------------------------------------------------
#
# Project created by QtCreator 2018-02-18T20:53:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = comp5421pa1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    image.cpp \
    imgdisp.cpp \
    fibheap.cpp \
    shortest.cpp

HEADERS += \
        mainwindow.h \
    image.h \
    imgdisp.h \
    fibheap.h \
    shortest.h

FORMS += \
        mainwindow.ui

win32{INCLUDEPATH += \
        D:/code/opencv/build/include \
        D:/code/opencv/build/include/opencv \
        D:/code/opencv/build/include/opencv2}

macx{INCLUDEPATH += \
    /usr/local/Cellar/opencv/3.4.0_1/include\
    /usr/local/Cellar/opencv/3.4.0_1/include/opencv \
    /usr/local/Cellar/opencv/3.4.0_1/include/opencv2
    LIBS+= -L/usr/local/Cellar/opencv/3.4.0_1/lib -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_dpm -lopencv_face -lopencv_photo -lopencv_fuzzy -lopencv_img_hash -lopencv_line_descriptor -lopencv_optflow -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_surface_matching -lopencv_tracking -lopencv_datasets -lopencv_text -lopencv_dnn -lopencv_plot -lopencv_xfeatures2d -lopencv_shape -lopencv_video -lopencv_ml -lopencv_ximgproc -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_flann -lopencv_xobjdetect -lopencv_imgcodecs -lopencv_objdetect -lopencv_xphoto -lopencv_imgproc -lopencv_core
}

win32:CONFIG(release, debug|release): LIBS += D:/code/opencv/build/x64/vc15/lib/opencv_world340.lib
else:win32:CONFIG(debug, debug|release): LIBS += D:/code/opencv/build/x64/vc15/lib/opencv_world340d.lib

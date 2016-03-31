qmake TrackerRT_UI/TrackerRT_UI.pro
/usr/lib/arm-linux-gnueabihf/qt5/bin/uic ./TrackerRT_UI/mainwindow.ui -o TrackerRT_UI/ui_mainwindow.h
/usr/lib/arm-linux-gnueabihf/qt5/bin/uic ./TrackerRT_UI/duoconfigwidget.ui -o TrackerRT_UI/ui_duoconfigwidget.h
/usr/lib/arm-linux-gnueabihf/qt5/bin/uic ./TrackerRT_UI/UICalibrationDisplay.ui -o TrackerRT_UI/ui_UICalibrationDisplay.h
/usr/lib/arm-linux-gnueabihf/qt5/bin/uic ./TrackerRT_UI/UIStereoDisplay.ui -o TrackerRT_UI/ui_UIStereoDisplay.h
#/usr/lib/arm-linux-gnueabihf/qt5/bin/uic ./TrackerRT_UI/UIOpenGLFrameRenderer.ui -o TrackerRT_UI/ui_UIOpenGLFrameRenderer.h
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI/ -o main.o ./TrackerRT_UI/main.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI/ -o mainwindow.o ./TrackerRT_UI/mainwindow.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o circularindicator.o ./TrackerRT_UI/circularindicator.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o duoconfigwidget.o ./TrackerRT_UI/duoconfigwidget.cpp
#g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/opt/Qt/5.5/gcc/include/QtOpenGL -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o UIOpenGLFrameRenderer.o ./TrackerRT_UI/UIOpenGLFrameRenderer.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o UICalibrationDisplay.o ./TrackerRT_UI/UICalibrationDisplay.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o UIController.o ./TrackerRT_UI/UIController.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o UIModel.o ./TrackerRT_UI/UIModel.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I/usr/include/qt5 -I/usr/include/qt5/QtOpenGL -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o GLWidget.o ./TrackerRT_UI/GLWidget.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtOpenGL -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I. -I. -o UIStereoDisplay.o ./TrackerRT_UI/UIStereoDisplay.cpp
/usr/lib/arm-linux-gnueabihf/qt5/bin/moc -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -I/usr/include/c++/4.8 -I/usr/include/arm-linux-gnueabihf/c++/4.8 -I/usr/include/c++/4.8/backward -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include -I/usr/local/include -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include-fixed -I/usr/include/arm-linux-gnueabihf -I/usr/include ./TrackerRT_UI/mainwindow.h -o moc_mainwindow.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o moc_mainwindow.o moc_mainwindow.cpp
/usr/lib/arm-linux-gnueabihf/qt5/bin/moc -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -I/usr/include/c++/4.8 -I/usr/include/arm-linux-gnueabihf/c++/4.8 -I/usr/include/c++/4.8/backward -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include -I/usr/local/include -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include-fixed -I/usr/include/arm-linux-gnueabihf -I/usr/include ./TrackerRT_UI/circularindicator.h -o moc_circularindicator.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o moc_circularindicator.o moc_circularindicator.cpp
/usr/lib/arm-linux-gnueabihf/qt5/bin/moc -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -I/usr/include/c++/4.8 -I/usr/include/arm-linux-gnueabihf/c++/4.8 -I/usr/include/c++/4.8/backward -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include -I/usr/local/include -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include-fixed -I/usr/include/arm-linux-gnueabihf -I/usr/include ./TrackerRT_UI/duoconfigwidget.h -o moc_duoconfigwidget.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o moc_duoconfigwidget.o moc_duoconfigwidget.cpp
/usr/lib/arm-linux-gnueabihf/qt5/bin/moc -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -I/usr/include/c++/4.8 -I/usr/include/arm-linux-gnueabihf/c++/4.8 -I/usr/include/c++/4.8/backward -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include -I/usr/local/include -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include-fixed -I/usr/include/arm-linux-gnueabihf -I/usr/include ./TrackerRT_UI/UICalibrationDisplay.h -o moc_UICalibrationDisplay.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o moc_UICalibrationDisplay.o moc_UICalibrationDisplay.cpp
#/usr/lib/arm-linux-gnueabihf/qt5/bin/moc -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -I/usr/include/c++/4.8 -I/usr/include/arm-linux-gnueabihf/c++/4.8 -I/usr/include/c++/4.8/backward -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include -I/usr/local/include -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include-fixed -I/usr/include/arm-linux-gnueabihf -I/usr/include -I/opt/Qt/5.5/gcc/include/QtOpenGL ./TrackerRT_UI/UIOpenGLFrameRenderer.h -o moc_UIOpenGLFrameRenderer.cpp
#g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I/usr/include/GL -I./TrackerRT_UI -o moc_UIOpenGLFrameRenderer.o moc_UIOpenGLFrameRenderer.cpp
/usr/lib/arm-linux-gnueabihf/qt5/bin/moc -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtOpenGL -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I. -I. -I/usr/include/c++/4.8 -I/usr/include/arm-linux-gnueabihf/c++/4.8 -I/usr/include/c++/4.8/backward -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include -I/usr/local/include -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include-fixed -I/usr/include/arm-linux-gnueabihf -I/usr/include ./TrackerRT_UI/GLWidget.h -o ./TrackerRT_UI/moc_GLWidget.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I. -I/usr/include/qt5 -I/usr/include/qt5/QtOpenGL -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o moc_GLWidget.o moc_GLWidget.cpp
/usr/lib/arm-linux-gnueabihf/qt5/bin/moc -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I./TrackerRT_UI -I/usr/include/qt5 -I/usr/include/qt5/QtOpenGL -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I/usr/include/c++/4.8 -I/usr/include/arm-linux-gnueabihf/c++/4.8 -I/usr/include/c++/4.8/backward -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include -I/usr/local/include -I/usr/lib/gcc/arm-linux-gnueabihf/4.8/include-fixed -I/usr/include/arm-linux-gnueabihf -I/usr/include ./TrackerRT_UI/UIStereoDisplay.h -o moc_UIStereoDisplay.cpp
g++ -c -pipe -O2 -Wall -W -D_REENTRANT -fPIE -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/lib/arm-linux-gnueabihf/qt5/mkspecs/linux-g++ -I. -I/usr/include/qt5 -I/usr/include/qt5/QtOpenGL -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I./TrackerRT_UI -o moc_UIStereoDisplay.o moc_UIStereoDisplay.cpp
g++ -std=c++11 -Wno-unused-result -Wno-write-strings main.o mainwindow.o circularindicator.o duoconfigwidget.o GLWidget.o UIStereoDisplay.o UICalibrationDisplay.o UIController.o UIModel.o moc_mainwindow.o moc_circularindicator.o moc_duoconfigwidget.o moc_UICalibrationDisplay.o moc_GLWidget.o moc_UIStereoDisplay.o Application/*.cpp AugmentedReality/*.cpp Calibration/*.cpp StereoVision/*.cpp Common/*.cpp Configuration/*.cpp DataAssociation/*.cpp FastMemory/*.cpp NeuralNetwork/*.cpp OpticalLayer/*.cpp RealTime/*.cpp SensorAbstractionLayer/*.cpp SpinlockQueue/*.cpp InterpolationEngine/*.cpp StaticModel/*.cpp TrajectoryTracking/*.cpp VelocityFilter/*.cpp VirtualCameraDevice/*.cpp DynamicModel3D/DynamicModel3D.cpp DynamicModel3D/DynamicModel3D_KalmanFilter.cpp -O3 -o bin/Application -I/usr/local/zed/include -I/home/ubuntu/opencv-2.4.10/include /home/ubuntu/opencv-2.4.10/lib/libopencv_calib3d.so /home/ubuntu/opencv-2.4.10/lib/libopencv_contrib.so /home/ubuntu/opencv-2.4.10/lib/libopencv_core.so /home/ubuntu/opencv-2.4.10/lib/libopencv_features2d.so /home/ubuntu/opencv-2.4.10/lib/libopencv_flann.so /home/ubuntu/opencv-2.4.10/lib/libopencv_gpu.so /home/ubuntu/opencv-2.4.10/lib/libopencv_highgui.so /home/ubuntu/opencv-2.4.10/lib/libopencv_imgproc.so /home/ubuntu/opencv-2.4.10/lib/libopencv_legacy.so /home/ubuntu/opencv-2.4.10/lib/libopencv_ml.so /home/ubuntu/opencv-2.4.10/lib/libopencv_objdetect.so /home/ubuntu/opencv-2.4.10/lib/libopencv_photo.so /home/ubuntu/opencv-2.4.10/lib/libopencv_stitching.so /home/ubuntu/opencv-2.4.10/lib/libopencv_superres.so /home/ubuntu/opencv-2.4.10/lib/libopencv_ts.a /home/ubuntu/opencv-2.4.10/lib/libopencv_video.so /home/ubuntu/opencv-2.4.10/lib/libopencv_videostab.so -lgpu -lgsl -lcufft -lnpps -lnppi -lnppc -lcudart -lrt -lQt5Widgets -L/usr/local/zed/lib -L/usr/lib/arm-linux-gnueabihf -lsl_calibration -lsl_depthcore -lsl_zed -lQt5Gui -lQt5OpenGL -lQt5Core -lGLESv2 -lpthread -lgsl -lgslcblas -lm -ldl -I/usr/local/cuda/include -I/home/ubuntu/CL-DUO3D-ARM-1.0.35.226/DUOSDK/SDK/include/ -L/usr/local/cuda/lib -L/home/ubuntu/Release/CUDA/ -L/home/ubuntu/CL-DUO3D-ARM-1.0.35.226/DUOSDK/SDK/linux/arm/ -lDUO -lDense3D -lpthread -lrt -lm -lglut -lGL -lGLU -lGLESv2 -Wl,-rpath,/usr/local/cuda/lib:/usr/lib/arm-linux-gnueabihf/tegra:/home/ubuntu/CL-DUO3D-ARM-1.0.35.226/DUOSDK/SDK/linux/arm/:/home/ubuntu/opencv-2.4.10/lib

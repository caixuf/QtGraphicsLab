QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 设置头文件搜索路径
INCLUDEPATH += include \
               src

SOURCES += \
    src/main.cpp \
    src/mygraphicbackgrounditem.cpp\
    src/mygraphicrectitem.cpp

HEADERS += \
    include/drawarrow.hpp \
    src/globals.h \
    include/graphicsmanager.hpp \
    include/resourcemanager.h \
    src/mygraphicbackgrounditem.h\
    src/mygraphicrectitem.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/image.qrc

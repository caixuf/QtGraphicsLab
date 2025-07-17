#ifndef QTGRAPHICSLAB_MAIN
#define QTGRAPHICSLAB_MAIN
// QtGraphicsLab - Interactive Graphics Framework
// 实现固定场景和视图的大小，使得场景正好和视图一样大
#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 创建主窗口并显示
    MainWidget mainWindow;
    mainWindow.show();
    
    return a.exec();
}
#endif

//首先看图元，第一个是他的boundingRect，这个矩形返回的是图元在场景中的区域


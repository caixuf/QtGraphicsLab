#ifndef QTGRAPHICSLAB_MAIN
#define QTGRAPHICSLAB_MAIN
// QtGraphicsLab - Interactive Graphics Framework
// 实现固定场景和视图的大小，使得场景正好和视图一样大
#include "graphicsmanager.hpp"
#include <memory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 修复内存泄漏：使用智能指针自动管理内存
    auto manager = std::make_unique<GraphicsManager>();
    manager->getView()->show();
    
    return a.exec();
    // manager会在作用域结束时自动释放
}
#endif

//首先看图元，第一个是他的boundingRect，这个矩形返回的是图元在场景中的区域


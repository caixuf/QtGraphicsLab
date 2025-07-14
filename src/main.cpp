#ifndef SCENE_DEMO_01
#define SCENE_DEMO_01
//1.固定场景和视图的大小，使得场景正好和视图一样大，实现方式有直接固定大小以及通过重设大小的事件将视图大小和场景大小绑定，同时要考虑到视图最大不能超过显示器大小
#include "graphicsmanager.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphicsManager * manager = new GraphicsManager();
    manager->getView()->show();
    return a.exec();
}
#endif

//首先看图元，第一个是他的boundingRect，这个矩形返回的是图元在场景中的区域


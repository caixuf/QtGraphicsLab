#include "widget.h"
#include <QResizeEvent>
#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    // 创建 GraphicsManager
    manager = new GraphicsManager(this);
    
    // 创建布局
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // 将 GraphicsView 添加到布局中
    layout->addWidget(manager->getView());
    
    // 配置 GraphicsView 的缩放和显示策略
    QGraphicsView* view = manager->getView();
    view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    view->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // 设置窗口标题
    setWindowTitle("Qt Graphics Lab - Magnifier Demo");
    
    // 设置最小尺寸，但允许调整大小
    setMinimumSize(manager->getView()->size());
    resize(manager->getView()->size());
    
    // 初始化时设置合适的视图缩放，让场景填充整个视图
    QTimer::singleShot(0, this, [this]() {
        if (manager && manager->getView() && manager->getScene()) {
            QGraphicsView* view = manager->getView();
            QGraphicsScene* scene = manager->getScene();
            view->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
        }
    });
}

Widget::~Widget()
{
    // manager 会在父对象销毁时自动销毁
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    // 当窗口大小变化时，让场景内容填充整个窗口
    if (manager && manager->getView() && manager->getScene()) {
        QGraphicsView* view = manager->getView();
        QGraphicsScene* scene = manager->getScene();
        
        // 使用 fitInView 让场景完全填充视图
        view->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
        
        // 或者如果你想保持宽高比但填充尽可能多的空间：
        // view->fitInView(scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
    }
}

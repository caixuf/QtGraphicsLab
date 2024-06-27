#include "mygraphicbackgrounditem.h"

MyGraphicBackgroundItem::MyGraphicBackgroundItem(QGraphicsItem *parent):QGraphicsItem(parent),
    image(QImage(":/image/a1.png"))
    ,m_size(150, 100)
{
}

MyGraphicBackgroundItem::MyGraphicBackgroundItem(QString imagePath, QSizeF size):image(imagePath), m_size(size)
{

}

// 设置图元的大小
void MyGraphicBackgroundItem::setSize(const QSizeF &size)
{
    prepareGeometryChange(); // 通知Qt将要改变图元的几何形状
    m_size = size;
    this->update(); // 通知Qt需要重新绘制图元
}

// 重写 boundingRect 来返回图元的边界矩形
QRectF MyGraphicBackgroundItem::boundingRect() const
{
    return QRectF(0, 0, m_size.width(), m_size.height());
}

// 重写 shape 来返回图元的精确形状（如果需要更精确的碰撞检测）
QPainterPath MyGraphicBackgroundItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

// 重写 paint 来绘制图元
void MyGraphicBackgroundItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawImage(boundingRect(), image);
    painter->drawRect(boundingRect());
    painter->fillRect(boundingRect(), QColor(0, 0, 0, 150));
}

// 重写 mousePressEvent 来处理鼠标点击事件
void MyGraphicBackgroundItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
    }
}

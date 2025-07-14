#ifndef MYMOVEWINDOWITEM_H
#define MYMOVEWINDOWITEM_H
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QUrl>
#include <QTimer>
#include <QImage>
#include <QSizeF>
#include <QString>
#include "math.h"
#include "globals.h"

class MyMoveWindowItem : public QGraphicsItem
{
public:
    MyMoveWindowItem(QSizeF size, QString imagePath);
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:
    QSizeF m_size;
    bool m_isDrag;
    QPointF m_startPos;
    QPointF m_pos;
    QPointF m_pressedPos;
    QPixmap pixmap;
};


#endif // MYMOVEWINDOWITEM_H

#ifndef MYGRAPHICBACKGROUNDITEM_H
#define MYGRAPHICBACKGROUNDITEM_H

#include "math.h"

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QUrl>
#include <QTimer>
#include <QImage>
#include <QString>

class MyGraphicBackgroundItem : public QGraphicsItem
{
public:
    MyGraphicBackgroundItem(QGraphicsItem *parent = nullptr);
    MyGraphicBackgroundItem(QString imagePath, QSizeF size);
    void setSize(const QSizeF &size);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QImage image;
    QSizeF m_size; // 图元的大小
};

#endif // MYGRAPHICBACKGROUNDITEM_H

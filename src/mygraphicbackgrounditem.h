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
#include "resourcemanager.h"

class MyGraphicBackgroundItem : public QGraphicsItem
{
public:
    MyGraphicBackgroundItem(QGraphicsItem *parent = nullptr);
    MyGraphicBackgroundItem(QString imagePath, QSizeF size);
    void setSize(const QSizeF &size);
    void setMaskOpacity(qreal opacity);
    qreal getMaskOpacity() const { return m_maskOpacity; }
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QImage image;
    QSizeF m_size;      // 图元的大小
    qreal m_maskOpacity; // 遮罩透明度 (0.0 - 1.0)
};

#endif // MYGRAPHICBACKGROUNDITEM_H

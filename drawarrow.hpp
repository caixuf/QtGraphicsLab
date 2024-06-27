#ifndef DRAWARROW_H
#define DRAWARROW_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QLineF>
#include "globals.h"

class ArrowItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    ArrowItem(QGraphicsItem *parent = nullptr) : QGraphicsItem(parent), startPoint(), endPoint() {}

    QRectF boundingRect() const override
    {
        // 假设 sceneRect 是场景的大小
        return QRectF(0, 0, WIDTH, HEIGHT);
    }

    QPainterPath shape() const override
    {
        // 返回一个空的路径，使鼠标事件穿透
        return QPainterPath();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        Q_UNUSED(option)
        Q_UNUSED(widget)

        if (!isValid(startPoint) || !isValid(endPoint))
        {
            return; // 如果没有设置起点和终点，则不绘制
        }

        painter->setRenderHint(QPainter::Antialiasing); // 设置抗锯齿，使绘制更平滑
        QPen pen(QColor(0, 0, 0, 0));
        pen.setWidth(1);
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::green));
        double startX = startPoint.x();
        double startY = startPoint.y();
        double endX = endPoint.x();
        double endY = endPoint.y();
        double fromX = startX;
        double fromY = startY;
        double toX = endX;
        double toY = endY;
        double theta = 30, theta2 = 15;
        double headlen = sqrt(pow(toX - fromX, 2) + pow(toY - fromY, 2)) / 25.0,
            headlen2 = headlen * cos(theta * M_PI / 180) / cos(theta2 * M_PI / 180);

        double angle = atan2(toY - fromY, toX - fromX);
        double angle1 = angle + (theta * M_PI / 180);
        double angle2 = angle - (theta * M_PI / 180);
        double angle3 = angle + (theta2 * M_PI / 180);
        double angle4 = angle - (theta2 * M_PI / 180);

        double topX = headlen * cos(angle1);
        double topY = headlen * sin(angle1);
        double botX = headlen * cos(angle2);
        double botY = headlen * sin(angle2);

        double topX2 = headlen2 * cos(angle3);
        double topY2 = headlen2 * sin(angle3);
        double botX2 = headlen2 * cos(angle4);
        double botY2 = headlen2 * sin(angle4);

        // 将坐标转换为相对于起点的位置
        double arrowTopX = toX - topX;
        double arrowTopY = toY - topY;
        double arrowBotX = toX - botX;
        double arrowBotY = toY - botY;

        double arrowTopX2 = toX - topX2;
        double arrowTopY2 = toY - topY2;
        double arrowBotX2 = toX - botX2;
        double arrowBotY2 = toY - botY2;
        // 绘制线段和箭头主体
        painter->drawPolygon(QPolygonF()
                             << QPointF(fromX, fromY)
                             << QPointF(arrowTopX2, arrowTopY2)
                             << QPointF(arrowTopX, arrowTopY)
                             << QPointF(toX, toY)
                             << QPointF(arrowBotX, arrowBotY)
                             << QPointF(arrowBotX2, arrowBotY2)
                             << QPointF(fromX, fromY)
                             );

        //绘制遮罩区域
        QPen pen1(QColor(0, 0, 0, 0));
        pen1.setWidth(1);
        painter->setPen(pen1);
        painter->setBrush(QBrush(QColor(0, 0, 0, 150)));
        QPointF intersectionPoint1 = getLineInterRectPoint(fromX, fromY, arrowTopX2, arrowTopY2, m_oldRectf);
        QPointF intersectionPoint2 = getLineInterRectPoint(fromX, fromY, arrowBotX2, arrowBotY2, m_oldRectf);
        QPolygonF markArrowArea = QPolygonF()
                                  << QPointF(fromX, fromY)
                                  << intersectionPoint1
                                  << intersectionPoint2
                                  << QPointF(fromX, fromY);
        QPointF inPolyPoint;
        if(!IsRectPointInPolygon(markArrowArea, m_oldRectf, &inPolyPoint))
        {
            painter->drawPolygon(markArrowArea);
        }
        else
        {
            painter->drawPolygon(QPolygonF()
                                 << QPointF(fromX, fromY)
                                 << intersectionPoint1
                                 << inPolyPoint
                                 << intersectionPoint2
                                 << QPointF(fromX, fromY));
        }
        // 绘制目标点的十字标记
        QPen pen2(Qt::yellow);
        pen.setWidth(2);
        painter->setPen(pen2);
        painter->drawLine(QLineF(toX - 10, toY, toX + 10, toY));
        painter->drawLine(QLineF(toX, toY - 10, toX, toY + 10));
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        startPoint = event->pos();
        endPoint = startPoint; // 初始时终点和起点相同
        update(); // 更新绘制
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override
    {
        if (isValid(startPoint)) {
            endPoint = event->pos();
            update(); // 更新绘制
        }
    }

    void setStartPoint(const QPointF &point)
    {
        startPoint = point;
        update(); // 更新绘制
    }

    void setEndPoint(const QPointF &point)
    {
        endPoint = point;
        update(); // 更新绘制
    }
    bool isValid(const QPointF &point)
    {
        return std::isfinite(point.x()) && std::isfinite(point.y());
    }
    QPointF getLineInterRectPoint(qreal x1, qreal y1, qreal x2, qreal y2, QRectF rect) {
        QLineF line(x1, y1, x2, y2);
        QPointF intersection;

        // 检查与矩形左边的交点
        if (lineSegmentsIntersect(line, QLineF(rect.left(), rect.top(), rect.left(), rect.bottom()),
                                  &intersection))
        {
            return intersection;
        }

        // 检查与矩形右边的交点
        if (lineSegmentsIntersect(line, QLineF(rect.right(), rect.top(), rect.right(), rect.bottom()),
                                  &intersection))
        {
            return intersection;
        }

        // 检查与矩形顶部的交点
        if (lineSegmentsIntersect(line, QLineF(rect.left(), rect.top(), rect.right(), rect.top()),
                                  &intersection))
        {
            return intersection;
        }

        // 检查与矩形底部的交点
        if (lineSegmentsIntersect(line, QLineF(rect.left(), rect.bottom(), rect.right(), rect.bottom()),
                                  &intersection))
        {
            return intersection;
        }

        // 如果没有交点，返回无效的QPointF
        qDebug()<<"无交点";
        return QPointF();
    }

    bool lineSegmentsIntersect(const QLineF &line1, const QLineF &line2, QPointF *intersectionPoint)
    {
        QLineF::IntersectType type = line1.intersects(line2, intersectionPoint);
        if (type != QLineF::BoundedIntersection)
            return false;
        return true;

    }
    bool IsRectPointInPolygon(const QPolygonF polygon, const QRectF m_oldRect, QPointF *intersectionPoint)
    {
        QPointF ltp = mapToScene(QPointF(m_oldRect.left(), m_oldRect.top()));
        QPointF rtp = mapToScene(QPointF(m_oldRect.right(), m_oldRect.top()));
        QPointF lbp = mapToScene(QPointF(m_oldRect.left(), m_oldRect.bottom()));
        QPointF rbp = mapToScene(QPointF(m_oldRect.right(), m_oldRect.bottom()));
        if(polygon.containsPoint(ltp, Qt::WindingFill))
        {
            *intersectionPoint = ltp;
            return true;
        }
        if(polygon.containsPoint(rtp, Qt::WindingFill))
        {
            *intersectionPoint = rtp;
            return true;
        }
        if(polygon.containsPoint(lbp, Qt::WindingFill))
        {
            *intersectionPoint = lbp;
            return true;
        }
        if(polygon.containsPoint(rbp, Qt::WindingFill))
        {
            *intersectionPoint = rbp;
            return true;
        }
        return false;
    }
    void setOldRectf(QRectF rect)
    {
        m_oldRectf = rect;
    }

private:
    QPointF startPoint, endPoint;
    QRectF m_oldRectf;
};
#endif // DRAWARROW_H

#ifndef DRAWARROW_H
#define DRAWARROW_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QLineF>
#include <cmath>
#include <QDebug>
#include "globals.h"

class ArrowItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    explicit ArrowItem(QGraphicsItem *parent = nullptr);
    
    // QGraphicsItem interface
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    // 鼠标事件
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    
    // 公共接口
    void setStartPoint(const QPointF &point);
    void setEndPoint(const QPointF &point);
    void setOldRectf(const QRectF &rect);

signals:
    void arrowChanged();

private:
    // 常量定义
    static constexpr double ARROW_ANGLE = 30.0;           // 箭头角度（度）
    static constexpr double ARROW_INNER_ANGLE = 15.0;     // 箭头内角度（度）
    static constexpr double CROSS_SIZE = 10.0;            // 十字标记大小
    static constexpr double BOUNDING_MARGIN = 20.0;       // 边界框边距
    static constexpr int PEN_WIDTH = 2;                   // 画笔宽度
    
    // 成员变量
    QPointF m_startPoint;
    QPointF m_endPoint;
    QRectF m_oldRectf;
    
    // 私有方法
    bool isValidPoint(const QPointF &point) const;
    QPolygonF createArrowPolygon() const;
    QPolygonF createMaskPolygon() const;
    void drawArrow(QPainter *painter) const;
    void drawMask(QPainter *painter) const;
    void drawTargetCross(QPainter *painter) const;
    
    // 几何计算方法
    QPointF getLineRectIntersection(const QLineF &line, const QRectF &rect) const;
    bool lineSegmentsIntersect(const QLineF &line1, const QLineF &line2, QPointF *intersectionPoint) const;
    bool isRectPointInPolygon(const QPolygonF &polygon, const QRectF &rect, QPointF *intersectionPoint) const;
};

// 实现部分
inline ArrowItem::ArrowItem(QGraphicsItem *parent) 
    : QGraphicsItem(parent)
    , m_startPoint()
    , m_endPoint()
    , m_oldRectf()
{
    setFlag(ItemIsMovable, false);
    setFlag(ItemIsSelectable, false);
}

inline QRectF ArrowItem::boundingRect() const
{
    if (!isValidPoint(m_startPoint) || !isValidPoint(m_endPoint)) {
        return QRectF();
    }
    
    QRectF rect = QRectF(m_startPoint, m_endPoint).normalized();
    return rect.adjusted(-BOUNDING_MARGIN, -BOUNDING_MARGIN, 
                        BOUNDING_MARGIN, BOUNDING_MARGIN);
}

inline QPainterPath ArrowItem::shape() const
{
    QPainterPath path;
    if (isValidPoint(m_startPoint) && isValidPoint(m_endPoint)) {
        QPolygonF arrow = createArrowPolygon();
        path.addPolygon(arrow);
    }
    return path;
}

inline void ArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (!isValidPoint(m_startPoint) || !isValidPoint(m_endPoint)) {
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);
    
    drawArrow(painter);
    drawMask(painter);
    drawTargetCross(painter);
}

inline void ArrowItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        setStartPoint(event->pos());
        setEndPoint(event->pos());
    }
    QGraphicsItem::mousePressEvent(event);
}

inline void ArrowItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (isValidPoint(m_startPoint)) {
        setEndPoint(event->pos());
    }
    QGraphicsItem::mouseMoveEvent(event);
}

inline void ArrowItem::setStartPoint(const QPointF &point)
{
    if (!isValidPoint(point) || m_startPoint == point) {
        return;
    }
    
    prepareGeometryChange();
    m_startPoint = point;
    update();
    emit arrowChanged();
}

inline void ArrowItem::setEndPoint(const QPointF &point)
{
    if (!isValidPoint(point) || m_endPoint == point) {
        return;
    }
    
    prepareGeometryChange();
    m_endPoint = point;
    update();
    emit arrowChanged();
}

inline void ArrowItem::setOldRectf(const QRectF &rect)
{
    if (m_oldRectf == rect) {
        return;
    }
    
    m_oldRectf = rect;
    update();
}

inline bool ArrowItem::isValidPoint(const QPointF &point) const
{
    return std::isfinite(point.x()) && std::isfinite(point.y());
}

inline QPolygonF ArrowItem::createArrowPolygon() const
{
    const double dx = m_endPoint.x() - m_startPoint.x();
    const double dy = m_endPoint.y() - m_startPoint.y();
    const double distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance < 1.0) {
        return QPolygonF();
    }
    
    const double headLength = distance / 25.0;
    const double headLength2 = headLength * std::cos(ARROW_ANGLE * M_PI / 180.0) / 
                              std::cos(ARROW_INNER_ANGLE * M_PI / 180.0);
    
    const double angle = std::atan2(dy, dx);
    const double angle1 = angle + (ARROW_ANGLE * M_PI / 180.0);
    const double angle2 = angle - (ARROW_ANGLE * M_PI / 180.0);
    const double angle3 = angle + (ARROW_INNER_ANGLE * M_PI / 180.0);
    const double angle4 = angle - (ARROW_INNER_ANGLE * M_PI / 180.0);
    
    const QPointF arrowTop = m_endPoint - QPointF(headLength * std::cos(angle1), 
                                                  headLength * std::sin(angle1));
    const QPointF arrowBot = m_endPoint - QPointF(headLength * std::cos(angle2), 
                                                  headLength * std::sin(angle2));
    const QPointF arrowTop2 = m_endPoint - QPointF(headLength2 * std::cos(angle3), 
                                                   headLength2 * std::sin(angle3));
    const QPointF arrowBot2 = m_endPoint - QPointF(headLength2 * std::cos(angle4), 
                                                   headLength2 * std::sin(angle4));
    
    return QPolygonF() << m_startPoint << arrowTop2 << arrowTop 
                      << m_endPoint << arrowBot << arrowBot2;
}

inline void ArrowItem::drawArrow(QPainter *painter) const
{
    QPen pen(QColor(0, 0, 0, 0));
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(QBrush(Qt::green));
    
    QPolygonF arrow = createArrowPolygon();
    if (!arrow.isEmpty()) {
        painter->drawPolygon(arrow);
    }
}

inline void ArrowItem::drawMask(QPainter *painter) const
{
    if (m_oldRectf.isEmpty()) {
        return;
    }
    
    QPen pen(QColor(0, 0, 0, 0));
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(0, 0, 0, 150)));
    
    QPolygonF mask = createMaskPolygon();
    if (!mask.isEmpty()) {
        painter->drawPolygon(mask);
    }
}

inline QPolygonF ArrowItem::createMaskPolygon() const
{
    if (m_oldRectf.isEmpty()) {
        return QPolygonF();
    }
    
    QPolygonF arrow = createArrowPolygon();
    if (arrow.size() < 6) {
        return QPolygonF();
    }
    
    const QPointF &arrowTop2 = arrow[1];
    const QPointF &arrowBot2 = arrow[5];
    
    QLineF line1(m_startPoint, arrowTop2);
    QLineF line2(m_startPoint, arrowBot2);
    
    QPointF intersection1 = getLineRectIntersection(line1, m_oldRectf);
    QPointF intersection2 = getLineRectIntersection(line2, m_oldRectf);
    
    if (intersection1.isNull() || intersection2.isNull()) {
        return QPolygonF();
    }
    
    QPolygonF maskArea;
    maskArea << m_startPoint << intersection1 << intersection2;
    
    QPointF inPolyPoint;
    if (isRectPointInPolygon(maskArea, m_oldRectf, &inPolyPoint)) {
        maskArea.insert(2, inPolyPoint);
    }
    
    return maskArea;
}

inline void ArrowItem::drawTargetCross(QPainter *painter) const
{
    QPen pen(Qt::yellow);
    pen.setWidth(PEN_WIDTH);
    painter->setPen(pen);
    
    const double x = m_endPoint.x();
    const double y = m_endPoint.y();
    
    painter->drawLine(QLineF(x - CROSS_SIZE, y, x + CROSS_SIZE, y));
    painter->drawLine(QLineF(x, y - CROSS_SIZE, x, y + CROSS_SIZE));
}

inline QPointF ArrowItem::getLineRectIntersection(const QLineF &line, const QRectF &rect) const
{
    QPointF intersection;
    
    // 检查与矩形四边的交点
    const QLineF edges[4] = {
        QLineF(rect.topLeft(), rect.topRight()),      // 顶边
        QLineF(rect.topRight(), rect.bottomRight()),  // 右边
        QLineF(rect.bottomRight(), rect.bottomLeft()), // 底边
        QLineF(rect.bottomLeft(), rect.topLeft())     // 左边
    };
    
    for (const auto &edge : edges) {
        if (lineSegmentsIntersect(line, edge, &intersection)) {
            return intersection;
        }
    }
    
    return QPointF(); // 无交点
}

inline bool ArrowItem::lineSegmentsIntersect(const QLineF &line1, const QLineF &line2, 
                                           QPointF *intersectionPoint) const
{
    return line1.intersects(line2, intersectionPoint) == QLineF::BoundedIntersection;
}

inline bool ArrowItem::isRectPointInPolygon(const QPolygonF &polygon, const QRectF &rect, 
                                          QPointF *intersectionPoint) const
{
    const QPointF corners[4] = {
        rect.topLeft(),
        rect.topRight(),
        rect.bottomLeft(),
        rect.bottomRight()
    };
    
    for (const auto &corner : corners) {
        if (polygon.containsPoint(corner, Qt::WindingFill)) {
            if (intersectionPoint) {
                *intersectionPoint = corner;
            }
            return true;
        }
    }
    
    return false;
}

#endif // DRAWARROW_H
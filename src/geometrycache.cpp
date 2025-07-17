#include "geometrycache.h"
#include <QtMath>
#include <QDebug>

GeometryCache& GeometryCache::instance()
{
    static GeometryCache instance;
    return instance;
}

QPolygonF GeometryCache::getCachedRotatedPolygon(const QRectF& rect, qreal angle, const QPointF& center)
{
    m_stats.totalRequests++;
    
    GeometryKey key{rect, angle, center};
    
    // 检查缓存
    auto it = m_polygonCache.find(key);
    if (it != m_polygonCache.end()) {
        m_stats.cacheHits++;
        // 更新访问顺序
        m_accessOrder[key] = ++m_accessCounter;
        return it->second;
    }
    
    // 缓存未命中，计算新的多边形
    m_stats.cacheMisses++;
    QPolygonF result = calculateRotatedPolygon(rect, angle, center);
    
    // 检查是否需要清理缓存
    if (m_polygonCache.size() >= m_maxCacheSize) {
        evictOldest();
    }
    
    // 添加到缓存
    m_polygonCache[key] = result;
    m_accessOrder[key] = ++m_accessCounter;
    m_stats.currentCacheSize = m_polygonCache.size();
    
    return result;
}

QPolygonF GeometryCache::calculateRotatedPolygon(const QRectF& rect, qreal angle, const QPointF& center)
{
    // 如果角度为0，直接返回矩形的四个角点
    if (qAbs(angle) < 0.001) {
        return QPolygonF(rect);
    }
    
    // 创建矩形的四个角点
    QPolygonF polygon;
    polygon << rect.topLeft() << rect.topRight() << rect.bottomRight() << rect.bottomLeft();
    
    // 应用旋转变换
    QPolygonF rotatedPolygon;
    qreal radians = qDegreesToRadians(angle);
    qreal cosTheta = qCos(radians);
    qreal sinTheta = qSin(radians);
    
    for (const QPointF& point : polygon) {
        // 平移到原点
        qreal x = point.x() - center.x();
        qreal y = point.y() - center.y();
        
        // 旋转
        qreal rotatedX = x * cosTheta - y * sinTheta;
        qreal rotatedY = x * sinTheta + y * cosTheta;
        
        // 平移回原位置
        rotatedPolygon << QPointF(rotatedX + center.x(), rotatedY + center.y());
    }
    
    return rotatedPolygon;
}

void GeometryCache::evictOldest()
{
    if (m_polygonCache.empty()) return;
    
    // 找到最老的访问记录
    auto oldestIt = m_accessOrder.begin();
    int oldestAccess = oldestIt->second;
    
    for (auto it = m_accessOrder.begin(); it != m_accessOrder.end(); ++it) {
        if (it->second < oldestAccess) {
            oldestAccess = it->second;
            oldestIt = it;
        }
    }
    
    // 从两个映射中删除最老的项
    GeometryKey keyToRemove = oldestIt->first;
    m_polygonCache.erase(keyToRemove);
    m_accessOrder.erase(keyToRemove);
    
    qDebug() << "GeometryCache: Evicted oldest item. Current cache size:" << m_polygonCache.size();
}

void GeometryCache::clearCache()
{
    m_polygonCache.clear();
    m_accessOrder.clear();
    m_accessCounter = 0;
    m_stats.currentCacheSize = 0;
    qDebug() << "GeometryCache: Cache cleared";
}

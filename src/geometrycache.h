#ifndef GEOMETRYCACHE_H
#define GEOMETRYCACHE_H

#include <QRectF>
#include <QPolygonF>
#include <QPointF>
#include <QString>
#include <unordered_map>
#include <functional>

/**
 * @brief 几何计算缓存键
 */
struct GeometryKey {
    QRectF rect;
    qreal angle;
    QPointF center;
    
    bool operator==(const GeometryKey& other) const {
        return qAbs(rect.x() - other.rect.x()) < 0.001 &&
               qAbs(rect.y() - other.rect.y()) < 0.001 &&
               qAbs(rect.width() - other.rect.width()) < 0.001 &&
               qAbs(rect.height() - other.rect.height()) < 0.001 &&
               qAbs(angle - other.angle) < 0.001 &&
               qAbs(center.x() - other.center.x()) < 0.001 &&
               qAbs(center.y() - other.center.y()) < 0.001;
    }
};

/**
 * @brief GeometryKey的哈希函数
 */
struct GeometryKeyHash {
    std::size_t operator()(const GeometryKey& k) const {
        // 使用std::hash结合多个值
        std::size_t h1 = std::hash<double>{}(k.rect.x());
        std::size_t h2 = std::hash<double>{}(k.rect.y());
        std::size_t h3 = std::hash<double>{}(k.rect.width());
        std::size_t h4 = std::hash<double>{}(k.rect.height());
        std::size_t h5 = std::hash<double>{}(k.angle);
        std::size_t h6 = std::hash<double>{}(k.center.x());
        std::size_t h7 = std::hash<double>{}(k.center.y());
        
        // 组合哈希值
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6);
    }
};

/**
 * @brief 高级几何计算缓存系统
 * 
 * 该类提供了高效的几何计算缓存，避免重复计算相同的多边形变换，
 * 显著提升复杂几何操作的性能。
 */
class GeometryCache
{
public:
    /**
     * @brief 获取单例实例
     */
    static GeometryCache& instance();
    
    /**
     * @brief 获取缓存的旋转多边形
     * @param rect 矩形
     * @param angle 旋转角度
     * @param center 旋转中心
     * @return 旋转后的多边形
     */
    QPolygonF getCachedRotatedPolygon(const QRectF& rect, qreal angle, const QPointF& center);
    
    /**
     * @brief 清除缓存
     */
    void clearCache();
    
    /**
     * @brief 获取缓存统计信息
     */
    struct CacheStats {
        size_t totalRequests = 0;
        size_t cacheHits = 0;
        size_t cacheMisses = 0;
        size_t currentCacheSize = 0;
        
        double hitRate() const {
            return totalRequests > 0 ? static_cast<double>(cacheHits) / totalRequests : 0.0;
        }
    };
    
    CacheStats getStats() const { return m_stats; }
    
    /**
     * @brief 设置最大缓存大小
     */
    void setMaxCacheSize(size_t maxSize) { m_maxCacheSize = maxSize; }

private:
    GeometryCache() = default;
    ~GeometryCache() = default;
    
    // 禁止拷贝和赋值
    GeometryCache(const GeometryCache&) = delete;
    GeometryCache& operator=(const GeometryCache&) = delete;
    
    /**
     * @brief 实际计算旋转多边形（无缓存）
     */
    QPolygonF calculateRotatedPolygon(const QRectF& rect, qreal angle, const QPointF& center);
    
    /**
     * @brief 清理最老的缓存项（LRU策略）
     */
    void evictOldest();
    
    // 多边形缓存
    std::unordered_map<GeometryKey, QPolygonF, GeometryKeyHash> m_polygonCache;
    
    // LRU跟踪（简化版）
    std::unordered_map<GeometryKey, int, GeometryKeyHash> m_accessOrder;
    int m_accessCounter = 0;
    
    // 缓存统计
    mutable CacheStats m_stats;
    
    // 最大缓存大小
    size_t m_maxCacheSize = 1000;
};

#endif // GEOMETRYCACHE_H

#ifndef SHAREDRESOURCEPOOL_H
#define SHAREDRESOURCEPOOL_H

#include <QPixmap>
#include <QString>
#include <unordered_map>
#include <memory>

/**
 * @brief 共享资源池 - 优化内存使用
 * 
 * 该类实现了单例模式的共享资源池，避免重复加载相同的图片资源，
 * 显著减少内存使用量，提升应用程序性能。
 */
class SharedResourcePool
{
public:
    /**
     * @brief 获取单例实例
     * @return 共享资源池的唯一实例
     */
    static SharedResourcePool& instance();
    
    /**
     * @brief 获取缓存的QPixmap，如果不存在则加载并缓存
     * @param path 图片文件路径
     * @return 对应的QPixmap引用（共享资源）
     */
    const QPixmap& getPixmap(const QString& path);
    
    /**
     * @brief 获取缩放后的QPixmap，带尺寸缓存
     * @param path 原始图片文件路径
     * @param width 目标宽度
     * @param height 目标高度
     * @return 缩放后的QPixmap引用
     */
    const QPixmap& getScaledPixmap(const QString& path, int width, int height);
    
    /**
     * @brief 清除缓存（可选，用于内存管理）
     */
    void clearCache();
    
    /**
     * @brief 获取缓存统计信息
     * @return 当前缓存的图片数量
     */
    size_t getCacheSize() const;

private:
    SharedResourcePool() = default;
    ~SharedResourcePool() = default;
    
    // 禁止拷贝和赋值
    SharedResourcePool(const SharedResourcePool&) = delete;
    SharedResourcePool& operator=(const SharedResourcePool&) = delete;
    
    // 原始图片缓存
    std::unordered_map<QString, QPixmap> m_pixmapCache;
    
    // 缩放图片缓存（路径+尺寸作为key）
    std::unordered_map<QString, QPixmap> m_scaledPixmapCache;
    
    /**
     * @brief 生成缩放图片的缓存key
     * @param path 原始路径
     * @param width 宽度
     * @param height 高度
     * @return 组合的缓存key
     */
    QString generateScaledKey(const QString& path, int width, int height) const;
};

#endif // SHAREDRESOURCEPOOL_H

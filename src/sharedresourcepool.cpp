#include "sharedresourcepool.h"
#include <QDebug>

SharedResourcePool& SharedResourcePool::instance()
{
    static SharedResourcePool instance;
    return instance;
}

const QPixmap& SharedResourcePool::getPixmap(const QString& path)
{
    // 检查缓存中是否已存在
    auto it = m_pixmapCache.find(path);
    if (it != m_pixmapCache.end()) {
        return it->second;
    }
    
    // 加载新图片并缓存
    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        qWarning() << "Failed to load pixmap from path:" << path;
        // 返回一个默认的空白pixmap
        static QPixmap defaultPixmap(100, 100);
        defaultPixmap.fill(Qt::gray);
        return defaultPixmap;
    }
    
    // 插入缓存并返回引用
    auto result = m_pixmapCache.emplace(path, std::move(pixmap));
    return result.first->second;
}

const QPixmap& SharedResourcePool::getScaledPixmap(const QString& path, int width, int height)
{
    QString scaledKey = generateScaledKey(path, width, height);
    
    // 检查缩放缓存
    auto it = m_scaledPixmapCache.find(scaledKey);
    if (it != m_scaledPixmapCache.end()) {
        return it->second;
    }
    
    // 获取原始图片
    const QPixmap& originalPixmap = getPixmap(path);
    
    // 创建缩放版本
    QPixmap scaledPixmap = originalPixmap.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    
    // 缓存缩放版本
    auto result = m_scaledPixmapCache.emplace(scaledKey, std::move(scaledPixmap));
    return result.first->second;
}

void SharedResourcePool::clearCache()
{
    m_pixmapCache.clear();
    m_scaledPixmapCache.clear();
    qDebug() << "SharedResourcePool: Cache cleared";
}

size_t SharedResourcePool::getCacheSize() const
{
    return m_pixmapCache.size() + m_scaledPixmapCache.size();
}

QString SharedResourcePool::generateScaledKey(const QString& path, int width, int height) const
{
    return QString("%1_%2x%3").arg(path).arg(width).arg(height);
}

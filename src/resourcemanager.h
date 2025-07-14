#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QString>
#include <QMap>

class ResourceManager {
public:
    enum ImageType {
        DEFAULT_RECT,
        BACKGROUND,
        ARROW_HEAD,
        CUSTOM
    };

    static ResourceManager& instance() {
        static ResourceManager instance;
        return instance;
    }

    QString getImagePath(ImageType type) const {
        return imagePaths.value(type, ":/image/images/a1.png");
    }

    void setImagePath(ImageType type, const QString& path) {
        imagePaths[type] = path;
    }

private:
    ResourceManager() {
        // 初始化默认路径
        imagePaths[DEFAULT_RECT] = ":/image/images/a1.png";
        imagePaths[BACKGROUND] = ":/image/images/a1.png";
        imagePaths[ARROW_HEAD] = ":/image/images/a1.png";
    }

    QMap<ImageType, QString> imagePaths;
};

#endif // RESOURCEMANAGER_H

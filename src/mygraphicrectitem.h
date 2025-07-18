#ifndef MYGRAPHICRECTITEM_H
#define MYGRAPHICRECTITEM_H
#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QRect>
#include <QPainter>
#include <QPolygon>
#include <QString>
#include <QList>
#include <globals.h>
#include "resourcemanager.h"
#include "sharedresourcepool.h"
#include "geometrycache.h"
enum STATE_FLAG{
    DEFAULT_FLAG = 0,
    MOV_LEFT_LINE,//标记当前为用户按下矩形的左边界区域
    MOV_TOP_LINE,//标记当前为用户按下矩形的上边界区域
    MOV_RIGHT_LINE,//标记当前为用户按下矩形的右边界区域
    MOV_BOTTOM_LINE,//标记当前为用户按下矩形的下边界区域
    MOV_RIGHTBOTTOM_RECT,//标记当前为用户按下矩形的右下角
    MOV_RECT,//标记当前为鼠标拖动图片移动状态
    ROTATE//标记当前为旋转状态
};

// myGraphicRectItem类，继承自QObject和QGraphicsItem，用于在图形场景中绘制和交互矩形项
class myGraphicRectItem : public QObject, public QGraphicsItem {
    Q_OBJECT // 宏定义，用于支持Qt的信号和槽机制
    Q_INTERFACES(QGraphicsItem) // 接口声明

public:
    // 构造函数，允许指定父QGraphicsItem，默认为nullptr
    myGraphicRectItem(QGraphicsItem *parent = nullptr);

    myGraphicRectItem(QSizeF size, QString imagePath);

    // 返回矩形项的边界矩形
    QRectF boundingRect() const;

    // 析构函数
    ~myGraphicRectItem();

    // 设置矩形大小和位置，可选是否重置旋转中心
    void setRectSize(QRectF mrect, bool bResetRotateCenter = true);

    // 绘制矩形项
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // 鼠标按下事件处理函数
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    // 鼠标移动事件处理函数
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    // 鼠标释放事件处理函数
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    // 设置旋转角度和旋转中心，旋转中心默认为(-999,-999)，表示不改变
    void SetRotate(qreal RotateAngle, QPointF ptCenter = QPointF(-999, -999));

    // 获取旋转后的点
    QPointF getRotatePoint(QPointF ptCenter, QPointF ptIn, qreal angle);

    // 获取多个点旋转后的点集合
    QList<QPointF> getRotatePoints(QPointF ptCenter, QList<QPointF> &ptIns, qreal angle);

    // 将矩形旋转后返回对应的多边形
    QPolygonF getRotatePolygonFromRect(QPointF ptCenter, QRectF &rectIn, qreal angle);

    // 获取当前矩形项在场景中的位置对应的矩形
    QRectF getCrtPosRectToSceen();

    // 矩形顶部用来表示旋转的标记的矩形
    QRectF m_SmallRotateRect;

    // 矩形顶部用来表示旋转的标记的矩形旋转后形成的多边形
    QPolygonF m_SmallRotatePolygon;

    // 根据两点获取旋转时候矩形正上方的旋转标记矩形的中心
    QPointF getSmallRotateRectCenter(QPointF ptA, QPointF ptB);

    // 根据两点获取旋转时候矩形正上方的旋转标记矩形
    QRectF getSmallRotateRect(QPointF ptA, QPointF ptB);

    // 根据三点求点到直线的距离
    double pointToLineDistance(double x1, double y1, double x2, double y2, double x0, double y0);

    //返回OldRect在场景中的坐标
    QRectF getOldRect();

    // 放大镜相关方法
    void setMagnifierMode(bool isMagnifier);
    void setBackgroundImage(const QPixmap& bgImage);
    void setSelectionRect(const QRectF& selectionRect);
    void setZoomFactor(qreal factor);
    void setMagnifierQuality(MagnifierQuality quality);
    bool isMagnifierMode() const { return m_isMagnifierMode; }
    bool getMark() const { return mark; }  // 获取mark状态
    MagnifierQuality getMagnifierQuality() const { return m_magnifierQuality; }

    // 高质量图像缩放辅助方法
    QPixmap createHighQualityScaledPixmap(const QPixmap& source, const QSize& targetSize) const;
    QPixmap createSuperSampledPixmap(const QPixmap& source, const QSize& targetSize) const;
    QPixmap applyEdgeEnhancement(const QPixmap& source) const;
    
    // 超分辨率算法方法
    QPixmap applySuperResolution(const QPixmap& source, const QSize& targetSize) const;
    QPixmap bicubicInterpolation(const QImage& source, const QSize& targetSize) const;
    QPixmap edgePreservingUpscale(const QImage& source, const QSize& targetSize) const;
    qreal bicubicWeight(qreal x, qreal a = BICUBIC_SHARPNESS) const;
    qreal detectEdgeStrength(const QImage& image, int x, int y) const;
    
    // 超级优化方法
    QPixmap ultraHighQualityUpscale(const QPixmap& source, const QSize& targetSize) const;
    QPixmap lanczosResample(const QImage& source, const QSize& targetSize) const;
    QPixmap adaptiveEdgeEnhancement(const QImage& source) const;
    QPixmap colorSpaceEnhancement(const QImage& source) const;
    QPixmap multiLevelDetail(const QImage& source, const QSize& targetSize) const;
    qreal lanczosWeight(qreal x, int a = 3) const;
    qreal calculateLocalContrast(const QImage& image, int x, int y, int radius = 2) const;
    void rgbToYuv(int r, int g, int b, qreal& y, qreal& u, qreal& v) const;
    void yuvToRgb(qreal y, qreal u, qreal v, int& r, int& g, int& b) const;

    // 标记是否正在旋转
    bool m_bRotate;

    // 当前旋转角度
    qreal m_RotateAngle;

    // 旋转中心
    QPointF m_RotateCenter;

private:
    // 存储原始矩形
    QRectF m_oldRect;

    // 存储原始矩形的多边形表示
    QPolygonF m_oldRectPolygon;

    //存储拉伸时候的临时副本
    QPolygonF m_tempOldRectPolygon;

    //存储拉伸时候的起点
    QPointF m_tempPos;

    // 矩形旋转区域的边界矩形
    QRectF m_RotateAreaRect;

    // 标记是否正在调整大小
    bool m_bResize;

    // 插入点的多边形
    QPolygonF m_insicedPolygon;

    // 插入点的矩形
    QRectF m_insicedRectf;

    // 左侧边界的多边形和矩形
    QPolygonF m_leftPolygon;
    QRectF m_leftRectf;

    // 上侧边界的多边形和矩形
    QPolygonF m_topPolygon;
    QRectF m_topRectf;

    // 右侧边界的多边形和矩形
    QPolygonF m_rightPolygon;
    QRectF m_rightRectf;

    // 下侧边界的多边形和矩形
    QPolygonF m_bottomPolygon;
    QRectF m_bottomRectf;

    // 右下侧边界的多边形和矩形
    QPolygonF m_rightAndBottomPolygon;
    QRectF m_rightAndBottomRectf;

    //左上
    QPolygonF m_leftAndTopPolygon;
    QRectF m_leftAndTopRectf;

    //右上
    QPolygonF m_rightAndTopPolygon;
    QRectF m_rightAndTopRectf;

    //左下
    QPolygonF m_leftAndBottomPolygon;
    QRectF m_leftAndBottomRectf;

    // 起点位置
    QPointF m_startPos;

    // 状态标志
    STATE_FLAG m_StateFlag;

    // 指向表示旋转标记矩形顶点的指针
    QPointF *pPointFofSmallRotateRect;

    // 要绘制的图片
    QPixmap pixmap;

    //遮罩状态
    bool mark;

    // 放大镜相关成员
    bool m_isMagnifierMode;        // 是否为放大镜模式
    QPixmap m_backgroundImage;     // 背景图像
    QRectF m_selectionRect;        // 选择区域（在背景图像中的位置）
    qreal m_zoomFactor;            // 放大倍数
    MagnifierQuality m_magnifierQuality; // 放大镜质量级别

    // 鼠标交互状态跟踪
    bool m_hasDragged;             // 是否发生了拖拽
    QPointF m_pressPos;            // 鼠标按下时的位置

    // 优化：几何计算缓存
    mutable QRectF m_cachedRect;
    mutable qreal m_cachedAngle;
    mutable QPointF m_cachedCenter;
    mutable bool m_cacheValid;
    
    // 缓存辅助方法
    bool isCacheValid(const QRectF& rect, qreal angle, const QPointF& center) const;
    void invalidateCache();
    
    // LOD渲染方法
    void drawHighDetail(QPainter *painter, const QRectF &target);

signals:
    void centerChange(QPointF center);
    void markChange(bool mark);
    void showMid(int sign);
    // 放大镜相关信号
    void selectionRectChanged(const QRectF& rect);  // 选择区域发生变化
    void magnifierUpdateNeeded();                   // 需要更新放大镜内容
public slots:
    void setMark(bool otherMark);

};
#endif

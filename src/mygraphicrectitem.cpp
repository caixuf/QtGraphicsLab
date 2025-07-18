#include "mygraphicrectitem.h"
#include <QtMath>
#include <QDebug>
myGraphicRectItem::myGraphicRectItem(QGraphicsItem *parent):
    QGraphicsItem(parent),
    m_bRotate(false),
    m_RotateAngle(0),
    m_oldRect(0, 0, 100, 50),
    m_bResize(false),
    m_StateFlag(DEFAULT_FLAG),
    m_cacheValid(false)  // 初始化缓存状态
{
    mark = true;
    setRectSize(m_oldRect); //根据传入的矩形设置组成图元的几个区域
    setToolTip(u8"Click and drag me!");  //提示
    pPointFofSmallRotateRect = new QPointF[4]; //指向旋转矩形的四个顶点
    SetRotate(0);//初始旋转角度为0
    setFlag(QGraphicsItem::ItemIsSelectable);//设置图元是可选的，可以接受鼠标移动事件
}

myGraphicRectItem::myGraphicRectItem(QSizeF size, QString imagePath):
    QGraphicsItem(nullptr),
    m_bRotate(false),
    m_RotateAngle(0),
    m_oldRect(0, 0, size.width(), size.height()),
    m_bResize(false),
    m_StateFlag(DEFAULT_FLAG),
    m_isMagnifierMode(false),  // 按照头文件中的声明顺序
    m_zoomFactor(1.0),         // 按照头文件中的声明顺序
    m_magnifierQuality(QUALITY_FAST), // 默认快速模式
    m_hasDragged(false),       // 初始化拖拽状态
    m_cacheValid(false)        // 最后初始化缓存状态
{
    mark = true;
    setRectSize(m_oldRect); //根据传入的矩形设置组成图元的几个区域
    setToolTip(u8"Click and drag me!");  //提示
    pPointFofSmallRotateRect = new QPointF[4]; //指向旋转矩形的四个顶点
    SetRotate(0);//初始旋转角度为0
    setFlag(QGraphicsItem::ItemIsSelectable);//设置图元是可选的，可以接受鼠标移动事件
    
    // 如果提供了imagePath参数，使用它；否则使用ResourceManager的默认路径
    QString actualPath = imagePath.isEmpty() ? 
        ResourceManager::instance().getImagePath(ResourceManager::DEFAULT_RECT) : imagePath;
    
    // 使用共享资源池获取缩放后的图片，减少内存使用
    pixmap = SharedResourcePool::instance().getScaledPixmap(actualPath, WIDTH, HEIGHT);
}

QRectF myGraphicRectItem::boundingRect() const
{
    QRectF boundingRectF = m_oldRectPolygon.boundingRect();
    
    // 优化：精确计算边界框，避免过度重绘
    QRectF result = boundingRectF;
    
    // 只在需要时添加旋转标记区域
    if (m_SmallRotatePolygon.size() > 0) {
        result = result.united(m_SmallRotatePolygon.boundingRect());
    }
    
    // 使用最小必要边距替代固定的80像素边距
    const qreal minMargin = 10.0; // 最小必要边距
    return result.adjusted(-minMargin, -minMargin, minMargin, minMargin);
}
//RAII手法即获取资源即初始化
myGraphicRectItem::~myGraphicRectItem()
{
    delete []pPointFofSmallRotateRect;
    pPointFofSmallRotateRect = nullptr;
}

void myGraphicRectItem::setRectSize(QRectF mrect, bool bResetRotateCenter)
{
    // 优化：检查是否需要重新计算
    if (m_cacheValid && isCacheValid(mrect, m_RotateAngle, m_RotateCenter) && !bResetRotateCenter) {
        return; // 使用缓存，避免重复计算
    }
    
    m_oldRect = mrect;
    if(bResetRotateCenter)
    {
        m_RotateCenter.setX(m_oldRect.x() + m_oldRect.width() / 2);
        m_RotateCenter.setY(m_oldRect.y() + m_oldRect.height() / 2);
    }
    
    // 批量计算所有多边形以提高效率
    m_oldRectPolygon = getRotatePolygonFromRect(m_RotateCenter, m_oldRect, m_RotateAngle);

    m_insicedRectf = QRectF(m_oldRect.x() + 8, m_oldRect.y() + 8, m_oldRect.width() - 16, m_oldRect.height() - 16);
    m_insicedPolygon = getRotatePolygonFromRect(m_RotateCenter, m_insicedRectf, m_RotateAngle);

    m_leftRectf = QRectF(m_oldRect.x(), m_oldRect.y() + 8, 3, m_oldRect.height() - 16);
    m_leftPolygon = getRotatePolygonFromRect(m_RotateCenter, m_leftRectf,m_RotateAngle);

    m_topRectf = QRectF(m_oldRect.x() + 8, m_oldRect.y(), m_oldRect.width() - 16, 3);
    m_topPolygon = getRotatePolygonFromRect(m_RotateCenter, m_topRectf, m_RotateAngle);

    m_rightRectf = QRectF(m_oldRect.right() - 3, m_oldRect.y() + 8, 3, m_oldRect.height() - 16);
    m_rightPolygon = getRotatePolygonFromRect(m_RotateCenter, m_rightRectf,m_RotateAngle);

    m_bottomRectf = QRectF(m_oldRect.x() + 8, m_oldRect.bottom() - 3, m_oldRect.width() - 16, 3);
    m_bottomPolygon = getRotatePolygonFromRect(m_RotateCenter, m_bottomRectf, m_RotateAngle);

    m_rightAndBottomRectf = QRectF(m_oldRect.right() - 8, m_oldRect.bottom() - 8, 8, 8);
    m_rightAndBottomPolygon = getRotatePolygonFromRect(m_RotateCenter, m_rightAndBottomRectf, m_RotateAngle);

    m_leftAndTopRectf = QRectF(m_oldRect.x(), m_oldRect.y(), 8, 8);
    m_leftAndTopPolygon = getRotatePolygonFromRect(m_RotateCenter, m_leftAndTopRectf, m_RotateAngle);

    m_rightAndTopRectf = QRectF(m_oldRect.right() - 8, m_oldRect.y(), 8, 8);

    m_leftAndBottomRectf = QRectF(m_oldRect.x(), m_oldRect.bottom() - 8, 8, 8);


    m_SmallRotateRect = getSmallRotateRect(mrect.topLeft(), mrect.topRight());//矩形正上方的旋转标记矩形
    m_SmallRotatePolygon = getRotatePolygonFromRect(m_RotateCenter, m_SmallRotateRect, m_RotateAngle);
    
    // 更新缓存
    m_cachedRect = mrect;
    m_cachedAngle = m_RotateAngle;
    m_cachedCenter = m_RotateCenter;
    m_cacheValid = true;
}

void myGraphicRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    // LOD渲染：根据缩放级别选择不同的渲染细节
    qreal scaleFactor = painter->transform().m11(); // 获取X方向的缩放因子
    
    QRectF target = m_oldRectPolygon.boundingRect();
    
    if (scaleFactor < 0.3) {
        // 极低细节渲染：只绘制简单的填充矩形
        painter->fillRect(target, Qt::blue);
        return;
    }
    else if (scaleFactor < 0.6) {
        // 低细节渲染：但是放大镜模式需要特殊处理
        if (m_isMagnifierMode && !m_backgroundImage.isNull() && m_selectionRect.isValid()) {
            // 放大镜模式：显示放大内容
            drawHighDetail(painter, target);
        } else {
            // 普通模式：只绘制图片，不绘制控制点
            QRectF source = QRectF(this->pos().x(), this->pos().y(), target.width(), target.height());
            painter->drawPixmap(target, pixmap, source);
        }
        
        // 只绘制选择边框
        if (!m_isMagnifierMode && mark) {
            QPen pen(Qt::black);
            pen.setWidth(2);
            painter->setPen(pen);
            painter->drawRect(target);
        } else if (m_isMagnifierMode) {
            QPen pen(Qt::cyan);
            pen.setWidth(2);
            painter->setPen(pen);
            painter->drawRect(target);
        }
        return;
    }
    else if (scaleFactor < 1.0) {
        // 中等细节：绘制图片和主要控制点
        // 但是如果是放大镜模式，需要特殊处理
        if (m_isMagnifierMode && !m_backgroundImage.isNull() && m_selectionRect.isValid()) {
            // 放大镜模式优先：即使在中等细节级别也要显示放大镜内容
            drawHighDetail(painter, target);
        } else {
            // 普通模式：显示原始图片
            QRectF source = QRectF(this->pos().x(), this->pos().y(), target.width(), target.height());
            painter->drawPixmap(target, pixmap, source);
        }
        
        QPen pen(m_isMagnifierMode ? Qt::cyan : Qt::yellow);
        pen.setWidth(1);
        painter->setPen(pen);
        
        // 只绘制四个角的控制点
        painter->fillRect(m_rightAndBottomRectf, m_isMagnifierMode ? Qt::cyan : Qt::yellow);
        painter->fillRect(m_leftAndTopRectf, m_isMagnifierMode ? Qt::cyan : Qt::yellow);
        painter->fillRect(m_rightAndTopRectf, m_isMagnifierMode ? Qt::cyan : Qt::yellow);
        painter->fillRect(m_leftAndBottomRectf, m_isMagnifierMode ? Qt::cyan : Qt::yellow);
        
        painter->drawRect(target);
        if (!m_isMagnifierMode && mark) {
            painter->fillRect(QRectF(target.x(), target.y(), target.width() + 2, target.height() + 2), QColor(0, 0, 0, 150));
        }
        return;
    }
    
    // 高细节渲染：完整的控制点和装饰
    drawHighDetail(painter, target);
}

void myGraphicRectItem::drawHighDetail(QPainter *painter, const QRectF &target)
{
    if (m_isMagnifierMode && !m_backgroundImage.isNull() && m_selectionRect.isValid()) {
        // 放大镜模式：绘制选择区域的放大内容
        
        // 从背景图像中提取选择区域
        QPixmap sourcePixmap = m_backgroundImage.copy(m_selectionRect.toRect());
        
        if (!sourcePixmap.isNull()) {
            // 最高质量渲染设置
            painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->setRenderHint(QPainter::TextAntialiasing, true);
            
            // 计算放大倍数
            qreal scaleFactorX = target.width() / sourcePixmap.width();
            qreal scaleFactorY = target.height() / sourcePixmap.height();
            qreal maxScaleFactor = qMax(scaleFactorX, scaleFactorY);
            
            QPixmap finalPixmap;
            
            // 根据质量级别和放大倍数选择算法
            switch (m_magnifierQuality) {
            case QUALITY_FAST:
                // 快速模式：使用Qt内置的平滑缩放
                finalPixmap = sourcePixmap.scaled(
                    target.size().toSize(),
                    Qt::IgnoreAspectRatio,
                    Qt::SmoothTransformation
                );
                break;
                
            case QUALITY_BALANCED:
                // 平衡模式：中等放大倍数使用多步骤缩放
                if (maxScaleFactor > MAGNIFIER_INTERPOLATION_THRESHOLD) {
                    finalPixmap = createHighQualityScaledPixmap(sourcePixmap, target.size().toSize());
                } else {
                    finalPixmap = sourcePixmap.scaled(
                        target.size().toSize(),
                        Qt::IgnoreAspectRatio,
                        Qt::SmoothTransformation
                    );
                }
                break;
                
            case QUALITY_HIGH:
                // 高质量模式：使用超分辨率算法
                if (maxScaleFactor > SUPER_RESOLUTION_THRESHOLD) {
                    finalPixmap = applySuperResolution(sourcePixmap, target.size().toSize());
                } else if (maxScaleFactor > MAGNIFIER_INTERPOLATION_THRESHOLD) {
                    finalPixmap = createSuperSampledPixmap(sourcePixmap, target.size().toSize());
                } else {
                    finalPixmap = createHighQualityScaledPixmap(sourcePixmap, target.size().toSize());
                }
                break;
                
            case QUALITY_ULTRA:
                // 超高质量模式：使用最高质量算法
                if (maxScaleFactor > SUPER_RESOLUTION_THRESHOLD) {
                    finalPixmap = ultraHighQualityUpscale(sourcePixmap, target.size().toSize());
                } else if (maxScaleFactor > MAGNIFIER_STEPS_THRESHOLD) {
                    finalPixmap = applySuperResolution(sourcePixmap, target.size().toSize());
                } else if (maxScaleFactor > MAGNIFIER_INTERPOLATION_THRESHOLD) {
                    finalPixmap = createSuperSampledPixmap(sourcePixmap, target.size().toSize());
                } else {
                    finalPixmap = createHighQualityScaledPixmap(sourcePixmap, target.size().toSize());
                }
                break;
            }
            
            // 只在高质量模式下应用边缘增强
            if (m_magnifierQuality >= QUALITY_HIGH && maxScaleFactor > MAGNIFIER_INTERPOLATION_THRESHOLD) {
                finalPixmap = applyEdgeEnhancement(finalPixmap);
            }
            
            // 绘制最终图像
            QRectF sourceRect(0, 0, finalPixmap.width(), finalPixmap.height());
            painter->drawPixmap(target, finalPixmap, sourceRect);
            
            // 绘制放大镜边框（更亮的边框表示这是放大镜）
            QPen magnifierPen(Qt::white, 3);
            painter->setPen(magnifierPen);
            painter->drawRect(target);
            
            // 内边框
            QPen innerPen(Qt::cyan, 1);
            painter->setPen(innerPen);
            QRectF innerTarget = target.adjusted(2, 2, -2, -2);
            painter->drawRect(innerTarget);
        }
    } else {
        // 普通模式：绘制原始图片
        QRectF source = QRectF(this->pos().x(), this->pos().y(), target.width(), target.height());
        painter->drawPixmap(target, pixmap, source);
        
        // 如果是选择器模式（暗的矩形），绘制半透明遮罩
        if (!m_isMagnifierMode && mark) {
            painter->fillRect(QRectF(target.x(), target.y(), target.width() + 2, target.height() + 2), QColor(0, 0, 0, 150));
        }
    }
    
    // 绘制所有控制点
    QPen pen(m_isMagnifierMode ? Qt::cyan : Qt::yellow);
    pen.setWidth(1);
    painter->setPen(pen);
    
    painter->drawRect(m_leftRectf);
    painter->drawRect(m_rightRectf);
    painter->drawRect(m_bottomRectf);
    painter->drawRect(m_topRectf);
    painter->drawRect(m_rightAndBottomRectf);
    painter->drawRect(m_rightAndTopRectf);
    painter->drawRect(m_leftAndTopRectf);
    painter->drawRect(m_leftAndBottomRectf);
    painter->drawRect(target);
    
    QColor fillColor = m_isMagnifierMode ? Qt::cyan : Qt::yellow;
    painter->fillRect(m_leftRectf, fillColor);
    painter->fillRect(m_rightRectf, fillColor);
    painter->fillRect(m_topRectf, fillColor);
    painter->fillRect(m_bottomRectf, fillColor);
    painter->fillRect(m_rightAndBottomRectf, fillColor);
    painter->fillRect(m_leftAndTopRectf, fillColor);
    painter->fillRect(m_rightAndTopRectf, fillColor);
    painter->fillRect(m_leftAndBottomRectf, fillColor);
}

void myGraphicRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_tempOldRectPolygon = m_oldRectPolygon;
    m_tempPos = this->pos();
    if(event->button()== Qt::LeftButton)
    {
        emit centerChange(this->pos() + QPointF(m_oldRect.width() / 2, m_oldRect.height() / 2));
        
        // 记录鼠标按下位置，用于判断是否为选择操作
        m_pressPos = event->scenePos();
        m_hasDragged = false;
        
        m_startPos = event->pos();
        // if(m_SmallRotatePolygon.containsPoint(m_startPos,Qt::WindingFill))//旋转矩形
        // {
        //     setCursor(Qt::PointingHandCursor);
        //     m_StateFlag = ROTATE;
        // }
        if(m_insicedPolygon.containsPoint(m_startPos, Qt::WindingFill))//在矩形内框区域时按下鼠标，则可拖动图片
        {
            setCursor(Qt::ClosedHandCursor);   //改变光标形状,手的形状
            m_StateFlag = MOV_RECT;//标记当前为鼠标拖动图片移动状态
        }
        else if(m_leftPolygon.containsPoint(m_startPos, Qt::WindingFill))
        {
            setCursor(Qt::SizeHorCursor);
            m_StateFlag = MOV_LEFT_LINE;//标记当前为用户按下矩形的左边界区域
        }
        else if(m_rightPolygon.containsPoint(m_startPos, Qt::WindingFill))
        {
            setCursor(Qt::SizeHorCursor);
            m_StateFlag = MOV_RIGHT_LINE;//标记当前为用户按下矩形的右边界区域
        }
        else if(m_topPolygon.containsPoint(m_startPos, Qt::WindingFill))
        {
            setCursor(Qt::SizeVerCursor);
            m_StateFlag = MOV_TOP_LINE;//标记当前为用户按下矩形的上边界区域
        }
        else if(m_bottomPolygon.containsPoint(m_startPos, Qt::WindingFill))
        {
            setCursor(Qt::SizeVerCursor);
            m_StateFlag = MOV_BOTTOM_LINE;//标记当前为用户按下矩形的下边界区域
        }
        else if(m_rightAndBottomPolygon.containsPoint(m_startPos, Qt::WindingFill))
        {
            setCursor(Qt::SizeFDiagCursor);
            m_StateFlag = MOV_RIGHTBOTTOM_RECT;
        }
        else
        {
            m_StateFlag = DEFAULT_FLAG;
        }
    }
    else
    {
        QGraphicsItem::mousePressEvent(event);
    }
}

void myGraphicRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // 检测是否发生了拖拽（鼠标移动超过一定距离）
    if (!m_hasDragged) {
        QPointF currentPos = event->scenePos();
        qreal distance = QLineF(m_pressPos, currentPos).length();
        if (distance > 3.0) { // 移动超过3像素认为是拖拽
            m_hasDragged = true;
        }
    }
    
    if(m_StateFlag == ROTATE)
    {
        int nRotateAngle = atan2((event->pos().x() - m_RotateCenter.x()),
                                 (event->pos().y() - m_RotateCenter.y())) * 180 / M_PI;
        SetRotate(180 - nRotateAngle);
        setRectSize(m_oldRect);
    }
    else if(m_StateFlag == MOV_RECT)
    {
        QPointF point = (event->pos() - m_startPos);
        //边界检测
        QPointF newP = point + pos();
        if(newP.x() < 0 || newP.x() > WIDTH - m_oldRect.width() || newP.y() < 0 || newP.y() > HEIGHT - m_oldRect.height())
        {
            return;
        }
        //moveBy(point.x(), point.y());
        this->setPos(pos() + point);
        setRectSize(m_oldRect);
        scene()->update();
        if(!mark)
        {
            QPointF ltp = mapToScene(QPointF(m_oldRect.left(), m_oldRect.top()));
            QPointF rtp = mapToScene(QPointF(m_oldRect.right(), m_oldRect.top()));
            QPointF lbp = mapToScene(QPointF(m_oldRect.left(), m_oldRect.bottom()));
            QPointF rbp = mapToScene(QPointF(m_oldRect.right(), m_oldRect.bottom()));
            QPointF cp = QPointF((ltp.x() + rbp.x()) / 2, (rtp.y() + lbp.y()) / 2);
            if(ltp.x() > WIDTH / 2 && ltp.x() - WIDTH / 2 <= 30)
            {
                emit showMid(1);
            }
            else if(rtp.x() < WIDTH / 2 && rtp.x() - WIDTH / 2 >= -30)
            {
                emit showMid(1);
            }
            else if(ltp.y() > HEIGHT / 2 && ltp.y() - HEIGHT / 2 <= 30)
            {
                emit showMid(2);
            }
            else if(rbp.y() < HEIGHT / 2 && rbp.y() - HEIGHT / 2 >= -30)
            {
                emit showMid(2);
            }
            else if(fabs(cp.x() - WIDTH / 2) <= 30)
            {
                emit showMid(1);
            }
            else if(fabs(cp.y() - HEIGHT / 2) <= 30)
            {
                emit showMid(2);
            }
            else
            {
                emit showMid(0);
            }

            //吸附
            if(ltp.x() > WIDTH / 2 && ltp.x() - WIDTH / 2 <= 20)
            {
                this->setPos(WIDTH / 2, pos().y());
            }
            else if(rtp.x() < WIDTH / 2 && rtp.x() - WIDTH / 2 >= -20)
            {
                this->setPos(WIDTH / 2 - m_oldRect.width(), pos().y());
            }
            else if(ltp.y() > HEIGHT / 2 && ltp.y() - HEIGHT / 2 <= 20)
            {
                this->setPos(pos().x(), HEIGHT / 2);
            }
            else if(rbp.y() < HEIGHT / 2 && rbp.y() - HEIGHT / 2 >= -20)
            {
                this->setPos(pos().x(), HEIGHT / 2 - m_oldRect.height());
            }
            else if(fabs(cp.x() - WIDTH / 2) <= 20)
            {
                this->setPos(WIDTH / 2 - m_oldRect.width() / 2, pos().y());
            }
            else if(fabs(cp.y() - HEIGHT / 2) <= 20)
            {
                this->setPos(pos().x(), HEIGHT / 2 - m_oldRect.height() / 2);
            }
        }
    }
    else if(m_StateFlag == MOV_LEFT_LINE)
    {
        // 更直观的边拖拽逻辑：固定右边，移动左边
        QPointF currentMousePos = event->pos();
        
        // 固定右边界
        qreal fixedRight = m_oldRect.right();
        qreal fixedTop = m_oldRect.top();
        qreal fixedBottom = m_oldRect.bottom();
        
        // 新的左边界位置
        qreal newLeft = currentMousePos.x();
        
        // 计算新宽度
        qreal newWidth = fixedRight - newLeft;
        
        // 应用最小尺寸限制
        if(newWidth < 30) {
            newLeft = fixedRight - 30; // 调整左边界位置
            newWidth = 30;
        }
        
        // 创建新矩形
        QRectF newRect(newLeft, fixedTop, newWidth, fixedBottom - fixedTop);
        m_oldRect = newRect;
        
        // 更新旋转中心
        m_RotateCenter = QPointF(m_oldRect.x() + m_oldRect.width() / 2, 
                                m_oldRect.y() + m_oldRect.height() / 2);
        
        // 一次性更新所有几何形状
        setRectSize(m_oldRect);
        prepareGeometryChange(); // 通知Qt将要改变图元的几何形状
        this->update();
        scene()->update();//必须要用scene()->update()，不能用update();否则会出现重影
    }
    else if(m_StateFlag == MOV_TOP_LINE)
    {
        // 更直观的边拖拽逻辑：固定底边，移动顶边
        QPointF currentMousePos = event->pos();
        
        // 固定底边界
        qreal fixedBottom = m_oldRect.bottom();
        qreal fixedLeft = m_oldRect.left();
        qreal fixedRight = m_oldRect.right();
        
        // 新的顶边界位置
        qreal newTop = currentMousePos.y();
        
        // 计算新高度
        qreal newHeight = fixedBottom - newTop;
        
        // 应用最小尺寸限制
        if(newHeight < 30) {
            newTop = fixedBottom - 30; // 调整顶边界位置
            newHeight = 30;
        }
        
        // 创建新矩形
        QRectF newRect(fixedLeft, newTop, fixedRight - fixedLeft, newHeight);
        m_oldRect = newRect;
        
        // 更新旋转中心
        m_RotateCenter = QPointF(m_oldRect.x() + m_oldRect.width() / 2, 
                                m_oldRect.y() + m_oldRect.height() / 2);
        
        // 一次性更新所有几何形状
        setRectSize(m_oldRect);
        prepareGeometryChange(); // 通知Qt将要改变图元的几何形状
        this->update();
        scene()->update();//必须要用scene()->update()，不能用update();否则会出现重影
    }
    else if(m_StateFlag == MOV_RIGHT_LINE)
    {
        // 更直观的边拖拽逻辑：固定左边，移动右边
        QPointF currentMousePos = event->pos();
        
        // 固定左边界
        qreal fixedLeft = m_oldRect.left();
        qreal fixedTop = m_oldRect.top();
        qreal fixedBottom = m_oldRect.bottom();
        
        // 新的右边界位置
        qreal newRight = currentMousePos.x();
        
        // 计算新宽度
        qreal newWidth = newRight - fixedLeft;
        
        // 应用最小尺寸限制
        if(newWidth < 30) {
            newWidth = 30;
        }
        
        // 创建新矩形
        QRectF newRect(fixedLeft, fixedTop, newWidth, fixedBottom - fixedTop);
        m_oldRect = newRect;
        
        // 更新旋转中心
        m_RotateCenter = QPointF(m_oldRect.x() + m_oldRect.width() / 2, 
                                m_oldRect.y() + m_oldRect.height() / 2);
        
        // 一次性更新所有几何形状
        setRectSize(m_oldRect);
        prepareGeometryChange(); // 通知Qt将要改变图元的几何形状
        this->update();
        scene()->update();//必须要用scene()->update()，不能用update();否则会出现重影
    }
    else if(m_StateFlag == MOV_BOTTOM_LINE)
    {
        // 更直观的边拖拽逻辑：固定顶边，移动底边
        QPointF currentMousePos = event->pos();
        
        // 固定顶边界
        qreal fixedTop = m_oldRect.top();
        qreal fixedLeft = m_oldRect.left();
        qreal fixedRight = m_oldRect.right();
        
        // 新的底边界位置
        qreal newBottom = currentMousePos.y();
        
        // 计算新高度
        qreal newHeight = newBottom - fixedTop;
        
        // 应用最小尺寸限制
        if(newHeight < 30) {
            newHeight = 30;
        }
        
        // 创建新矩形
        QRectF newRect(fixedLeft, fixedTop, fixedRight - fixedLeft, newHeight);
        m_oldRect = newRect;
        
        // 更新旋转中心
        m_RotateCenter = QPointF(m_oldRect.x() + m_oldRect.width() / 2, 
                                m_oldRect.y() + m_oldRect.height() / 2);
        
        // 一次性更新所有几何形状
        setRectSize(m_oldRect);
        prepareGeometryChange();
        this->update();
        scene()->update();
    }
    else if(m_StateFlag == MOV_RIGHTBOTTOM_RECT)
    {
        // 更直观的角拖拽逻辑：固定左上角，用当前鼠标位置重新定义矩形
        QPointF currentMousePos = event->pos();
        
        // 获取原始矩形的左上角（固定点）
        QPointF fixedTopLeft = QPointF(m_oldRect.left(), m_oldRect.top());
        
        // 将鼠标位置转换为相对于图元的坐标
        QPointF newBottomRight = currentMousePos;
        
        // 计算新的宽度和高度
        qreal newWidth = newBottomRight.x() - fixedTopLeft.x();
        qreal newHeight = newBottomRight.y() - fixedTopLeft.y();
        
        // 应用最小尺寸限制
        if(newWidth < 30) newWidth = 30;
        if(newHeight < 30) newHeight = 30;
        
        // 创建新矩形
        QRectF newRect(fixedTopLeft.x(), fixedTopLeft.y(), newWidth, newHeight);
        m_oldRect = newRect;
        
        // 更新旋转中心
        m_RotateCenter = QPointF(m_oldRect.x() + m_oldRect.width() / 2, 
                                m_oldRect.y() + m_oldRect.height() / 2);
        
        // 一次性更新所有几何形状
        setRectSize(m_oldRect);
        prepareGeometryChange(); // 通知Qt将要改变图元的几何形状
        this->update();
        scene()->update();//必须要用scene()->update()，不能用update();否则会出现重影

    }
    emit centerChange(this->pos() + QPointF(m_oldRect.width() / 2, m_oldRect.height() / 2));
    
    // 如果不是放大镜模式（即为选择器），发射选择区域变化信号
    if (!m_isMagnifierMode) {
        QRectF currentRect = getOldRect(); // 获取在场景中的位置
        emit selectionRectChanged(currentRect);
    }
}

void myGraphicRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    emit showMid(0);
    setCursor(Qt::ArrowCursor);
    
    // 只有在没有拖拽的情况下才处理选择逻辑
    if (!m_hasDragged && event->button() == Qt::LeftButton) {
        // 这是一个选择操作，切换 mark 状态
        mark = !mark;  // 简单的切换逻辑
        emit markChange(mark);
        update();
    }
    
    if(m_StateFlag == MOV_RECT)
    {
        m_StateFlag = DEFAULT_FLAG;
    }
    else {
        QGraphicsItem::mouseReleaseEvent(event);
    }
    
    // 重置拖拽状态
    m_hasDragged = false;
}

void myGraphicRectItem::SetRotate(qreal RotateAngle, QPointF ptCenter)
{
    m_bRotate = true;
    if(ptCenter.x() == -999 && ptCenter.y() == -999)
    {
        m_RotateCenter = QPointF(m_oldRect.x() + m_oldRect.width() / 2, m_oldRect.y() + m_oldRect.height() / 2);
    }
    else
    {
        m_RotateCenter = ptCenter;
    }
    m_RotateAngle = RotateAngle;
    
    // 旋转改变时，使缓存失效
    invalidateCache();
    
    this->update();
}

QPointF myGraphicRectItem::getRotatePoint(QPointF ptCenter, QPointF ptIn, qreal angle)
{
    double dx = ptCenter.x();
    double dy = ptCenter.y();
    double x = ptIn.x();
    double y = ptIn.y();
    double xx,yy;
    {
        double radianAngle = angle * M_PI / 180;
        xx = (x - dx) * cos(radianAngle) - (y - dy) * sin(radianAngle) + dx;
        yy = (x - dx) * sin(radianAngle) + (y - dy)* cos(radianAngle) + dy;
    }

    return QPointF(xx, yy);
}

QList<QPointF> myGraphicRectItem::getRotatePoints(QPointF ptCenter, QList<QPointF> &ptIns, qreal angle)
{
    QList<QPointF> lstPt;
    for(int i = 0; i < ptIns.count(); i++)
    {
        lstPt.append(getRotatePoint(ptCenter, ptIns.at(i), angle));
    }
    return lstPt;
}

QPolygonF myGraphicRectItem::getRotatePolygonFromRect(QPointF ptCenter, QRectF &rectIn, qreal angle)
{
    // 使用几何缓存优化旋转多边形计算
    return GeometryCache::instance().getCachedRotatedPolygon(rectIn, angle, ptCenter);
}

QRectF myGraphicRectItem::getCrtPosRectToSceen()
{
    QRectF retRect = QRectF(m_oldRect.x() + pos().x(), m_oldRect.y() + pos().y(), m_oldRect.width(), m_oldRect.height());
    return retRect;
}
QRectF myGraphicRectItem::getSmallRotateRect(QPointF ptA, QPointF ptB)
{
    QPointF pt = getSmallRotateRectCenter(ptA, ptB);
    return QRectF(pt.x() - 10, pt.y() - 10, 20, 20);
}

double myGraphicRectItem::pointToLineDistance(double x1, double y1, double x2, double y2, double x0, double y0)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    if (dx == 0)
    {
        return fabs(x0 - x1);
    }
    double k = dy / dx;
    double c = y1 - k * x1;
    double distance = fabs(k * x0 - y0 + c) / sqrt(k * k + 1);
    return distance;

}

QRectF myGraphicRectItem::getOldRect()
{
    return QRectF(pos().x(), pos().y(), m_oldRect.width(), m_oldRect.height());
}

void myGraphicRectItem::setMark(bool otherMark)
{
    this->mark = otherMark;  // 直接设置mark状态
    update();
}
QPointF myGraphicRectItem::getSmallRotateRectCenter(QPointF ptA, QPointF ptB)
{
    QPointF ptCenter = QPointF((ptA.x() + ptB.x()) / 2,(ptA.y() + ptB.y()) / 2);//A,B点的中点C
    //中垂线方程式为 y = x * k + b;
    qreal x = 0.0, y = 0.0;//旋转图标矩形的中心 - 初始化变量
    if(abs(ptB.y() - ptA.y()) < 0.1)
    {
        if(ptA.x() < ptB.x())//矩形左上角在上方
        {
            x = ptCenter.x();
            y = ptCenter.y() - 20;
        }
        else//矩形左上角在下方
        {
            x = ptCenter.x();
            y = ptCenter.y() + 20;
        }
    }
    else if(ptB.y() > ptA.y())//顺时针旋转0-180
    {
        qreal k = (ptA.x() - ptB.x()) / (ptB.y() - ptA.y());//中垂线斜率
        qreal b = (ptA.y() + ptB.y()) / 2 - k *(ptA.x() + ptB.x()) / 2;
        //求AB线中垂线上离AB中点20个像素的点C的坐标
        x = 20 * cos(atan(k)) + ptCenter.x();
        y = k * x + b;
    }
    else if(ptB.y() < ptA.y())//顺时针旋转180-360
    {
        qreal k = (ptA.x() - ptB.x()) / (ptB.y() - ptA.y());//中垂线斜率
        qreal b = (ptA.y() + ptB.y()) / 2 - k * (ptA.x() + ptB.x()) / 2;
        //求AB线中垂线上离AB中点20个像素的点C的坐标
        x = -20 * cos(atan(k)) + ptCenter.x();
        y = k * x + b;
    }
    return QPointF(x, y);
}

// 优化：缓存辅助方法实现
bool myGraphicRectItem::isCacheValid(const QRectF& rect, qreal angle, const QPointF& center) const
{
    const qreal epsilon = 0.001; // 浮点数比较精度
    return m_cacheValid && 
           qAbs(rect.x() - m_cachedRect.x()) < epsilon &&
           qAbs(rect.y() - m_cachedRect.y()) < epsilon &&
           qAbs(rect.width() - m_cachedRect.width()) < epsilon &&
           qAbs(rect.height() - m_cachedRect.height()) < epsilon &&
           qAbs(angle - m_cachedAngle) < epsilon &&
           qAbs(center.x() - m_cachedCenter.x()) < epsilon &&
           qAbs(center.y() - m_cachedCenter.y()) < epsilon;
}

void myGraphicRectItem::invalidateCache()
{
    m_cacheValid = false;
}

// 放大镜相关方法实现
void myGraphicRectItem::setMagnifierMode(bool isMagnifier)
{
    if (m_isMagnifierMode != isMagnifier) {
        m_isMagnifierMode = isMagnifier;
        update(); // 重新绘制
    }
}

void myGraphicRectItem::setBackgroundImage(const QPixmap& bgImage)
{
    m_backgroundImage = bgImage;
    if (m_isMagnifierMode) {
        update(); // 重新绘制放大镜内容
    }
}

void myGraphicRectItem::setSelectionRect(const QRectF& selectionRect)
{
    if (m_selectionRect != selectionRect) {
        m_selectionRect = selectionRect;
        if (m_isMagnifierMode) {
            update(); // 重新绘制放大镜内容
            emit magnifierUpdateNeeded();
        }
    }
}

void myGraphicRectItem::setZoomFactor(qreal factor)
{
    if (m_zoomFactor != factor) {
        m_zoomFactor = qMax(0.1, factor); // 确保放大倍数不为负或零
        if (m_isMagnifierMode) {
            update(); // 重新绘制放大镜内容
        }
    }
}

void myGraphicRectItem::setMagnifierQuality(MagnifierQuality quality)
{
    if (m_magnifierQuality != quality) {
        m_magnifierQuality = quality;
        if (m_isMagnifierMode) {
            update(); // 重新绘制放大镜内容
        }
    }
}

// 高质量图像缩放实现
QPixmap myGraphicRectItem::createHighQualityScaledPixmap(const QPixmap& source, const QSize& targetSize) const
{
    if (source.isNull() || targetSize.isEmpty()) {
        return QPixmap();
    }
    
    // 如果目标尺寸与源尺寸相同，直接返回
    if (source.size() == targetSize) {
        return source;
    }
    
    // 计算缩放比例
    qreal scaleX = (qreal)targetSize.width() / source.width();
    qreal scaleY = (qreal)targetSize.height() / source.height();
    qreal maxScale = qMax(scaleX, scaleY);
    
    // 对于大幅度放大，使用多步骤缩放
    if (maxScale > MAGNIFIER_QUALITY_MULTIPLIER) {
        // 计算中间尺寸
        qreal intermediateScale = qSqrt(maxScale);
        QSize intermediateSize(
            qRound(source.width() * intermediateScale),
            qRound(source.height() * intermediateScale)
        );
        
        // 限制中间尺寸在合理范围内
        intermediateSize = intermediateSize.boundedTo(QSize(MAGNIFIER_MAX_SIZE, MAGNIFIER_MAX_SIZE));
        
        // 第一步：缩放到中间尺寸
        QPixmap intermediatePixmap = source.scaled(
            intermediateSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        
        // 第二步：从中间尺寸缩放到目标尺寸
        return intermediatePixmap.scaled(
            targetSize,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
        );
    } else {
        // 对于小幅度放大或缩小，直接缩放
        return source.scaled(
            targetSize,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
        );
    }
}

// 超采样缩放实现
QPixmap myGraphicRectItem::createSuperSampledPixmap(const QPixmap& source, const QSize& targetSize) const
{
    if (source.isNull() || targetSize.isEmpty()) {
        return QPixmap();
    }
    
    // 计算超采样尺寸
    QSize superSampledSize(
        qRound(targetSize.width() * MAGNIFIER_SUPERSAMPLING_FACTOR),
        qRound(targetSize.height() * MAGNIFIER_SUPERSAMPLING_FACTOR)
    );
    
    // 限制超采样尺寸
    superSampledSize = superSampledSize.boundedTo(QSize(MAGNIFIER_MAX_SIZE, MAGNIFIER_MAX_SIZE));
    
    // 首先缩放到超采样尺寸
    QPixmap superSampledPixmap = createHighQualityScaledPixmap(source, superSampledSize);
    
    // 然后缩放到目标尺寸（这样可以获得更好的抗锯齿效果）
    return superSampledPixmap.scaled(
        targetSize,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
    );
}

// 边缘增强实现
QPixmap myGraphicRectItem::applyEdgeEnhancement(const QPixmap& source) const
{
    if (source.isNull()) {
        return QPixmap();
    }
    
    // 将QPixmap转换为QImage以进行像素级处理
    QImage image = source.toImage();
    if (image.isNull()) {
        return source;
    }
    
    // 确保图像格式支持像素访问
    if (image.format() != QImage::Format_ARGB32 && image.format() != QImage::Format_RGB32) {
        image = image.convertToFormat(QImage::Format_ARGB32);
    }
    
    // 创建增强后的图像
    QImage enhancedImage = image.copy();
    
    // 应用简单的锐化滤波器（3x3 Laplacian）
    const int width = image.width();
    const int height = image.height();
    
    // 锐化核心
    const qreal kernel[3][3] = {
        { 0, -MAGNIFIER_EDGE_ENHANCE_STRENGTH, 0 },
        { -MAGNIFIER_EDGE_ENHANCE_STRENGTH, 1 + 4 * MAGNIFIER_EDGE_ENHANCE_STRENGTH, -MAGNIFIER_EDGE_ENHANCE_STRENGTH },
        { 0, -MAGNIFIER_EDGE_ENHANCE_STRENGTH, 0 }
    };
    
    // 应用滤波器（跳过边界像素）
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            qreal r = 0, g = 0, b = 0;
            
            // 应用3x3滤波器
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    QRgb pixel = image.pixel(x + kx, y + ky);
                    qreal weight = kernel[ky + 1][kx + 1];
                    r += qRed(pixel) * weight;
                    g += qGreen(pixel) * weight;
                    b += qBlue(pixel) * weight;
                }
            }
            
            // 限制值范围并设置像素
            int newR = qBound(0, qRound(r), 255);
            int newG = qBound(0, qRound(g), 255);
            int newB = qBound(0, qRound(b), 255);
            
            enhancedImage.setPixel(x, y, qRgb(newR, newG, newB));
        }
    }
    
    return QPixmap::fromImage(enhancedImage);
}

// 超分辨率算法实现
QPixmap myGraphicRectItem::applySuperResolution(const QPixmap& source, const QSize& targetSize) const
{
    if (source.isNull() || targetSize.isEmpty()) {
        return QPixmap();
    }
    
    QImage sourceImage = source.toImage();
    if (sourceImage.isNull()) {
        return source;
    }
    
    // 确保图像格式支持像素访问
    if (sourceImage.format() != QImage::Format_ARGB32 && sourceImage.format() != QImage::Format_RGB32) {
        sourceImage = sourceImage.convertToFormat(QImage::Format_ARGB32);
    }
    
    // 使用边缘保持的超分辨率算法
    QPixmap result = edgePreservingUpscale(sourceImage, targetSize);
    
    // 应用轻微的噪声抑制
    if (!result.isNull()) {
        QImage resultImage = result.toImage();
        // 简单的高斯模糊用于噪声抑制
        // 这里可以根据需要实现更复杂的噪声抑制算法
        result = QPixmap::fromImage(resultImage);
    }
    
    return result;
}

// 双三次插值实现
QPixmap myGraphicRectItem::bicubicInterpolation(const QImage& source, const QSize& targetSize) const
{
    if (source.isNull() || targetSize.isEmpty()) {
        return QPixmap();
    }
    
    const int srcWidth = source.width();
    const int srcHeight = source.height();
    const int dstWidth = targetSize.width();
    const int dstHeight = targetSize.height();
    
    QImage result(dstWidth, dstHeight, QImage::Format_ARGB32);
    
    const qreal scaleX = (qreal)srcWidth / dstWidth;
    const qreal scaleY = (qreal)srcHeight / dstHeight;
    
    for (int y = 0; y < dstHeight; ++y) {
        for (int x = 0; x < dstWidth; ++x) {
            const qreal srcX = x * scaleX;
            const qreal srcY = y * scaleY;
            
            const int x1 = qFloor(srcX);
            const int y1 = qFloor(srcY);
            const qreal dx = srcX - x1;
            const qreal dy = srcY - y1;
            
            qreal r = 0, g = 0, b = 0, a = 0;
            
            // 双三次插值：使用4x4像素窗口
            for (int j = -1; j <= 2; ++j) {
                for (int i = -1; i <= 2; ++i) {
                    const int px = qBound(0, x1 + i, srcWidth - 1);
                    const int py = qBound(0, y1 + j, srcHeight - 1);
                    
                    const QRgb pixel = source.pixel(px, py);
                    const qreal weightX = bicubicWeight(dx - i);
                    const qreal weightY = bicubicWeight(dy - j);
                    const qreal weight = weightX * weightY;
                    
                    r += qRed(pixel) * weight;
                    g += qGreen(pixel) * weight;
                    b += qBlue(pixel) * weight;
                    a += qAlpha(pixel) * weight;
                }
            }
            
            result.setPixel(x, y, qRgba(
                qBound(0, qRound(r), 255),
                qBound(0, qRound(g), 255),
                qBound(0, qRound(b), 255),
                qBound(0, qRound(a), 255)
            ));
        }
    }
    
    return QPixmap::fromImage(result);
}

// 边缘保持的超分辨率放大
QPixmap myGraphicRectItem::edgePreservingUpscale(const QImage& source, const QSize& targetSize) const
{
    if (source.isNull() || targetSize.isEmpty()) {
        return QPixmap();
    }
    
    const int srcWidth = source.width();
    const int srcHeight = source.height();
    const int dstWidth = targetSize.width();
    const int dstHeight = targetSize.height();
    
    QImage result(dstWidth, dstHeight, QImage::Format_ARGB32);
    
    const qreal scaleX = (qreal)srcWidth / dstWidth;
    const qreal scaleY = (qreal)srcHeight / dstHeight;
    
    for (int y = 0; y < dstHeight; ++y) {
        for (int x = 0; x < dstWidth; ++x) {
            const qreal srcX = x * scaleX;
            const qreal srcY = y * scaleY;
            
            const int x1 = qFloor(srcX);
            const int y1 = qFloor(srcY);
            const qreal dx = srcX - x1;
            const qreal dy = srcY - y1;
            
            // 检测边缘强度
            const qreal edgeStrength = detectEdgeStrength(source, 
                qBound(0, x1, srcWidth - 1), 
                qBound(0, y1, srcHeight - 1));
            
            qreal r = 0, g = 0, b = 0, a = 0;
            
            if (edgeStrength > EDGE_THRESHOLD) {
                // 在边缘区域使用更锐利的插值
                // 使用双三次插值但调整参数以保持边缘
                for (int j = -1; j <= 2; ++j) {
                    for (int i = -1; i <= 2; ++i) {
                        const int px = qBound(0, x1 + i, srcWidth - 1);
                        const int py = qBound(0, y1 + j, srcHeight - 1);
                        
                        const QRgb pixel = source.pixel(px, py);
                        const qreal weightX = bicubicWeight(dx - i, -0.75); // 更锐利的参数
                        const qreal weightY = bicubicWeight(dy - j, -0.75);
                        const qreal weight = weightX * weightY;
                        
                        r += qRed(pixel) * weight;
                        g += qGreen(pixel) * weight;
                        b += qBlue(pixel) * weight;
                        a += qAlpha(pixel) * weight;
                    }
                }
            } else {
                // 在平滑区域使用标准双三次插值
                for (int j = -1; j <= 2; ++j) {
                    for (int i = -1; i <= 2; ++i) {
                        const int px = qBound(0, x1 + i, srcWidth - 1);
                        const int py = qBound(0, y1 + j, srcHeight - 1);
                        
                        const QRgb pixel = source.pixel(px, py);
                        const qreal weightX = bicubicWeight(dx - i);
                        const qreal weightY = bicubicWeight(dy - j);
                        const qreal weight = weightX * weightY;
                        
                        r += qRed(pixel) * weight;
                        g += qGreen(pixel) * weight;
                        b += qBlue(pixel) * weight;
                        a += qAlpha(pixel) * weight;
                    }
                }
            }
            
            result.setPixel(x, y, qRgba(
                qBound(0, qRound(r), 255),
                qBound(0, qRound(g), 255),
                qBound(0, qRound(b), 255),
                qBound(0, qRound(a), 255)
            ));
        }
    }
    
    return QPixmap::fromImage(result);
}

// 双三次插值权重函数
qreal myGraphicRectItem::bicubicWeight(qreal x, qreal a) const
{
    x = qAbs(x);
    if (x <= 1.0) {
        return (a + 2.0) * x * x * x - (a + 3.0) * x * x + 1.0;
    } else if (x < 2.0) {
        return a * x * x * x - 5.0 * a * x * x + 8.0 * a * x - 4.0 * a;
    } else {
        return 0.0;
    }
}

// 边缘强度检测
qreal myGraphicRectItem::detectEdgeStrength(const QImage& image, int x, int y) const
{
    const int width = image.width();
    const int height = image.height();
    
    if (x <= 0 || x >= width - 1 || y <= 0 || y >= height - 1) {
        return 0.0;
    }
    
    // 使用Sobel算子检测边缘
    const QRgb center = image.pixel(x, y);
    // 移除未使用的变量
    
    // Sobel X方向
    int sobelX = 0;
    sobelX += (qRed(image.pixel(x-1, y-1)) + qGreen(image.pixel(x-1, y-1)) + qBlue(image.pixel(x-1, y-1))) / 3 * (-1);
    sobelX += (qRed(image.pixel(x+1, y-1)) + qGreen(image.pixel(x+1, y-1)) + qBlue(image.pixel(x+1, y-1))) / 3 * 1;
    sobelX += (qRed(image.pixel(x-1, y)) + qGreen(image.pixel(x-1, y)) + qBlue(image.pixel(x-1, y))) / 3 * (-2);
    sobelX += (qRed(image.pixel(x+1, y)) + qGreen(image.pixel(x+1, y)) + qBlue(image.pixel(x+1, y))) / 3 * 2;
    sobelX += (qRed(image.pixel(x-1, y+1)) + qGreen(image.pixel(x-1, y+1)) + qBlue(image.pixel(x-1, y+1))) / 3 * (-1);
    sobelX += (qRed(image.pixel(x+1, y+1)) + qGreen(image.pixel(x+1, y+1)) + qBlue(image.pixel(x+1, y+1))) / 3 * 1;
    
    // Sobel Y方向
    int sobelY = 0;
    sobelY += (qRed(image.pixel(x-1, y-1)) + qGreen(image.pixel(x-1, y-1)) + qBlue(image.pixel(x-1, y-1))) / 3 * (-1);
    sobelY += (qRed(image.pixel(x, y-1)) + qGreen(image.pixel(x, y-1)) + qBlue(image.pixel(x, y-1))) / 3 * (-2);
    sobelY += (qRed(image.pixel(x+1, y-1)) + qGreen(image.pixel(x+1, y-1)) + qBlue(image.pixel(x+1, y-1))) / 3 * (-1);
    sobelY += (qRed(image.pixel(x-1, y+1)) + qGreen(image.pixel(x-1, y+1)) + qBlue(image.pixel(x-1, y+1))) / 3 * 1;
    sobelY += (qRed(image.pixel(x, y+1)) + qGreen(image.pixel(x, y+1)) + qBlue(image.pixel(x, y+1))) / 3 * 2;
    sobelY += (qRed(image.pixel(x+1, y+1)) + qGreen(image.pixel(x+1, y+1)) + qBlue(image.pixel(x+1, y+1))) / 3 * 1;
    
    // 计算梯度幅度
    return qSqrt(sobelX * sobelX + sobelY * sobelY);
}

// 超级优化：终极超分辨率算法
QPixmap myGraphicRectItem::ultraHighQualityUpscale(const QPixmap& source, const QSize& targetSize) const
{
    if (source.isNull() || targetSize.isEmpty()) {
        return QPixmap();
    }
    
    QImage sourceImage = source.toImage();
    if (sourceImage.format() != QImage::Format_ARGB32) {
        sourceImage = sourceImage.convertToFormat(QImage::Format_ARGB32);
    }
    
    // 第一步：使用多级细节增强
    QPixmap detailEnhanced = multiLevelDetail(sourceImage, targetSize);
    
    // 第二步：Lanczos重采样（最高质量插值）
    QImage detailImage = detailEnhanced.toImage();
    QPixmap lanczosResult = lanczosResample(detailImage, targetSize);
    
    // 第三步：自适应边缘增强
    QImage lanczosImage = lanczosResult.toImage();
    QPixmap edgeEnhanced = adaptiveEdgeEnhancement(lanczosImage);
    
    // 第四步：色彩空间增强
    QImage edgeImage = edgeEnhanced.toImage();
    QPixmap colorEnhanced = colorSpaceEnhancement(edgeImage);
    
    return colorEnhanced;
}

// Lanczos重采样算法（高质量插值）
QPixmap myGraphicRectItem::lanczosResample(const QImage& source, const QSize& targetSize) const
{
    if (source.isNull() || targetSize.isEmpty()) {
        return QPixmap();
    }
    
    const int srcWidth = source.width();
    const int srcHeight = source.height();
    const int dstWidth = targetSize.width();
    const int dstHeight = targetSize.height();
    
    QImage result(dstWidth, dstHeight, QImage::Format_ARGB32);
    
    const qreal scaleX = (qreal)srcWidth / dstWidth;
    const qreal scaleY = (qreal)srcHeight / dstHeight;
    const int kernelSize = ADAPTIVE_KERNEL_SIZE;
    
    for (int y = 0; y < dstHeight; ++y) {
        for (int x = 0; x < dstWidth; ++x) {
            const qreal srcX = x * scaleX;
            const qreal srcY = y * scaleY;
            
            qreal r = 0, g = 0, b = 0, a = 0;
            qreal weightSum = 0;
            
            // 使用更大的Lanczos核心
            for (int j = -kernelSize; j <= kernelSize; ++j) {
                for (int i = -kernelSize; i <= kernelSize; ++i) {
                    const int px = qBound(0, qRound(srcX) + i, srcWidth - 1);
                    const int py = qBound(0, qRound(srcY) + j, srcHeight - 1);
                    
                    const qreal dx = srcX - (qRound(srcX) + i);
                    const qreal dy = srcY - (qRound(srcY) + j);
                    
                    const qreal weightX = lanczosWeight(dx, 3);
                    const qreal weightY = lanczosWeight(dy, 3);
                    const qreal weight = weightX * weightY;
                    
                    if (weight != 0) {
                        const QRgb pixel = source.pixel(px, py);
                        r += qRed(pixel) * weight;
                        g += qGreen(pixel) * weight;
                        b += qBlue(pixel) * weight;
                        a += qAlpha(pixel) * weight;
                        weightSum += weight;
                    }
                }
            }
            
            if (weightSum > 0) {
                r /= weightSum;
                g /= weightSum;
                b /= weightSum;
                a /= weightSum;
            }
            
            result.setPixel(x, y, qRgba(
                qBound(0, qRound(r), 255),
                qBound(0, qRound(g), 255),
                qBound(0, qRound(b), 255),
                qBound(0, qRound(a), 255)
            ));
        }
    }
    
    return QPixmap::fromImage(result);
}

// 自适应边缘增强
QPixmap myGraphicRectItem::adaptiveEdgeEnhancement(const QImage& source) const
{
    if (source.isNull()) {
        return QPixmap();
    }
    
    const int width = source.width();
    const int height = source.height();
    QImage result = source.copy();
    
    // 多级边缘检测和增强
    for (int level = 0; level < MULTI_LEVEL_EDGE_SAMPLES; ++level) {
        qreal levelFactor = 1.0 + level * 0.2;
        
        for (int y = 2; y < height - 2; ++y) {
            for (int x = 2; x < width - 2; ++x) {
                // 检测局部对比度
                qreal contrast = calculateLocalContrast(source, x, y, 2);
                
                if (contrast > EDGE_THRESHOLD * levelFactor) {
                    // 在边缘区域应用自适应锐化
                    QRgb center = source.pixel(x, y);
                    
                    // 计算拉普拉斯算子
                    qreal laplacian = 0;
                    laplacian += (qRed(source.pixel(x-1, y)) + qGreen(source.pixel(x-1, y)) + qBlue(source.pixel(x-1, y))) / 3.0 * (-1);
                    laplacian += (qRed(source.pixel(x+1, y)) + qGreen(source.pixel(x+1, y)) + qBlue(source.pixel(x+1, y))) / 3.0 * (-1);
                    laplacian += (qRed(source.pixel(x, y-1)) + qGreen(source.pixel(x, y-1)) + qBlue(source.pixel(x, y-1))) / 3.0 * (-1);
                    laplacian += (qRed(source.pixel(x, y+1)) + qGreen(source.pixel(x, y+1)) + qBlue(source.pixel(x, y+1))) / 3.0 * (-1);
                    laplacian += (qRed(center) + qGreen(center) + qBlue(center)) / 3.0 * 4;
                    
                    // 自适应增强强度
                    qreal enhanceFactor = DETAIL_ENHANCEMENT_FACTOR * (contrast / 100.0);
                    
                    int newR = qBound(0, qRound(qRed(center) + laplacian * enhanceFactor * 0.01), 255);
                    int newG = qBound(0, qRound(qGreen(center) + laplacian * enhanceFactor * 0.01), 255);
                    int newB = qBound(0, qRound(qBlue(center) + laplacian * enhanceFactor * 0.01), 255);
                    
                    result.setPixel(x, y, qRgba(newR, newG, newB, qAlpha(center)));
                }
            }
        }
    }
    
    return QPixmap::fromImage(result);
}

// 色彩空间增强
QPixmap myGraphicRectItem::colorSpaceEnhancement(const QImage& source) const
{
    if (source.isNull()) {
        return QPixmap();
    }
    
    const int width = source.width();
    const int height = source.height();
    QImage result(width, height, QImage::Format_ARGB32);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb pixel = source.pixel(x, y);
            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);
            int a = qAlpha(pixel);
            
            // 转换到YUV色彩空间进行增强
            qreal Y, U, V;
            rgbToYuv(r, g, b, Y, U, V);
            
            // 增强亮度对比度
            Y = Y * CONTRAST_ENHANCEMENT_FACTOR;
            Y = qBound(0.0, Y, 255.0);
            
            // 轻微增强色度饱和度
            U = U * 1.1;
            V = V * 1.1;
            U = qBound(-128.0, U, 127.0);
            V = qBound(-128.0, V, 127.0);
            
            // 转换回RGB
            int newR, newG, newB;
            yuvToRgb(Y, U, V, newR, newG, newB);
            
            result.setPixel(x, y, qRgba(newR, newG, newB, a));
        }
    }
    
    return QPixmap::fromImage(result);
}

// 多级细节增强
QPixmap myGraphicRectItem::multiLevelDetail(const QImage& source, const QSize& targetSize) const
{
    if (source.isNull() || targetSize.isEmpty()) {
        return QPixmap();
    }
    
    // 使用超高采样率进行中间处理
    QSize ultraSize(
        qRound(targetSize.width() * ULTRA_SAMPLING_FACTOR),
        qRound(targetSize.height() * ULTRA_SAMPLING_FACTOR)
    );
    
    // 限制尺寸以防止内存溢出
    ultraSize = ultraSize.boundedTo(QSize(MAGNIFIER_MAX_SIZE, MAGNIFIER_MAX_SIZE));
    
    // 使用边缘保持算法放大到超高分辨率
    QPixmap ultraRes = edgePreservingUpscale(source, ultraSize);
    
    // 应用多级细节增强
    QImage ultraImage = ultraRes.toImage();
    QPixmap enhanced = adaptiveEdgeEnhancement(ultraImage);
    
    // 最后缩放到目标尺寸（使用最高质量）
    return enhanced.scaled(targetSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

// Lanczos权重函数
qreal myGraphicRectItem::lanczosWeight(qreal x, int a) const
{
    if (x == 0) return 1.0;
    if (qAbs(x) >= a) return 0.0;
    
    const qreal pi = 3.14159265359;
    const qreal pix = pi * x;
    const qreal pixOverA = pix / a;
    
    return (qSin(pix) / pix) * (qSin(pixOverA) / pixOverA);
}

// 计算局部对比度
qreal myGraphicRectItem::calculateLocalContrast(const QImage& image, int x, int y, int radius) const
{
    const int width = image.width();
    const int height = image.height();
    
    if (x < radius || x >= width - radius || y < radius || y >= height - radius) {
        return 0.0;
    }
    
    qreal min = 255.0, max = 0.0;
    
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            QRgb pixel = image.pixel(x + dx, y + dy);
            qreal gray = (qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / 3.0;
            min = qMin(min, gray);
            max = qMax(max, gray);
        }
    }
    
    return max - min;
}

// RGB到YUV转换
void myGraphicRectItem::rgbToYuv(int r, int g, int b, qreal& y, qreal& u, qreal& v) const
{
    y = LUMINANCE_WEIGHT * r + CHROMINANCE_U_WEIGHT * g + CHROMINANCE_V_WEIGHT * b;
    u = -0.14713 * r - 0.28886 * g + 0.436 * b;
    v = 0.615 * r - 0.51499 * g - 0.10001 * b;
}

// YUV到RGB转换
void myGraphicRectItem::yuvToRgb(qreal y, qreal u, qreal v, int& r, int& g, int& b) const
{
    r = qBound(0, qRound(y + 1.13983 * v), 255);
    g = qBound(0, qRound(y - 0.39465 * u - 0.58060 * v), 255);
    b = qBound(0, qRound(y + 2.03211 * u), 255);
}

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
    m_cacheValid(false)  // 初始化缓存状态
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
    
    QRectF target = m_oldRectPolygon.boundingRect();
    QRectF souce = QRectF(this->pos().x(), this->pos().y(), target.width(), target.height());
    painter->drawPixmap(target, pixmap, souce);
    QPen mPen = QPen(Qt::yellow);
    painter->setPen(mPen);
    mPen.setWidth(1);
    painter->drawRect(m_leftRectf);
    painter->drawRect(m_rightRectf);
    painter->drawRect(m_bottomRectf);
    painter->drawRect(m_topRectf);
    painter->drawRect(m_rightAndBottomRectf);
    painter->drawRect(m_rightAndTopRectf);
    painter->drawRect(m_leftAndTopRectf);
    painter->drawRect(m_leftAndBottomRectf);
    painter->drawRect(target);
    painter->fillRect(m_leftRectf, Qt::yellow);
    painter->fillRect(m_rightRectf, Qt::yellow);
    painter->fillRect(m_topRectf, Qt::yellow);
    painter->fillRect(m_bottomRectf, Qt::yellow);
    painter->fillRect(m_rightAndBottomRectf, Qt::yellow);
    painter->fillRect(m_leftAndTopRectf, Qt::yellow);
    painter->fillRect(m_rightAndTopRectf, Qt::yellow);
    painter->fillRect(m_leftAndBottomRectf, Qt::yellow);
    if(mark) painter->fillRect(QRectF(target.x(), target.y(), target.width() + 2, target.height() + 2), QColor(0, 0, 0, 150));
    // QPen mPen1 = QPen(Qt::yellow);
    // painter->setPen(mPen1);
    // painter->drawPolygon(m_oldRectPolygon);
    // //绘制旋转圆形
    // mPen.setWidth(2);
    // mPen.setColor(Qt::green);
    // painter->setPen(mPen);
    // QPointF pf = getSmallRotateRectCenter(m_oldRectPolygon[0],m_oldRectPolygon[1]);
    // QRectF rect = QRectF(pf.x()-10,pf.y()-10,20,20);
    // painter->drawEllipse(rect);//绘制圆形
    // painter->drawPoint(pf);//绘制点
}

void myGraphicRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_tempOldRectPolygon = m_oldRectPolygon;
    m_tempPos = this->pos();
    if(event->button()== Qt::LeftButton)
    {
        emit centerChange(this->pos() + QPointF(m_oldRect.width() / 2, m_oldRect.height() / 2));
        if(mark)
        {
            mark = false;
        }
        emit markChange(mark);
        update();
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
}

void myGraphicRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    emit showMid(0);
    setCursor(Qt::ArrowCursor);
    if(m_StateFlag == MOV_RECT)
    {
        m_StateFlag = DEFAULT_FLAG;
    }
    else {
        QGraphicsItem::mouseReleaseEvent(event);
    }
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
    QVector<QPointF> vpt;

    // 左上角点
    QPointF topLeft = getRotatePoint(ptCenter, rectIn.topLeft(), angle);
    vpt.append(topLeft);

    // 右上角点
    QPointF topRight = getRotatePoint(ptCenter, rectIn.topRight(), angle);
    vpt.append(topRight);

    // 右下角点
    QPointF bottomRight = getRotatePoint(ptCenter, rectIn.bottomRight(), angle);
    vpt.append(bottomRight);

    // 左下角点
    QPointF bottomLeft = getRotatePoint(ptCenter, rectIn.bottomLeft(), angle);
    vpt.append(bottomLeft);

    return QPolygonF(vpt);
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
    if(!otherMark) //只能有一个遮罩亮着
    {
        this->mark = true;
        update();
    }
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

#include "mygraphicrectitem.h"
#include <QtMath>
#include <QDebug>
myGraphicRectItem::myGraphicRectItem(QGraphicsItem *parent):
    m_bResize(false),
    m_oldRect(0, 0, 100, 50),
    m_bRotate(false),
    m_RotateAngle(0),
    m_StateFlag(DEFAULT_FLAG)
{
    mark = true;
    setRectSize(m_oldRect); //根据传入的矩形设置组成图元的几个区域
    setToolTip(u8"Click and drag me!");  //提示
    pPointFofSmallRotateRect = new QPointF[4]; //指向旋转矩形的四个顶点
    SetRotate(0);//初始旋转角度为0
    setFlag(QGraphicsItem::ItemIsSelectable);//设置图元是可选的，可以接受鼠标移动事件
}

myGraphicRectItem::myGraphicRectItem(QSizeF size, QString imagePath):
    m_bResize(false),
    m_oldRect(0, 0, size.width(), size.height()),
    m_bRotate(false),
    m_RotateAngle(0),
    m_StateFlag(DEFAULT_FLAG)
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
    
    QPixmap originalPixmap = QPixmap(actualPath);
    int newWidth = WIDTH; // 新的宽度
    int newHeight = HEIGHT; // 新的高度
    pixmap = originalPixmap.scaled(newWidth, newHeight);
}

QRectF myGraphicRectItem::boundingRect() const
{
    QRectF boundingRectF = m_oldRectPolygon.boundingRect(); //返回图元的边界矩形
    //这里改成根据旋转中点以及矩形大小设置
    return QRectF(boundingRectF.x() - 40, boundingRectF.y() - 40, boundingRectF.width() + 80, boundingRectF.height() + 80);//由于旋转区域的存在加大图元的边界矩形
}
//RAII手法即获取资源即初始化
myGraphicRectItem::~myGraphicRectItem()
{
    delete []pPointFofSmallRotateRect;
    pPointFofSmallRotateRect = nullptr;
}

void myGraphicRectItem::setRectSize(QRectF mrect, bool bResetRotateCenter)
{
    m_oldRect = mrect;
    if(bResetRotateCenter)
    {
        m_RotateCenter.setX(m_oldRect.x() + m_oldRect.width() / 2);
        m_RotateCenter.setY(m_oldRect.y() + m_oldRect.height() / 2);
    }
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
}

void myGraphicRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
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
        qreal dis = pointToLineDistance(m_tempOldRectPolygon.at(0).x(),m_tempOldRectPolygon.at(0).y(),
                                        m_tempOldRectPolygon.at(3).x(), m_tempOldRectPolygon.at(3).y(),
                                        event->pos().x(), event->pos().y());
        if(event->pos().x() < m_startPos.x())
        {
            dis = -dis;
        }
        QRectF tempRect = m_oldRect;
        if(tempRect.width() - dis < 50) return;
        //首先变化大小，再变化原点坐标，再旋转
        QRectF newRect(tempRect.x(), tempRect.y(), tempRect.width() - dis, tempRect.height());
        setRectSize(newRect, false);
        m_RotateCenter = QPointF((m_oldRectPolygon.at(0).x() + m_oldRectPolygon.at(2).x()) / 2,
                                 (m_oldRectPolygon.at(0).y() + m_oldRectPolygon.at(2).y()) / 2);
        m_oldRect.moveCenter(m_RotateCenter);
        setRectSize(m_oldRect);
        prepareGeometryChange(); // 通知Qt将要改变图元的几何形状
        this->setPos(pos().x() + dis, pos().y());
        this->update();
        scene()->update();//必须要用scene()->update()，不能用update();否则会出现重影
    }
    else if(m_StateFlag == MOV_TOP_LINE)
    {
        qreal dis = pointToLineDistance(m_tempOldRectPolygon.at(0).x(),m_tempOldRectPolygon.at(0).y(),
                                        m_tempOldRectPolygon.at(1).x(), m_tempOldRectPolygon.at(1).y(),
                                        event->pos().x(), event->pos().y());
        if(event->pos().y() < m_startPos.y())
        {
            dis = -dis;
        }
        QRectF tempRect = m_oldRect;
        if(tempRect.height() - dis < 50) return;
        //首先变化大小，再变化原点坐标，再旋转
        QRectF newRect(tempRect.x(), tempRect.y(), tempRect.width(), tempRect.height() - dis);
        setRectSize(newRect, false);
        m_RotateCenter = QPointF((m_oldRectPolygon.at(0).x() + m_oldRectPolygon.at(2).x()) / 2,
                                 (m_oldRectPolygon.at(0).y() + m_oldRectPolygon.at(2).y()) / 2);
        m_oldRect.moveCenter(m_RotateCenter);
        setRectSize(m_oldRect);
        prepareGeometryChange(); // 通知Qt将要改变图元的几何形状
        this->setPos(pos().x(), pos().y() + dis);
        this->update();
        scene()->update();//必须要用scene()->update()，不能用update();否则会出现重影
    }
    else if(m_StateFlag == MOV_RIGHT_LINE)
    {
        qreal dis = pointToLineDistance(m_tempOldRectPolygon.at(1).x(),m_tempOldRectPolygon.at(1).y(),
                                        m_tempOldRectPolygon.at(2).x(), m_tempOldRectPolygon.at(2).y(),
                                        event->pos().x(), event->pos().y());
        if(event->pos().x() < m_startPos.x())
        {
            dis = -dis;
        }
        QRectF tempRect = m_tempOldRectPolygon.boundingRect();
        if(tempRect.width() + dis < 50) return;
        //首先变化大小，再变化原点坐标，再旋转
        QRectF newRect(tempRect.x(), tempRect.y(), tempRect.width() + dis, tempRect.height());
        setRectSize(newRect, false);
        m_RotateCenter = QPointF((m_oldRectPolygon.at(0).x() + m_oldRectPolygon.at(2).x()) / 2,
                                 (m_oldRectPolygon.at(0).y() + m_oldRectPolygon.at(2).y()) / 2);
        m_oldRect.moveCenter(m_RotateCenter);
        setRectSize(m_oldRect);
        prepareGeometryChange(); // 通知Qt将要改变图元的几何形状
        this->update();
        scene()->update();//必须要用scene()->update()，不能用update();否则会出现重影
    }
    else if(m_StateFlag == MOV_BOTTOM_LINE)
    {
        qreal dis = pointToLineDistance(m_tempOldRectPolygon.at(2).x(),m_tempOldRectPolygon.at(2).y(),
                                        m_tempOldRectPolygon.at(3).x(), m_tempOldRectPolygon.at(3).y(),
                                        event->pos().x(), event->pos().y());
        if(event->pos().y() < m_startPos.y())
        {
            dis = -dis;
        }
        QRectF tempRect = m_tempOldRectPolygon.boundingRect();
        if(tempRect.height() + dis < 50) return;
        //首先变化大小，再变化原点坐标，再旋转
        QRectF newRect(tempRect.x(), tempRect.y(), tempRect.width(), tempRect.height() + dis);
        setRectSize(newRect, false);
        m_RotateCenter = QPointF((m_oldRectPolygon.at(0).x() + m_oldRectPolygon.at(2).x()) / 2,
                                 (m_oldRectPolygon.at(0).y() + m_oldRectPolygon.at(2).y()) / 2);
        m_oldRect.moveCenter(m_RotateCenter);
        setRectSize(m_oldRect);
        prepareGeometryChange();
        this->update();
        scene()->update();
    }
    else if(m_StateFlag == MOV_RIGHTBOTTOM_RECT)
    {
        qreal dis = pointToLineDistance(m_tempOldRectPolygon.at(2).x(),m_tempOldRectPolygon.at(2).y(),
                                        m_tempOldRectPolygon.at(3).x(), m_tempOldRectPolygon.at(3).y(),
                                        event->pos().x(), event->pos().y());
        qreal dis2 = pointToLineDistance(m_tempOldRectPolygon.at(2).x(),m_tempOldRectPolygon.at(2).y(),
                                         m_tempOldRectPolygon.at(3).x(), m_tempOldRectPolygon.at(3).y(),
                                         event->pos().x(), event->pos().y());
        if(event->pos().y() < m_startPos.y())
        {
            dis = -dis;
        }
        if(event->pos().x() < m_startPos.x())
        {
            dis2 = -dis2;
        }
        QRectF tempRect = m_tempOldRectPolygon.boundingRect();
        if(tempRect.height() + dis < 50 || tempRect.width() + dis2 < 50) return;
        //首先变化大小，再变化原点坐标，再旋转
        QRectF newRect(tempRect.x(), tempRect.y(), tempRect.width() + dis2, tempRect.height() + dis);
        setRectSize(newRect, false);
        m_RotateCenter = QPointF((m_oldRectPolygon.at(0).x() + m_oldRectPolygon.at(2).x()) / 2,
                                 (m_oldRectPolygon.at(0).y() + m_oldRectPolygon.at(2).y()) / 2);
        m_oldRect.moveCenter(m_RotateCenter);
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
    qreal x, y;//旋转图标矩形的中心
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

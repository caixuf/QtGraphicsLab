#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QDebug>
#include "mygraphicbackgrounditem.h"
#include "globals.h"
#include "mygraphicrectitem.h"
#include "drawarrow.hpp"
#include "resourcemanager.h"
#include "sharedresourcepool.h"

class GraphicsManager : public QObject {
    Q_OBJECT
public:
    GraphicsManager(QObject *parent = nullptr) : QObject(parent) {
        scene = new QGraphicsScene();
        scene->setBackgroundBrush(Qt::white);
        
        // 创建两个矩形项
        t = new myGraphicRectItem(QSizeF(120, 80), ResourceManager::instance().getImagePath(ResourceManager::DEFAULT_RECT));
        t1 = new myGraphicRectItem(QSizeF(240, 160), ResourceManager::instance().getImagePath(ResourceManager::DEFAULT_RECT));
        
        // 配置矩形角色
        // t 是暗的选择器矩形
        t->setMagnifierMode(false);  // 选择器模式
        t->setPos(50, 50);           // 初始位置
        t->setMark(true);            // 设置为暗的（有遮罩）
        
        // t1 是亮的放大镜矩形  
        t1->setMagnifierMode(true);  // 放大镜模式
        t1->setPos(400, 50);         // 放在右侧
        t1->setZoomFactor(2.0);      // 设置放大倍数
        t1->setMark(false);          // 设置为亮的（无遮罩）
        
        // 创建背景
        t2 = new MyGraphicBackgroundItem(ResourceManager::instance().getImagePath(ResourceManager::BACKGROUND), QSizeF(WIDTH, HEIGHT));
        t2->setSize(QSizeF(WIDTH, HEIGHT));
        
        // 加载背景图像并设置给放大镜
        QPixmap backgroundImage(ResourceManager::instance().getImagePath(ResourceManager::BACKGROUND));
        if (!backgroundImage.isNull()) {
            // 缩放背景图像以适应场景大小
            backgroundImage = backgroundImage.scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            t1->setBackgroundImage(backgroundImage);
        }
        
        // 箭头和辅助线
        t3 = new ArrowItem();
        QRectF rect1 = this->t->getOldRect();
        QRectF rect2 = this->t1->getOldRect();
        QPointF cen1 = t->pos() + QPointF(rect1.width() / 2, rect1.height() / 2);
        QPointF cen2 = t1->pos() + QPointF(rect2.width() / 2, rect2.height() / 2);
        t3->setStartPoint(cen1);
        t3->setEndPoint(cen2);
        t4 = new QGraphicsRectItem();
        t5 = new QGraphicsRectItem();
        t4->setRect(QRectF(WIDTH / 2, 0, 5, HEIGHT));
        t4->setBrush(Qt::yellow);
        t5->setRect(QRectF(0, HEIGHT / 2, WIDTH, 5));
        t5->setBrush(Qt::yellow);
        
        // 添加项目到场景
        scene->addItem(t2);
        scene->addItem(t1);
        scene->addItem(t);
        scene->addItem(t3);
        scene->addItem(t4);
        scene->addItem(t5);
        t4->setVisible(false);
        t5->setVisible(false);
        scene->setSceneRect(0, 0, WIDTH, HEIGHT);
        view = new QGraphicsView();
        view->setScene(scene);
        view->resize(scene->width() + 3, scene->height() + 3);
        view->setSceneRect(scene->sceneRect());
        
        // 连接信号 - 让GraphicsManager来处理明暗切换逻辑
        connect(t, &myGraphicRectItem::centerChange, this, &GraphicsManager::onCenterChange);
        connect(t1, &myGraphicRectItem::centerChange, this, &GraphicsManager::onCenterChange);
        connect(t, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        connect(t1, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        connect(t, &myGraphicRectItem::showMid, this, &GraphicsManager::showMid);
        connect(t1, &myGraphicRectItem::showMid, this, &GraphicsManager::showMid);
        
        // 连接放大镜信号：当选择器位置变化时，更新放大镜显示的区域
        connect(t, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
        
        // 初始化放大镜显示区域
        updateMagnifierSelection();
        
        // 输出共享资源池的统计信息
        qDebug() << "GraphicsManager initialized. SharedResourcePool cache size:" 
                 << SharedResourcePool::instance().getCacheSize();
    }

    QGraphicsView *getView() const { return view; }
    QGraphicsScene *getScene() const { return scene; }

public slots:
    void onCenterChange(QPointF point)
    {
        QRectF rect1 = this->t->getOldRect();
        QRectF rect2 = this->t1->getOldRect();
        QPointF cen1 = t->pos() + QPointF(rect1.width() / 2, rect1.height() / 2);
        QPointF cen2 = t1->pos() + QPointF(rect2.width() / 2, rect2.height() / 2);
        double eps = 1e-9;
        if(fabs(cen1.x() - point.x()) < eps && fabs(cen1.y() - point.y()) < eps)
        {
            t3->setOldRectf(t->getOldRect());
            t3->setEndPoint(cen1);
            t3->setStartPoint(cen2);
        }
        else
        {
            t3->setOldRectf(t1->getOldRect());
            t3->setEndPoint(cen2);
            t3->setStartPoint(cen1);
        }
    }

    void showMid(int sign)
    {
        if(sign == 0)
        {
            t4->setVisible(false);
            t5->setVisible(false);
        }
        if(sign == 1)
        {
            t4->setVisible(true);
            t5->setVisible(false);
        }
        if(sign == 2)
        {
            t4->setVisible(false);
            t5->setVisible(true);
        }
    }

    // 新增：处理选择区域变化的槽函数
    void onSelectionRectChanged(const QRectF& selectionRect)
    {
        Q_UNUSED(selectionRect);
        updateMagnifierSelection();
    }

    // 新增：处理mark状态变化的槽函数 - 确保两个矩形的明暗状态互斥，并切换放大镜功能
    void onMarkChange(bool mark)
    {
        // 找出是哪个矩形发出的信号
        myGraphicRectItem* sender = qobject_cast<myGraphicRectItem*>(QObject::sender());
        if (!sender) return;
        
        // 临时断开markChange信号连接，避免递归调用
        disconnect(t, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        disconnect(t1, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        
        // 加载背景图像（如果还没有加载）
        QPixmap backgroundImage(ResourceManager::instance().getImagePath(ResourceManager::BACKGROUND));
        if (!backgroundImage.isNull()) {
            backgroundImage = backgroundImage.scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        
        // 确保两个矩形的状态是互斥的：一个亮(mark=false)，一个暗(mark=true)
        // 同时切换放大镜功能到亮的矩形上，并调整比例
        if (sender == t) {
            // t 被点击了，让 t 变亮并成为放大镜，t1 变暗并成为选择器
            
            // 获取当前两个矩形的大小
            QRectF tRect = t->getOldRect();
            QRectF t1Rect = t1->getOldRect();
            
            // 计算需要的比例调整：让 t（新放大镜）的大小基于 t1（新选择器）的比例
            // 保持 t 的位置不变，只调整大小
            qreal aspectRatio = t1Rect.width() / t1Rect.height();
            qreal newWidth = tRect.height() * aspectRatio;  // 基于 t 的高度和 t1 的宽高比
            
            // 更新 t 的大小（保持中心位置）
            QPointF tCenter = t->pos() + QPointF(tRect.width() / 2, tRect.height() / 2);
            QRectF newTRect(0, 0, newWidth, tRect.height());
            t->setRectSize(newTRect);
            t->setPos(tCenter - QPointF(newWidth / 2, tRect.height() / 2));
            
            t->setMark(false);              // t 变亮（无遮罩）
            t->setMagnifierMode(true);      // t 成为放大镜
            t->setBackgroundImage(backgroundImage); // 设置背景图像
            
            t1->setMark(true);              // t1 变暗（有遮罩）
            t1->setMagnifierMode(false);    // t1 成为选择器
            t1->setBackgroundImage(QPixmap()); // 清除背景图像
            
            // 断开旧的选择区域变化信号连接，连接新的
            disconnect(t, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
            disconnect(t1, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
            connect(t1, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
            
        } else if (sender == t1) {
            // t1 被点击了，让 t1 变亮并成为放大镜，t 变暗并成为选择器
            
            // 获取当前两个矩形的大小
            QRectF tRect = t->getOldRect();
            QRectF t1Rect = t1->getOldRect();
            
            // 计算需要的比例调整：让 t1（新放大镜）的大小基于 t（新选择器）的比例
            // 保持 t1 的位置不变，只调整大小
            qreal aspectRatio = tRect.width() / tRect.height();
            qreal newWidth = t1Rect.height() * aspectRatio;  // 基于 t1 的高度和 t 的宽高比
            
            // 更新 t1 的大小（保持中心位置）
            QPointF t1Center = t1->pos() + QPointF(t1Rect.width() / 2, t1Rect.height() / 2);
            QRectF newT1Rect(0, 0, newWidth, t1Rect.height());
            t1->setRectSize(newT1Rect);
            t1->setPos(t1Center - QPointF(newWidth / 2, t1Rect.height() / 2));
            
            t1->setMark(false);             // t1 变亮（无遮罩）
            t1->setMagnifierMode(true);     // t1 成为放大镜
            t1->setBackgroundImage(backgroundImage); // 设置背景图像
            
            t->setMark(true);               // t 变暗（有遮罩）
            t->setMagnifierMode(false);     // t 成为选择器
            t->setBackgroundImage(QPixmap()); // 清除背景图像
            
            // 断开旧的选择区域变化信号连接，连接新的
            disconnect(t, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
            disconnect(t1, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
            connect(t, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
        }
        
        // 重新连接mark change信号
        connect(t, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        connect(t1, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        
        // 更新放大镜显示的区域
        updateMagnifierSelection();
        
        qDebug() << "Mark and magnifier role changed: t.mark=" << t->getMark() << ", t1.mark=" << t1->getMark()
                 << ", t.isMagnifier=" << t->isMagnifierMode() << ", t1.isMagnifier=" << t1->isMagnifierMode();
    }

private:
    // 更新放大镜显示的选择区域
    void updateMagnifierSelection()
    {
        if (t && t1) {
            // 动态确定哪个是选择器（暗的），哪个是放大镜（亮的）
            myGraphicRectItem* selector = nullptr;
            myGraphicRectItem* magnifier = nullptr;
            
            if (t->getMark() && !t1->getMark()) {
                // t 是暗的选择器，t1 是亮的放大镜
                selector = t;
                magnifier = t1;
            } else if (!t->getMark() && t1->getMark()) {
                // t 是亮的放大镜，t1 是暗的选择器
                selector = t1;
                magnifier = t;
            }
            
            if (selector && magnifier) {
                // 获取选择器的位置和大小
                QRectF selectorRect = selector->getOldRect();
                
                // 将选择器的位置映射到背景图像的坐标系
                // 这里假设背景图像填充整个场景
                magnifier->setSelectionRect(selectorRect);
            }
        }
    }

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    myGraphicRectItem * t;
    myGraphicRectItem * t1;
    MyGraphicBackgroundItem * t2;
    ArrowItem * t3;
    QGraphicsRectItem * t4;
    QGraphicsRectItem * t5;
};

#endif // GRAPHICSMANAGER_H

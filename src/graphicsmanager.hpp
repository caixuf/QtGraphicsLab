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
        selectorRect = new myGraphicRectItem(QSizeF(120, 80), ResourceManager::instance().getImagePath(ResourceManager::DEFAULT_RECT));
        magnifierRect = new myGraphicRectItem(QSizeF(240, 160), ResourceManager::instance().getImagePath(ResourceManager::DEFAULT_RECT));
        
        // 配置矩形角色
        // selectorRect 是暗的选择器矩形
        selectorRect->setMagnifierMode(false);  // 选择器模式
        selectorRect->setPos(50, 50);           // 初始位置
        selectorRect->setMark(true);            // 设置为暗的（有遮罩）
        
        // magnifierRect 是亮的放大镜矩形  
        magnifierRect->setMagnifierMode(true);  // 放大镜模式
        magnifierRect->setPos(400, 50);         // 放在右侧
        magnifierRect->setZoomFactor(2.0);      // 设置放大倍数
        magnifierRect->setMark(false);          // 设置为亮的（无遮罩）
        
        // 创建背景
        backgroundItem = new MyGraphicBackgroundItem(ResourceManager::instance().getImagePath(ResourceManager::BACKGROUND), QSizeF(WIDTH, HEIGHT));
        backgroundItem->setSize(QSizeF(WIDTH, HEIGHT));
        
        // 加载背景图像并设置给放大镜
        QPixmap backgroundImage(ResourceManager::instance().getImagePath(ResourceManager::BACKGROUND));
        if (!backgroundImage.isNull()) {
            // 缩放背景图像以适应场景大小
            backgroundImage = backgroundImage.scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            magnifierRect->setBackgroundImage(backgroundImage);
        }
        
        // 箭头和辅助线
        connectionArrow = new ArrowItem();
        QRectF rect1 = this->selectorRect->getOldRect();
        QRectF rect2 = this->magnifierRect->getOldRect();
        QPointF cen1 = selectorRect->pos() + QPointF(rect1.width() / 2, rect1.height() / 2);
        QPointF cen2 = magnifierRect->pos() + QPointF(rect2.width() / 2, rect2.height() / 2);
        connectionArrow->setStartPoint(cen1);
        connectionArrow->setEndPoint(cen2);
        verticalGuideLine = new QGraphicsRectItem();
        horizontalGuideLine = new QGraphicsRectItem();
        verticalGuideLine->setRect(QRectF(WIDTH / 2, 0, 5, HEIGHT));
        verticalGuideLine->setBrush(Qt::yellow);
        horizontalGuideLine->setRect(QRectF(0, HEIGHT / 2, WIDTH, 5));
        horizontalGuideLine->setBrush(Qt::yellow);
        
        // 添加项目到场景
        scene->addItem(backgroundItem);
        scene->addItem(magnifierRect);
        scene->addItem(selectorRect);
        scene->addItem(connectionArrow);
        scene->addItem(verticalGuideLine);
        scene->addItem(horizontalGuideLine);
        verticalGuideLine->setVisible(false);
        horizontalGuideLine->setVisible(false);
        scene->setSceneRect(0, 0, WIDTH, HEIGHT);
        view = new QGraphicsView();  // 不设置父窗口，由Widget来设置
        view->setScene(scene);
        view->resize(scene->width() + 3, scene->height() + 3);
        view->setSceneRect(scene->sceneRect());
        
        // 连接信号 - 让GraphicsManager来处理明暗切换逻辑
        connect(selectorRect, &myGraphicRectItem::centerChange, this, &GraphicsManager::onCenterChange);
        connect(magnifierRect, &myGraphicRectItem::centerChange, this, &GraphicsManager::onCenterChange);
        connect(selectorRect, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        connect(magnifierRect, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        connect(selectorRect, &myGraphicRectItem::showMid, this, &GraphicsManager::showMid);
        connect(magnifierRect, &myGraphicRectItem::showMid, this, &GraphicsManager::showMid);
        
        // 连接放大镜信号：当选择器位置变化时，更新放大镜显示的区域
        connect(selectorRect, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
        
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
        QRectF rect1 = this->selectorRect->getOldRect();
        QRectF rect2 = this->magnifierRect->getOldRect();
        QPointF cen1 = selectorRect->pos() + QPointF(rect1.width() / 2, rect1.height() / 2);
        QPointF cen2 = magnifierRect->pos() + QPointF(rect2.width() / 2, rect2.height() / 2);
        double eps = 1e-9;
        if(fabs(cen1.x() - point.x()) < eps && fabs(cen1.y() - point.y()) < eps)
        {
            connectionArrow->setOldRectf(selectorRect->getOldRect());
            connectionArrow->setEndPoint(cen1);
            connectionArrow->setStartPoint(cen2);
        }
        else
        {
            connectionArrow->setOldRectf(magnifierRect->getOldRect());
            connectionArrow->setEndPoint(cen2);
            connectionArrow->setStartPoint(cen1);
        }
    }

    void showMid(int sign)
    {
        if(sign == 0)
        {
            verticalGuideLine->setVisible(false);
            horizontalGuideLine->setVisible(false);
        }
        if(sign == 1)
        {
            verticalGuideLine->setVisible(true);
            horizontalGuideLine->setVisible(false);
        }
        if(sign == 2)
        {
            verticalGuideLine->setVisible(false);
            horizontalGuideLine->setVisible(true);
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
        Q_UNUSED(mark);  // 标记参数已使用，避免警告
        
        // 找出是哪个矩形发出的信号
        myGraphicRectItem* sender = qobject_cast<myGraphicRectItem*>(QObject::sender());
        if (!sender) return;
        
        // 临时断开markChange信号连接，避免递归调用
        disconnect(selectorRect, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        disconnect(magnifierRect, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        
        // 加载背景图像（如果还没有加载）
        QPixmap backgroundImage(ResourceManager::instance().getImagePath(ResourceManager::BACKGROUND));
        if (!backgroundImage.isNull()) {
            backgroundImage = backgroundImage.scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        
        // 确保两个矩形的状态是互斥的：一个亮(mark=false)，一个暗(mark=true)
        // 同时切换放大镜功能到亮的矩形上，并调整比例
        if (sender == selectorRect) {
            // selectorRect 被点击了，让 selectorRect 变亮并成为放大镜，magnifierRect 变暗并成为选择器
            
            // 获取当前两个矩形的大小
            QRectF selectorRectSize = selectorRect->getOldRect();
            QRectF magnifierRectSize = magnifierRect->getOldRect();
            
            // 计算需要的比例调整：让 selectorRect（新放大镜）的大小基于 magnifierRect（新选择器）的比例
            // 保持 selectorRect 的位置不变，只调整大小
            qreal aspectRatio = magnifierRectSize.width() / magnifierRectSize.height();
            qreal newWidth = selectorRectSize.height() * aspectRatio;  // 基于 selectorRect 的高度和 magnifierRect 的宽高比
            
            // 更新 selectorRect 的大小（保持中心位置）
            QPointF selectorCenter = selectorRect->pos() + QPointF(selectorRectSize.width() / 2, selectorRectSize.height() / 2);
            QRectF newSelectorRect(0, 0, newWidth, selectorRectSize.height());
            selectorRect->setRectSize(newSelectorRect);
            selectorRect->setPos(selectorCenter - QPointF(newWidth / 2, selectorRectSize.height() / 2));
            
            selectorRect->setMark(false);              // selectorRect 变亮（无遮罩）
            selectorRect->setMagnifierMode(true);      // selectorRect 成为放大镜
            selectorRect->setBackgroundImage(backgroundImage); // 设置背景图像
            
            magnifierRect->setMark(true);              // magnifierRect 变暗（有遮罩）
            magnifierRect->setMagnifierMode(false);    // magnifierRect 成为选择器
            magnifierRect->setBackgroundImage(QPixmap()); // 清除背景图像
            
            // 断开旧的选择区域变化信号连接，连接新的
            disconnect(selectorRect, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
            disconnect(magnifierRect, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
            connect(magnifierRect, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
            
        } else if (sender == magnifierRect) {
            // magnifierRect 被点击了，让 magnifierRect 变亮并成为放大镜，selectorRect 变暗并成为选择器
            
            // 获取当前两个矩形的大小
            QRectF selectorRectSize = selectorRect->getOldRect();
            QRectF magnifierRectSize = magnifierRect->getOldRect();
            
            // 计算需要的比例调整：让 magnifierRect（新放大镜）的大小基于 selectorRect（新选择器）的比例
            // 保持 magnifierRect 的位置不变，只调整大小
            qreal aspectRatio = selectorRectSize.width() / selectorRectSize.height();
            qreal newWidth = magnifierRectSize.height() * aspectRatio;  // 基于 magnifierRect 的高度和 selectorRect 的宽高比
            
            // 更新 magnifierRect 的大小（保持中心位置）
            QPointF magnifierCenter = magnifierRect->pos() + QPointF(magnifierRectSize.width() / 2, magnifierRectSize.height() / 2);
            QRectF newMagnifierRect(0, 0, newWidth, magnifierRectSize.height());
            magnifierRect->setRectSize(newMagnifierRect);
            magnifierRect->setPos(magnifierCenter - QPointF(newWidth / 2, magnifierRectSize.height() / 2));
            
            magnifierRect->setMark(false);             // magnifierRect 变亮（无遮罩）
            magnifierRect->setMagnifierMode(true);     // magnifierRect 成为放大镜
            magnifierRect->setBackgroundImage(backgroundImage); // 设置背景图像
            
            selectorRect->setMark(true);               // selectorRect 变暗（有遮罩）
            selectorRect->setMagnifierMode(false);     // selectorRect 成为选择器
            selectorRect->setBackgroundImage(QPixmap()); // 清除背景图像
            
            // 断开旧的选择区域变化信号连接，连接新的
            disconnect(selectorRect, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
            disconnect(magnifierRect, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
            connect(selectorRect, &myGraphicRectItem::selectionRectChanged, this, &GraphicsManager::onSelectionRectChanged);
        }
        
        // 重新连接mark change信号
        connect(selectorRect, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        connect(magnifierRect, &myGraphicRectItem::markChange, this, &GraphicsManager::onMarkChange);
        
        // 更新放大镜显示的区域
        updateMagnifierSelection();
        
        qDebug() << "Mark and magnifier role changed: selector.mark=" << selectorRect->getMark() << ", magnifier.mark=" << magnifierRect->getMark()
                 << ", selector.isMagnifier=" << selectorRect->isMagnifierMode() << ", magnifier.isMagnifier=" << magnifierRect->isMagnifierMode();
    }

private:
    // 更新放大镜显示的选择区域
    void updateMagnifierSelection()
    {
        if (selectorRect && magnifierRect) {
            // 动态确定哪个是选择器（暗的），哪个是放大镜（亮的）
            myGraphicRectItem* selector = nullptr;
            myGraphicRectItem* magnifier = nullptr;
            
            if (selectorRect->getMark() && !magnifierRect->getMark()) {
                // selectorRect 是暗的选择器，magnifierRect 是亮的放大镜
                selector = selectorRect;
                magnifier = magnifierRect;
            } else if (!selectorRect->getMark() && magnifierRect->getMark()) {
                // selectorRect 是亮的放大镜，magnifierRect 是暗的选择器
                selector = magnifierRect;
                magnifier = selectorRect;
            }
            
            if (selector && magnifier) {
                // 获取选择器在场景中的位置和大小
                QRectF selectorSceneRect = selector->getOldRect();
                
                // 将选择器的场景坐标转换为背景图像中的相对坐标
                // 背景图像坐标系是 (0,0) 到 (WIDTH, HEIGHT)
                // 确保选择区域在背景图像范围内
                qreal x = qMax(0.0, qMin(selectorSceneRect.x(), (qreal)WIDTH - selectorSceneRect.width()));
                qreal y = qMax(0.0, qMin(selectorSceneRect.y(), (qreal)HEIGHT - selectorSceneRect.height()));
                qreal w = qMin(selectorSceneRect.width(), (qreal)WIDTH - x);
                qreal h = qMin(selectorSceneRect.height(), (qreal)HEIGHT - y);
                
                QRectF backgroundImageRect(x, y, w, h);
                magnifier->setSelectionRect(backgroundImageRect);
            }
        }
    }

public:
    // 公开成员变量以便外部访问
    myGraphicRectItem *selectorRect;        // 选择器矩形
    myGraphicRectItem *magnifierRect;       // 放大镜矩形
    MyGraphicBackgroundItem *backgroundItem; // 背景图像项

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    ArrowItem *connectionArrow;             // 连接箭头
    QGraphicsRectItem *verticalGuideLine;   // 垂直辅助线
    QGraphicsRectItem *horizontalGuideLine; // 水平辅助线
};

#endif // GRAPHICSMANAGER_H

#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include "mygraphicbackgrounditem.h"
#include "globals.h"
#include "mygraphicrectitem.h"
#include "drawarrow.hpp"

class GraphicsManager : public QObject {
    Q_OBJECT
public:
    GraphicsManager(QObject *parent = nullptr) : QObject(parent) {
        scene = new QGraphicsScene();
        scene->setBackgroundBrush(Qt::white);
        t = new myGraphicRectItem(QSizeF(200, 140), u8":/image/a1.png");
        t1 = new myGraphicRectItem(QSizeF(200, 140), u8":/image/a1.png");
        t2 = new MyGraphicBackgroundItem(u8":/image/a1.png", QSizeF(WIDTH, HEIGHT));
        t2->setSize(QSizeF(WIDTH, HEIGHT));
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
        connect(t, &myGraphicRectItem::centerChange, this, &GraphicsManager::onCenterChange);
        connect(t1, &myGraphicRectItem::centerChange, this, &GraphicsManager::onCenterChange);
        connect(t, &myGraphicRectItem::markChange, t1, &myGraphicRectItem::setMark);
        connect(t1, &myGraphicRectItem::markChange, t, &myGraphicRectItem::setMark);
        connect(t, &myGraphicRectItem::showMid, this, &GraphicsManager::showMid);
        connect(t1, &myGraphicRectItem::showMid, this, &GraphicsManager::showMid);
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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include "graphicsmanager.hpp"

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    GraphicsManager *m_graphicsManager;
    QVBoxLayout *m_mainLayout;
};

#endif // MAINWIDGET_H

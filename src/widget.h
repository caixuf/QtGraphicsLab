#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include "graphicsmanager.hpp"

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    GraphicsManager *m_graphicsManager;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_controlLayout;
    QGroupBox *m_qualityGroup;
    QLabel *m_qualityLabel;
    QComboBox *m_qualityCombo;
    QLabel *m_maskLabel;
    QComboBox *m_maskCombo;
    
private slots:
    void onQualityChanged(int index);
    void onMaskChanged(int index);
    void setupQualityControls();
};

#endif // MAINWIDGET_H

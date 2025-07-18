#include "widget.h"
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include "globals.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    // 创建 GraphicsManager
    m_graphicsManager = new GraphicsManager(this);
    
    // 创建布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);
    m_mainLayout->setSpacing(5);
    
    // 设置质量控制界面
    setupQualityControls();
    
    // 将 GraphicsView 添加到布局中
    m_mainLayout->addWidget(m_graphicsManager->getView());
    
    // 配置 GraphicsView 的缩放和显示策略
    QGraphicsView* view = m_graphicsManager->getView();
    view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    view->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // 设置窗口标题
    setWindowTitle("Qt Graphics Lab - Magnifier Demo with Quality Control");
    
    // 设置窗口尺寸直接使用globals.h中定义的尺寸
    setMinimumSize(400, 300);  // 设置合理的最小尺寸
    resize(WIDTH + 20, HEIGHT + 100);     // 为控制界面增加空间
    
    // 初始化时使用fitInView自适应显示，这样放大镜功能可以正常工作
    QTimer::singleShot(0, this, [this]() {
        if (m_graphicsManager && m_graphicsManager->getView() && m_graphicsManager->getScene()) {
            QGraphicsView* view = m_graphicsManager->getView();
            QGraphicsScene* scene = m_graphicsManager->getScene();
            
            // 确保场景矩形正确设置
            scene->setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
            
            // 使用IgnoreAspectRatio填充整个视图区域
            view->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
            
            // 强制刷新视图
            view->viewport()->update();
        }
    });
}

MainWidget::~MainWidget()
{
    // m_graphicsManager 会在父对象销毁时自动销毁
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    // 当窗口大小变化时，让场景内容填充整个窗口
    if (m_graphicsManager && m_graphicsManager->getView() && m_graphicsManager->getScene()) {
        QGraphicsView* view = m_graphicsManager->getView();
        QGraphicsScene* scene = m_graphicsManager->getScene();
        
        // 使用 fitInView 让场景完全填充视图
        view->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
        
        // 或者如果你想保持宽高比但填充尽可能多的空间：
        // view->fitInView(scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
    }
}

void MainWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    
    // 确保在窗口显示时正确填充背景
    if (m_graphicsManager && m_graphicsManager->getView() && m_graphicsManager->getScene()) {
        QGraphicsView* view = m_graphicsManager->getView();
        QGraphicsScene* scene = m_graphicsManager->getScene();
        
        // 确保场景矩形正确设置
        scene->setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
        
        // 使用 fitInView 让场景完全填充视图
        view->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
        
        // 强制刷新视图
        view->viewport()->update();
    }
}

void MainWidget::setupQualityControls()
{
    // 创建质量控制分组
    m_qualityGroup = new QGroupBox("放大镜和显示设置", this);
    m_controlLayout = new QHBoxLayout(m_qualityGroup);
    
    // 创建质量标签和下拉框
    m_qualityLabel = new QLabel("质量级别:", m_qualityGroup);
    m_qualityCombo = new QComboBox(m_qualityGroup);
    m_qualityCombo->addItem("快速模式 (Fast)", static_cast<int>(QUALITY_FAST));
    m_qualityCombo->addItem("平衡模式 (Balanced)", static_cast<int>(QUALITY_BALANCED));
    m_qualityCombo->addItem("高质量模式 (High Quality)", static_cast<int>(QUALITY_HIGH));
    m_qualityCombo->addItem("超高质量模式 (Ultra Quality)", static_cast<int>(QUALITY_ULTRA));
    // 默认选择快速模式
    m_qualityCombo->setCurrentIndex(0);
    
    // 创建遮罩标签和下拉框
    m_maskLabel = new QLabel("背景遮罩:", m_qualityGroup);
    m_maskCombo = new QComboBox(m_qualityGroup);
    m_maskCombo->addItem("无遮罩", 0);
    m_maskCombo->addItem("轻微遮罩", 20);
    m_maskCombo->addItem("中等遮罩", 40);
    m_maskCombo->addItem("重度遮罩", 60);
    // 默认选择中等遮罩
    m_maskCombo->setCurrentIndex(2);
    
    // 添加到布局
    m_controlLayout->addWidget(m_qualityLabel);
    m_controlLayout->addWidget(m_qualityCombo);
    m_controlLayout->addWidget(m_maskLabel);
    m_controlLayout->addWidget(m_maskCombo);
    m_controlLayout->addStretch(); // 添加弹性空间
    
    // 连接信号
    connect(m_qualityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWidget::onQualityChanged);
    connect(m_maskCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWidget::onMaskChanged);
    
    // 添加到主布局
    m_mainLayout->addWidget(m_qualityGroup);
}

void MainWidget::onQualityChanged(int index)
{
    if (m_graphicsManager) {
        MagnifierQuality quality = static_cast<MagnifierQuality>(m_qualityCombo->itemData(index).toInt());
        
        // 设置两个矩形的质量
        m_graphicsManager->selectorRect->setMagnifierQuality(quality);
        m_graphicsManager->magnifierRect->setMagnifierQuality(quality);
        
        // 显示质量信息
        QString qualityText;
        switch (quality) {
        case QUALITY_FAST:
            qualityText = "快速模式：使用Qt内置缩放，性能最佳";
            break;
        case QUALITY_BALANCED:
            qualityText = "平衡模式：中等质量，较好性能";
            break;
        case QUALITY_HIGH:
            qualityText = "高质量模式：使用超分辨率算法，性能中等";
            break;
        case QUALITY_ULTRA:
            qualityText = "超高质量模式：使用所有优化算法，性能最慢";
            break;
        }
        
        // 更新状态栏或工具提示
        m_qualityCombo->setToolTip(qualityText);
    }
}

void MainWidget::onMaskChanged(int index)
{
    if (m_graphicsManager) {
        int maskPercent = m_maskCombo->itemData(index).toInt();
        qreal maskOpacity = maskPercent / 100.0;
        
        // 设置背景遮罩透明度
        if (m_graphicsManager->backgroundItem) {
            m_graphicsManager->backgroundItem->setMaskOpacity(maskOpacity);
        }
        
        // 显示遮罩信息
        QString maskText = QString("背景遮罩透明度: %1%").arg(maskPercent);
        m_maskCombo->setToolTip(maskText);
    }
}

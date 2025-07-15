# MaskViewer - 智能遮罩图像查看器

这是一个基于 Qt Quick 2.15 开发的创新图像查看器，通过智能遮罩系统和可拖拽透明窗口，为用户提供独特的图像浏览体验。

## 项目概述

本项目展示了一个创新的图像查看方式：
- 默认状态下，图像被半透明的黑色遮罩覆盖
- 用户可以通过两个可拖拽的矩形"窗口"来查看图像的特定区域
- 窗口之间具有互斥机制，确保同一时间只能打开一个窗口
- 内置智能对齐系统，提供流畅的拖拽体验

## 核心功能

### 1. 遮罩系统 (Mask System)

#### 设计理念
- **默认遮罩模式**：图像初始被半透明黑色遮罩覆盖（opacity: 0.5）
- **选择性显示**：通过窗口机制显示图像的特定区域
- **动态切换**：根据用户交互动态控制遮罩的显示状态

#### 技术实现
```qml
// 遮罩控制属性
property bool defaultBackgroudState1: false  // 左侧窗口遮罩状态
property bool defaultBackgroudState2: true   // 右侧窗口遮罩状态
property double defaultBackgroudOpacity: 0.5 // 遮罩透明度
property color defaultBackgroudColor1: "black" // 遮罩颜色
```

遮罩系统由5个Rectangle组件构成，围绕目标矩形形成完整覆盖：
- 顶部遮罩：覆盖矩形上方区域
- 底部遮罩：覆盖矩形下方区域  
- 左侧遮罩：覆盖矩形左侧区域
- 右侧遮罩：覆盖矩形右侧区域
- 中心底部遮罩：覆盖矩形正下方区域

#### 锚点系统 (Anchors System) - 核心技术

遮罩系统的核心是利用 QML 的锚点机制实现动态布局。每个遮罩矩形通过锚点与目标矩形建立相对位置关系，确保在目标矩形移动或调整大小时，遮罩能够自动重新计算并保持完美覆盖。

**锚点技术实现：**

```qml
// 顶部遮罩 - 锚定到目标矩形上方
Rectangle {
    anchors.bottom: firstAreaRect1.top    // 下边锚定到目标矩形顶部
    anchors.top: parent.top               // 上边锚定到父容器顶部
    anchors.left: parent.left             // 左边锚定到父容器左边
    anchors.right: parent.right           // 右边锚定到父容器右边
}

// 左侧遮罩 - 锚定到目标矩形左侧
Rectangle {
    anchors.top: firstAreaRect2.bottom    // 上边锚定到顶部遮罩下方
    anchors.bottom: parent.bottom         // 下边锚定到父容器底部
    anchors.left: parent.left             // 左边锚定到父容器左边
    anchors.right: firstAreaRect1.left    // 右边锚定到目标矩形左边
}

// 右侧遮罩 - 锚定到目标矩形右侧
Rectangle {
    anchors.top: firstAreaRect2.bottom    // 上边锚定到顶部遮罩下方
    anchors.bottom: parent.bottom         // 下边锚定到父容器底部
    anchors.left: firstAreaRect1.right    // 左边锚定到目标矩形右边
    anchors.right: parent.right           // 右边锚定到父容器右边
}

// 底部中心遮罩 - 锚定到目标矩形下方
Rectangle {
    anchors.top: firstAreaRect1.bottom    // 上边锚定到目标矩形底部
    anchors.bottom: parent.bottom         // 下边锚定到父容器底部
    anchors.left: firstAreaRect3.right    // 左边锚定到左侧遮罩右边
    anchors.right: firstAreaRect4.left    // 右边锚定到右侧遮罩左边
}
```

**锚点系统优势：**

1. **自动布局**：目标矩形移动时，所有遮罩自动跟随调整
2. **响应式设计**：矩形缩放时，遮罩区域自动重新计算
3. **性能优化**：利用 QML 引擎的原生布局系统，避免手动计算
4. **维护简单**：声明式语法，逻辑清晰易懂
5. **精确对齐**：确保遮罩与目标矩形之间无缝隙、无重叠

### 2. 窗口系统 (Window System)

#### 窗口特性
- **半透明矩形**：窗口本身是白色半透明矩形（opacity: 0.4）
- **透视效果**：用户可以透过窗口看到底层图像
- **视觉边框**：黄色角标提供清晰的视觉边界
- **互斥机制**：同一时间只能开启一个窗口

#### 交互逻辑
```qml
onClicked: {
    if(secondArea.viewState) return; // 互斥检查
    
    // 关闭对方窗口
    if(secondArea.viewState === true) {
        secondAreaRect1.color = "white"
        secondArea.viewState = false;
    }
    
    // 切换当前窗口
    if(firstArea.viewState === false) {
        firstAreaRect1.color = "#00000000" // 透明化
        firstArea.viewState = true;
    } else {
        firstAreaRect1.color = "white"     // 恢复
        firstArea.viewState = false;
    }
}
```

### 3. 智能拖拽系统 (Smart Dragging)

#### 碰撞检测
- **边界限制**：矩形无法拖拽出窗口边界
- **位置验证**：实时检查新位置的有效性
```qml
if(newX >= 0 && newX <= root.width - parent.width && 
   newY >= 0 && newY <= root.height - parent.height) {
    parent.x = newX;
    parent.y = newY;
}
```

#### 磁性对齐 (Magnetic Alignment)
- **对齐线显示**：拖拽时显示黄色参考线
- **自动吸附**：接近中心线时自动对齐
- **多方向支持**：支持水平和垂直方向对齐

```qml
// 水平对齐检测
if(Math.abs(centerX - root.width / 2) <= 20) {
    parent.x = root.width / 2 - parent.width / 2;
}
// 垂直对齐检测  
if(Math.abs(centerY - root.height / 2) <= 20) {
    parent.y = root.height / 2 - parent.height / 2;
}
```

### 4. 连接箭头系统 (Arrow Connection)

#### 视觉连接
- **动态箭头**：实时连接两个矩形的中心点
- **绿色填充**：箭头使用绿色填充，易于识别
- **十字标记**：箭头终点添加黄色十字标记

#### 实时更新
```qml
onWidthChanged: {
    canvas.startX = secondAreaRect1.x + secondAreaRect1.width / 2;
    canvas.startY = secondAreaRect1.y + secondAreaRect1.height / 2;
    canvas.endX = firstAreaRect1.x + firstAreaRect1.width / 2;
    canvas.endY = firstAreaRect1.y + firstAreaRect1.height / 2;
    canvas.requestPaint();
}
```

### 5. 可调整边框 (Resizable Borders)

#### TResizeBorder 组件
- **动态缩放**：支持拖拽调整矩形大小
- **保持比例**：可选择保持宽高比
- **实时反馈**：调整过程中实时更新连接箭头

#### 视觉反馈
- **黄色角标**：8个黄色小矩形标识矩形的四角和边中点
- **清晰边界**：提供明确的交互区域提示

## 技术架构

### 组件结构
```
Window (root)
├── Image (背景图像)
├── Item (firstArea - 左侧矩形系统)
│   ├── Rectangle (主矩形 + 交互逻辑)
│   ├── TResizeBorder (可调整边框)
│   └── Rectangle[] (5个遮罩矩形)
├── Item (secondArea - 右侧矩形系统)
│   ├── Rectangle (主矩形 + 交互逻辑)  
│   ├── TResizeBorder (可调整边框)
│   └── Rectangle[] (5个遮罩矩形)
├── Item (对齐辅助线)
│   ├── Rectangle (水平线)
│   └── Rectangle (垂直线)
└── Canvas (连接箭头)
```

### 状态管理
- **viewState**: 控制窗口开启/关闭状态
- **dragging**: 跟踪拖拽状态
- **defaultBackgroudState1/2**: 控制遮罩显示状态

### 属性绑定
- 使用 Qt 的属性绑定系统实现响应式更新
- 所有UI元素通过属性自动同步状态变化

## 使用指南

### 基本操作
1. **单击矩形**：开启/关闭窗口查看模式
2. **拖拽矩形**：移动窗口位置
3. **调整大小**：拖拽矩形边缘或角落
4. **对齐辅助**：拖拽时注意黄色参考线

### 交互规则
- 同一时间只能开启一个窗口
- 矩形无法拖拽出窗口边界
- 接近中心线时会自动对齐

## 项目文件

- `main.qml` - 主UI文件，包含所有核心逻辑
- `main.cpp` - Qt应用程序入口点
- `TResizeBorder.qml` - 可调整边框组件
- `demo04.pro` - Qt项目配置文件
- `imageRes.qrc` - 图像资源文件

## 技术特点

1. **模块化设计**：清晰的组件分离和职责划分
2. **响应式UI**：基于属性绑定的自动更新机制
3. **流畅交互**：优化的拖拽和对齐体验
4. **可扩展性**：易于添加新功能和自定义组件

## 开发环境

- Qt 5.15+
- Qt Quick 2.15
- C++11 或更高版本

## 编译运行

```bash
qmake demo04.pro
make
./demo04
```

## 未来扩展

- [ ] 支持多个窗口同时开启
- [ ] 添加窗口动画效果
- [ ] 支持不同形状的窗口
- [ ] 添加键盘快捷键支持
- [ ] 实现窗口状态保存/恢复


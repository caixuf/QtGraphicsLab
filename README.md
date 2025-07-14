# QtGraphicsLab - Interactive Graphics Framework

## 项目概述

一个基于Qt Graphics Framework开发的高级图形交互实验室，实现了可拖拽、可旋转、可缩放的矩形图元，并集成了背景遮罩、箭头连接线等功能。该项目展示了现代C++和Qt框架在图形界面开发中的最佳实践。

## 核心技术特性

### 🎯 **图形交互系统**

- **多状态图元操作**：支持拖拽移动、边界调整、旋转变换等多种交互模式
- **精确碰撞检测**：基于多边形的精确边界检测，支持旋转后的复杂几何计算
- **实时视觉反馈**：鼠标悬停时显示操作提示和辅助线

### 🏗️ **架构设计模式**

- **资源管理器模式**：实现了单例模式的资源管理器，统一管理图片资源路径
- **信号槽机制**：充分利用Qt的信号槽系统，实现组件间松耦合通信
- **组合模式**：通过GraphicsManager统一管理多个图形元素的生命周期

### 🎨 **高级图形功能**

- **自定义图元类**：继承QGraphicsItem实现定制化的矩形和箭头图元
- **动态箭头绘制**：智能计算箭头方向，实时更新连接线位置
- **背景遮罩效果**：半透明遮罩层提升视觉层次感

## 技术栈

| 类别 | 技术 |
|------|------|
| **编程语言** | C++17 |
| **UI框架** | Qt 5/6 (Qt Widgets + Qt Graphics) |
| **图形渲染** | QPainter、QGraphicsScene/View |
| **构建系统** | QMake |
| **设计模式** | 单例、观察者、组合模式 |

## 核心组件架构

```text
📦 Graphics System
├── 🎮 GraphicsManager        # 图形场景管理器
├── 🔲 myGraphicRectItem      # 可交互矩形图元
├── 🏞️ MyGraphicBackgroundItem # 背景遮罩图元  
├── ➡️ ArrowItem              # 智能箭头连接线
└── 📁 ResourceManager        # 资源路径管理器
```

## 实现亮点

### 1. **精确的几何变换计算**

```cpp
// 支持任意角度旋转的多边形碰撞检测
QPolygonF getRotatePolygonFromRect(QPointF center, QRectF rect, qreal angle);
```

### 2. **智能交互状态管理**

```cpp
enum STATE_FLAG {
    MOV_LEFT_LINE, MOV_TOP_LINE, MOV_RIGHT_LINE, MOV_BOTTOM_LINE,
    MOV_RIGHTBOTTOM_RECT, MOV_RECT, ROTATE
};
```

### 3. **资源管理最佳实践**

```cpp
class ResourceManager {
    static ResourceManager& instance();
    QString getImagePath(ImageType type) const;
};
```

## 项目特色

- ✅ **高性能渲染**：优化的重绘区域计算，确保流畅的实时交互
- ✅ **跨平台兼容**：基于Qt框架，支持Windows/Linux/macOS
- ✅ **模块化设计**：清晰的职责分离，便于扩展和维护
- ✅ **内存安全**：采用RAII原则，智能指针管理资源生命周期

## 构建说明

```bash
# 生成Makefile
qmake QtGraphicsLab.pro

# 编译项目
make

# 运行程序
./QtGraphicsLab
```

## 学习价值

此项目适合作为Qt Graphics开发的学习案例，涵盖了：

- Qt Graphics Framework的深度应用
- 现代C++特性的实际运用  
- 图形界面交互设计模式
- 高性能2D图形渲染技术

---

### 开发环境

Qt 5.x/6.x + C++17 + QMake
#ifndef QTGRAPHICSLAB_GLOBALS_H
#define QTGRAPHICSLAB_GLOBALS_H

// 放大镜质量级别枚举
enum MagnifierQuality {
    QUALITY_FAST = 0,       // 快速模式（默认）
    QUALITY_BALANCED = 1,   // 平衡模式
    QUALITY_HIGH = 2,       // 高质量模式
    QUALITY_ULTRA = 3       // 超高质量模式（最慢）
};

// 场景尺寸常量
#define SCENE_WIDTH 806
#define SCENE_HEIGHT 480

// 为了向后兼容保留旧的命名
#define WIDTH SCENE_WIDTH
#define HEIGHT SCENE_HEIGHT

// 图片资源路径常量
#define DEFAULT_RECT_IMAGE_PATH ":/image/images/a1.png"
#define BACKGROUND_IMAGE_PATH ":/image/images/a1.png"

// 为了向后兼容保留旧的命名
#define DEFAULT_RECT_IMAGE DEFAULT_RECT_IMAGE_PATH
#define BACKGROUND_IMAGE BACKGROUND_IMAGE_PATH

// 交互常量
#define MIN_RECT_SIZE 30.0          // 最小矩形尺寸
#define ROTATION_HANDLE_SIZE 20.0   // 旋转手柄大小
#define RESIZE_HANDLE_SIZE 8.0      // 调整大小手柄尺寸

// 放大镜质量优化常量
#define MAGNIFIER_QUALITY_MULTIPLIER 2.0   // 放大镜质量倍数
#define MAGNIFIER_MIN_SIZE 32              // 放大镜最小处理尺寸
#define MAGNIFIER_MAX_SIZE 4096            // 放大镜最大处理尺寸（提高到4096）
#define MAGNIFIER_INTERPOLATION_THRESHOLD 1.2  // 插值阈值（进一步降低）
#define MAGNIFIER_SUPERSAMPLING_FACTOR 3.0     // 超采样因子（提高到3.0）
#define MAGNIFIER_STEPS_THRESHOLD 3.0          // 多步骤缩放阈值
#define MAGNIFIER_EDGE_ENHANCE_STRENGTH 0.3    // 边缘增强强度（提高）

// 超分辨率算法常量 - 超级优化版本
#define SUPER_RESOLUTION_THRESHOLD 1.5         // 超分辨率启用阈值（降低）
#define SUPER_RESOLUTION_SCALE_FACTOR 4.0      // 超分辨率缩放因子（提高到4.0）
#define BICUBIC_SHARPNESS -0.75                // 双三次插值锐度参数（更锐利）
#define EDGE_THRESHOLD 20.0                    // 边缘检测阈值（更敏感）
#define NOISE_REDUCTION_STRENGTH 0.05          // 噪声抑制强度（降低保持细节）

// 超级优化参数
#define ULTRA_SAMPLING_FACTOR 4.0              // 超高采样因子
#define MULTI_LEVEL_EDGE_SAMPLES 5             // 多级边缘采样数
#define ADAPTIVE_KERNEL_SIZE 6                 // 自适应核心大小
#define COLOR_SPACE_ENHANCEMENT 1              // 色彩空间增强
#define LUMINANCE_WEIGHT 0.299                 // 亮度权重（Y分量）
#define CHROMINANCE_U_WEIGHT 0.587             // 色度U权重
#define CHROMINANCE_V_WEIGHT 0.114             // 色度V权重
#define DETAIL_ENHANCEMENT_FACTOR 1.5          // 细节增强因子
#define CONTRAST_ENHANCEMENT_FACTOR 1.2        // 对比度增强因子

#endif // QTGRAPHICSLAB_GLOBALS_H

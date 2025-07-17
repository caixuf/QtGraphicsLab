#ifndef QTGRAPHICSLAB_GLOBALS_H
#define QTGRAPHICSLAB_GLOBALS_H

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

#endif // QTGRAPHICSLAB_GLOBALS_H

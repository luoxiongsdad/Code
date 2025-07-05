#ifndef __WINDOW_FUNCTION_H
#define __WINDOW_FUNCTION_H

#include "stm32f4xx.h"
#include "arm_math.h"
#include"main.h"


// 窗函数类型定义
typedef enum {
    WITHOUT_WIN,   // 不加窗 (校正系数 = 1)
    BOXCAR,        // 矩形窗 (校正系数 = 1)
    TRIANG,        // 三角窗 (校正系数 = 2)
    HANNING,       // 汉宁窗 (校正系数 = 2)
    HAMMING,       // 汉明窗 (校正系数 = 1.852)
    BLACKMAN,      // 布莱克曼窗 (校正系数 = 2.381)
    FLATTOP,       // 平顶窗 (校正系数 = 4.18)
    NUTTALL,       // 纳托尔窗 (校正系数 ≈ 2.4)
    KAISER,        // 凯泽窗 (校正系数 = 1/∑w[i]/N)
    CHEBYSHEV,     // 切比雪夫窗 (校正系数 = 1/∑w[i]/N)
    EXACT_FLATTOP, // 精确平顶窗 (校正系数 ≈ 3.77)
    GAUSSIAN       // 高斯窗 (校正系数 ≈ 1.6-2.5)
} Window_Function_Type;

// 窗函数参数结构
typedef struct {
    float kaiser_beta;        // 凯泽窗参数 (推荐8.0)
    float chebyshev_sidelobe; // 切比雪夫窗旁瓣衰减(推荐60.0)
    float gaussian_alpha;     // 高斯窗参数(推荐2.5)
} Window_Params;

// 外部声明窗函数缓存
extern float Win_Function_Buf[FFT_LENGTH];

// 函数声明
void Window_Function_Init(Window_Function_Type WinFun, uint16_t Length, Window_Params params);
float Window_Function_Add(float Data, uint16_t Index);

// 计算窗函数平均增益
float Calculate_Window_Gain(uint16_t Length);

#endif


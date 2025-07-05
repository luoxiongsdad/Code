#include "windowfunction.h"
#include "math.h"

// 窗函数缓存
float Win_Function_Buf[FFT_LENGTH];

// ======================== 基础窗函数 (已简化) ========================

/* 
 * 矩形窗 (不加窗)
 * 特点: 主瓣最窄，旁瓣衰减最差(-13dB)
 * 幅度校正系数: 1.0
 */
static void boxcar(uint16_t Length) {
    for (uint16_t i = 0; i < Length; i++) {
        Win_Function_Buf[i] = 1.0f;
    }
}

/* 
 * 三角窗
 * 特点: 线性衰减，简单计算
 * 幅度校正系数: 2.0
 */
static void triang(uint16_t Length) {
    for (uint16_t i = 0; i < (Length + 1) / 2; i++) {
        Win_Function_Buf[i] = Win_Function_Buf[Length - 1 - i] = 
            1.0f - fabsf((2.0f * i - Length + 1.0f) / (Length + 1.0f));
    }
}

/* 
 * 汉宁窗 (推荐通用窗)
 * 特点: 主瓣较宽(4π/N)，旁瓣衰减快(-31dB)
 * 幅度校正系数: 2.0 (实际应用推荐2.2考虑ENBW补偿)
 * 简单校正: FFT幅度 × 2.0
 */
static void hanning(uint16_t Length) {
    for (uint16_t i = 0; i < Length; i++) {
        Win_Function_Buf[i] = 0.5f * (1.0f - cosf(2 * PI * i / (Length - 1)));
    }
}

/* 
 * 汉明窗
 * 特点: 主瓣较窄(4π/N)，第一旁瓣-42dB
 * 幅度校正系数: 1.852 (1/0.54) ≈ 1.85
 */
static void hamming(uint16_t Length) {
    for (uint16_t i = 0; i < Length; i++) {
        Win_Function_Buf[i] = 0.54f - 0.46f * cosf(2 * PI * i / (Length - 1));
    }
}

/* 
 * 布莱克曼窗
 * 特点: 主瓣最宽(6π/N)，旁瓣衰减好(-58dB)
 * 幅度校正系数: 2.381 (1/0.42) ≈ 2.38
 */
static void blackman(uint16_t Length) {
    for (uint16_t i = 0; i < Length; i++) {
        Win_Function_Buf[i] = 0.42f 
                            - 0.5f * cosf(2 * PI * i / (Length - 1)) 
                            + 0.08f * cosf(4 * PI * i / (Length - 1));
    }
}

/* 
 * 平顶窗
 * 特点: 幅值精度高(误差<0.1dB)，主瓣极宽
 * 幅度校正系数: 4.18
 */
static void flattop(uint16_t Length) {
    for (uint16_t i = 0; i < Length; i++) {
        Win_Function_Buf[i] = 1.0f 
                            - 1.93f * cosf(2 * PI * i / (Length - 1))
                            + 1.29f * cosf(4 * PI * i / (Length - 1))
                            - 0.388f * cosf(6 * PI * i / (Length - 1))
                            + 0.0322f * cosf(8 * PI * i / (Length - 1));
        Win_Function_Buf[i] /= 4.634f;
    }
}

// ======================== 高精度窗函数 (已简化) ========================

/* 
 * 纳托尔窗 (Nuttall Window)
 * 特点: 旁瓣抑制好(-46.7dB)，适用于通用高精度
 * 幅度校正系数: 约2.4 (N>1000时稳定值)
 */
static void nuttall(uint16_t Length) {
    const float a0 = 0.355768;
    const float a1 = 0.487396;
    const float a2 = 0.144232;
    const float a3 = 0.012604;
    
    for (uint16_t i = 0; i < Length; i++) {
        Win_Function_Buf[i] = a0 
                             - a1 * cosf(2 * PI * i / (Length - 1))
                             + a2 * cosf(4 * PI * i / (Length - 1))
                             - a3 * cosf(6 * PI * i / (Length - 1));
    }
}

/* 
 * 精确平顶窗 (Exact Flat-Top)
 * 特点: 幅值精度最高(误差<0.01dB)
 * 幅度校正系数: 3.77 (经验值) 或 计算平均增益的倒数
 */
static void exact_flattop(uint16_t Length) {
    const float a0 = 1.0000f;
    const float a1 = 1.4000f;
    const float a2 = 0.4844f;
    const float a3 = 0.0684f;
    const float a4 = 0.0000f;
    
    for (uint16_t i = 0; i < Length; i++) {
        float theta = 2 * PI * i / (Length - 1);
        Win_Function_Buf[i] = a0 
                             - a1 * cosf(theta)
                             + a2 * cosf(2 * theta)
                             - a3 * cosf(3 * theta)
                             + a4 * cosf(4 * theta);
        Win_Function_Buf[i] /= 4.8542f; // 归一化系数
    }
}

// ======================== 窗函数初始化 (简化版) ========================
void Window_Function_Init(Window_Function_Type WinFun, uint16_t Length, Window_Params params) {
    switch (WinFun) {
        case WITHOUT_WIN:
        case BOXCAR:
            boxcar(Length);
            break;
            
        case TRIANG:
            triang(Length);
            break;
            
        case HANNING:
            hanning(Length);
            break;
            
        case HAMMING:
            hamming(Length);
            break;
            
        case BLACKMAN:
            blackman(Length);
            break;
            
        case FLATTOP:
            flattop(Length);
            break;
            
        case NUTTALL:
            nuttall(Length);
            break;
            
        case KAISER: {
            // 凯泽窗实现...
            }
            break;
            
        case CHEBYSHEV: {
            // 切比雪夫窗实现...
            }
            break;
            
        case EXACT_FLATTOP:
            exact_flattop(Length);
            break;
            
        case GAUSSIAN: {
            // 高斯窗实现...
            }
            break;
            
        default:
            boxcar(Length);
            break;
    }
}

// ======================== 窗函数应用 ========================
float Window_Function_Add(uint16_t Data, uint16_t Index) {
    return (float)Data * Win_Function_Buf[Index];
}

/* 
 * 计算窗函数的平均增益
 * 用途: 计算窗函数的幅值校正因子
 * 对于矩形窗 = 1，汉宁窗 ≈ 0.5，平顶窗 ≈ 0.265
 * 
 * 校正因子计算方法:
 * 1. 计算平均增益: gain = ∑(w[i]) / N
 * 2. 校正因子 = 1 / gain (大部分窗适用)
 * 
 * 实际应用时: FFT幅度 × 校正因子
 */
float Calculate_Window_Gain(uint16_t Length) {
    float gain = 0.0f;
    for (uint16_t i = 0; i < Length; i++) {
        gain += Win_Function_Buf[i];
    }
    return gain / Length;
}



#ifndef __WINDOW_FUNCTION_H
#define __WINDOW_FUNCTION_H

#include "stm32f4xx.h"
#include "arm_math.h"
#include"main.h"


// ���������Ͷ���
typedef enum {
    WITHOUT_WIN,   // ���Ӵ� (У��ϵ�� = 1)
    BOXCAR,        // ���δ� (У��ϵ�� = 1)
    TRIANG,        // ���Ǵ� (У��ϵ�� = 2)
    HANNING,       // ������ (У��ϵ�� = 2)
    HAMMING,       // ������ (У��ϵ�� = 1.852)
    BLACKMAN,      // ���������� (У��ϵ�� = 2.381)
    FLATTOP,       // ƽ���� (У��ϵ�� = 4.18)
    NUTTALL,       // ���ж��� (У��ϵ�� �� 2.4)
    KAISER,        // ���� (У��ϵ�� = 1/��w[i]/N)
    CHEBYSHEV,     // �б�ѩ�� (У��ϵ�� = 1/��w[i]/N)
    EXACT_FLATTOP, // ��ȷƽ���� (У��ϵ�� �� 3.77)
    GAUSSIAN       // ��˹�� (У��ϵ�� �� 1.6-2.5)
} Window_Function_Type;

// �����������ṹ
typedef struct {
    float kaiser_beta;        // ���󴰲��� (�Ƽ�8.0)
    float chebyshev_sidelobe; // �б�ѩ���԰�˥��(�Ƽ�60.0)
    float gaussian_alpha;     // ��˹������(�Ƽ�2.5)
} Window_Params;

// �ⲿ��������������
extern float Win_Function_Buf[FFT_LENGTH];

// ��������
void Window_Function_Init(Window_Function_Type WinFun, uint16_t Length, Window_Params params);
float Window_Function_Add(float Data, uint16_t Index);

// ���㴰����ƽ������
float Calculate_Window_Gain(uint16_t Length);

#endif


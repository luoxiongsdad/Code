#include "windowfunction.h"
#include "math.h"

// ����������
float Win_Function_Buf[FFT_LENGTH];

// ======================== ���������� (�Ѽ�) ========================

/* 
 * ���δ� (���Ӵ�)
 * �ص�: ������խ���԰�˥�����(-13dB)
 * ����У��ϵ��: 1.0
 */
static void boxcar(uint16_t Length) {
    for (uint16_t i = 0; i < Length; i++) {
        Win_Function_Buf[i] = 1.0f;
    }
}

/* 
 * ���Ǵ�
 * �ص�: ����˥�����򵥼���
 * ����У��ϵ��: 2.0
 */
static void triang(uint16_t Length) {
    for (uint16_t i = 0; i < (Length + 1) / 2; i++) {
        Win_Function_Buf[i] = Win_Function_Buf[Length - 1 - i] = 
            1.0f - fabsf((2.0f * i - Length + 1.0f) / (Length + 1.0f));
    }
}

/* 
 * ������ (�Ƽ�ͨ�ô�)
 * �ص�: ����Ͽ�(4��/N)���԰�˥����(-31dB)
 * ����У��ϵ��: 2.0 (ʵ��Ӧ���Ƽ�2.2����ENBW����)
 * ��У��: FFT���� �� 2.0
 */
static void hanning(uint16_t Length) {
    for (uint16_t i = 0; i < Length; i++) {
        Win_Function_Buf[i] = 0.5f * (1.0f - cosf(2 * PI * i / (Length - 1)));
    }
}

/* 
 * ������
 * �ص�: �����խ(4��/N)����һ�԰�-42dB
 * ����У��ϵ��: 1.852 (1/0.54) �� 1.85
 */
static void hamming(uint16_t Length) {
    for (uint16_t i = 0; i < Length; i++) {
        Win_Function_Buf[i] = 0.54f - 0.46f * cosf(2 * PI * i / (Length - 1));
    }
}

/* 
 * ����������
 * �ص�: �������(6��/N)���԰�˥����(-58dB)
 * ����У��ϵ��: 2.381 (1/0.42) �� 2.38
 */
static void blackman(uint16_t Length) {
    for (uint16_t i = 0; i < Length; i++) {
        Win_Function_Buf[i] = 0.42f 
                            - 0.5f * cosf(2 * PI * i / (Length - 1)) 
                            + 0.08f * cosf(4 * PI * i / (Length - 1));
    }
}

/* 
 * ƽ����
 * �ص�: ��ֵ���ȸ�(���<0.1dB)�����꼫��
 * ����У��ϵ��: 4.18
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

// ======================== �߾��ȴ����� (�Ѽ�) ========================

/* 
 * ���ж��� (Nuttall Window)
 * �ص�: �԰����ƺ�(-46.7dB)��������ͨ�ø߾���
 * ����У��ϵ��: Լ2.4 (N>1000ʱ�ȶ�ֵ)
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
 * ��ȷƽ���� (Exact Flat-Top)
 * �ص�: ��ֵ�������(���<0.01dB)
 * ����У��ϵ��: 3.77 (����ֵ) �� ����ƽ������ĵ���
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
        Win_Function_Buf[i] /= 4.8542f; // ��һ��ϵ��
    }
}

// ======================== ��������ʼ�� (�򻯰�) ========================
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
            // ����ʵ��...
            }
            break;
            
        case CHEBYSHEV: {
            // �б�ѩ��ʵ��...
            }
            break;
            
        case EXACT_FLATTOP:
            exact_flattop(Length);
            break;
            
        case GAUSSIAN: {
            // ��˹��ʵ��...
            }
            break;
            
        default:
            boxcar(Length);
            break;
    }
}

// ======================== ������Ӧ�� ========================
float Window_Function_Add(uint16_t Data, uint16_t Index) {
    return (float)Data * Win_Function_Buf[Index];
}

/* 
 * ���㴰������ƽ������
 * ��;: ���㴰�����ķ�ֵУ������
 * ���ھ��δ� = 1�������� �� 0.5��ƽ���� �� 0.265
 * 
 * У�����Ӽ��㷽��:
 * 1. ����ƽ������: gain = ��(w[i]) / N
 * 2. У������ = 1 / gain (�󲿷ִ�����)
 * 
 * ʵ��Ӧ��ʱ: FFT���� �� У������
 */
float Calculate_Window_Gain(uint16_t Length) {
    float gain = 0.0f;
    for (uint16_t i = 0; i < Length; i++) {
        gain += Win_Function_Buf[i];
    }
    return gain / Length;
}



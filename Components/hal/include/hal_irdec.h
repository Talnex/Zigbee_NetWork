/***********************************************************
**  FileName:         IRDecode.h
**  Introductions:    IR Remote decode for MCU of CC2530
**  Last Modify time: 2013.05.20
**  Modify:           �޸Ķ�ʱ��4�����жϽ����еĳ�ʱʧЧ
**  Author:           GuoZhenjiang <zhenjiang.guo@sunplusapp.com>
***********************************************************/
/***********************����ԭ��****************************
*   1����ʱ��1��ͨ��0(Alt.2 = P1.2)�Ǻ����źŽ�����������
*   2�����ö�ʱ��1ͨ��0���������벶��ģʽ��ȡ�����ź�
*   3����ʱ��1����Ƶ��1MHz
*   4��ÿ�������ز�����ȡ��ǰ����ֵ(����)��Ȼ���������ֵ�����¿�ʼ����
*   5����ȡ��ÿһС���ڰ���ʱ�䷧ֵ�ж��������롢����0������1
*   5��������������ö�ʱ��4�����ж��Ƿ�ʱ
***********************************************************/

#ifndef __IRDECODE_H__
#define __IRDECODE_H__

#include "OSAL.h"

#define IRDEC_INT_EVENT     0xF1    //IO�˿��жϲ���0xF0

typedef struct {
    uint8 irLen;
    uint8 irCode[15];
} OSALIRDecData_t;

typedef struct
{
    osal_event_hdr_t hdr;  //�¼����ͼ�״̬
    uint8 endPoint;
    OSALIRDecData_t *data;
} OSALIRDecIntData_t;


#define CLR_T1CNT       T1CNTL = 0  // ��T1CNTL�Ĵ�����д�κ�ֵ�ᵼ��T1CNT���㡣

// T1CTL
// ��ʱ��1ʱ��Ԥ��Ƶ
typedef enum
{
    Tdiv_1      = 0X00, //���Ƶ��/1
    Tdiv_8      = 0X04, //���Ƶ��/8
    Tdiv_32     = 0X08, //���Ƶ��/32
    Tdiv_128    = 0X0C, //���Ƶ��/128
}Timer1Div_t;

// ��ʱ��1����ģʽ
typedef enum
{
    Tmod_stop   = 0X00, //��ͣ����
    Tmod_free   = 0X01, //��������
    Tmod_Mod    = 0X02, //ģģʽ
    Tmod_PN     = 0X03, //��/������ģʽ
}Timer1Mod_t;

// T1CCTLx(x = 0,1,2,3,4,5)
// ��ʱ��1ͨ������
typedef enum
{
    TCCSet      = 0X00, //�Ƚ�ƥ��ʱ��λ
    TCCClear    = 0X08, //�Ƚ�ƥ��ʱ����
    TCCExch     = 0X02, //ģģʽ
    TCCSet0Clr  = 0X03, //��/������ģʽ
}Timer1CC_t;

// ����ң�ر����������� in us
// 4.500ms�������жϷ�ֵ
#define T_IR_GUIDE4_MIN       8000      //4.5ms������ ����ʱ��9.000ms
#define T_IR_GUIDE4_MAX       9500
// 9.000ms�������жϷ�ֵ
#define T_IR_GUIDE9_MIN      12000      //9ms������ ����ʱ��13.500ms
#define T_IR_GUIDE9_MAX      14000
// ����λ0�жϷ�ֵ
#define T_IR_0_MIN            1000      //λ0 ����ʱ��1.125ms
#define T_IR_0_MAX            1500
// ����λ1�жϷ�ֵ
#define T_IR_1_MIN            2000      //λ1 ����ʱ��2.250ms
#define T_IR_1_MAX            2300
// �����ж�һ�κ����źŽ��������ڷ�ֵ
#define T_IR_ENDED            50      // ��λ1��������΢����,���������뷶Χ��

typedef enum
{
    WaitForGuideTrig1,      // �ȴ��������һ��������
    WaitForGuideTrig2,      // �ȴ�������ڶ���������
    Decoding,               // λ0λ1���������
    Decode_GuideOverTime,   // ����������ȴ���ʱ
    Decode_bitOverTime,     // λ0 λ1 �����ȴ���ʱ
    Decode_BufOverflow,     // ���ն���������½������
    Decode_OverUnknow,      // δ֪ԭ���½������
}IRDecodeSta_t;

// �жϴ�������ң�ؽ����ʼ��
void IRDecodeT1Init(unsigned char taskid, unsigned char ep);

#endif  //__IRDECODE_H__
/***********************************************************
**  FileName:         IR.h
**  Introductions:    Sunplusapp CC2530 ZigBee Node IR Romoter Signal Send Functions
**  Last Modify time: 2013.05.21
**  Modify:           ȥ����ͷ�ļ���û�õĺ궨��,���ע��
**  Author:           GuoZhenjiang <zhenjiang.guo@sunplusapp.com>
***********************************************************/

#ifndef __IR_H__
#define __IR_H__
#include <hal_board.h>

// ���ⷢ������[P1.3]�˿ڿ�����ؼĴ���
#define PSEL_IR_SEND    P1SEL
#define PDIR_IR_SEND    P1DIR
#define POUT_IR_SEND    P1
// ���ⷢ������λ���
#define BIT_IR_SEND     3

// ������뷢��״̬ö��
typedef enum
{
    IRSta_Guide,    // �����������
    IRSta_T3PWM,    // Timer3����38kHz���Ʋ�
    IRSta_Data0,    // IR��������͵�ƽ
    IRSta_Stop,     // ����ֹͣλ
}IRSendSta_t;

enum
{
    IRGuideLen_9ms  = 1,    // ���������볤��9.0ms
    IRGuideLen_13ms = 2,    // ���������볤��13.5ms
};

// λ���в����궨��
#define BIT_1(x)   ( BV(x))
#define BIT_0(x)   (~BV(x))

#define SET_IOB_BIT     POUT_IR_SEND |= BIT_1(BIT_IR_SEND)
#define CLR_IOB_BIT     POUT_IR_SEND &= BIT_0(BIT_IR_SEND)

#define IR_SEL_IOOUT    PSEL_IR_SEND &= BIT_0(BIT_IR_SEND); PDIR_IR_SEND |= BIT_1(BIT_IR_SEND);
#define IR_SEL_T3PWM    PSEL_IR_SEND |= BIT_1(BIT_IR_SEND)
#define STOP_T1_T3      T3CTL = 0x00;   T1CTL = 0x00

// ������뷢�ͳ�ʼ��
void IRSendInit(void);
// ������뷢�ͺ���
int GenIR(uint8 *IRData , uint8 Mode , uint8 CodeLen );

#endif  // __IR_H__

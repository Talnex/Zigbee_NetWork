/***********************************************************
**  FileName:         IR.c
**  Introductions:    Sunplusapp CC2530 ZigBee Node IR Romoter Signal Send Functions
**  Last Modify time: 2013.05.21
**  Modify:           �޸ĺ�����뷢�ͣ��Ż�ʱ�����ע��
**  Author:           GuoZhenjiang <zhenjiang.guo@sunplusapp.com>
***********************************************************/

#include "IR.h"
#include "OSAL.h"

#if defined(HAL_IRENC) && (HAL_IRENC == TRUE)

// ��ʱ��1��������,����ÿһλ���������
volatile uint16  T1_80usCnt = 0;

// Timer1��������ʼ��
void IRSend_T1_Init(void);
// Timer3��������ʼ��
void IRSend_T3_Init(void);
// Timer1 & Timer3 �жϴ���������
__near_func __interrupt void IRSend_T1IntHandle(void);
__near_func __interrupt void IRSend_T3IntHandle(void);

/***********************************************************
**  ��������: GenIR
**  ʵ�ֹ���: ϵͳʱ������
**  ��ڲ���: IRData:   ������������׵�ַ
**            Mode:     ���������:1 ����9ms    2:����13.56ms
**            CodeLen:  ����λ��(bit,����Byte)
**  ���ؽ��: 0:����ʧ��    1:���ͳɹ�
***********************************************************/
int GenIR(uint8 *IRData , uint8 Mode , uint8 CodeLen )
{
    uint16  GuideCodeLen=0;
    static IRSendSta_t IRSendSta = IRSta_Guide;
    static uint8 cntByte=0, cntbit=0, bitNum;
    bitNum = CodeLen;       //����λ��

    if(Mode == 1)
      GuideCodeLen = 56;//226;   �����볤�ȣ�4.5ms��4.5ms��
    else if(Mode == 2)
      GuideCodeLen = 114;   //�����볤�ȣ�9ms��4.5ms��

    IRSend_T1_Init();
    IRSend_T3_Init();
    IR_SEL_IOOUT;
    CLR_IOB_BIT;
    IRSendSta = IRSta_Guide;
    T1_80usCnt = 0;
    IR_SEL_T3PWM;
    while(1)
    {
        // ��ѯ������뷢��״̬
        switch(IRSendSta)
        {
            // ������׶�
        case IRSta_Guide:	
            // ������4.5ms��9ms�ز��׶�
            if(T1_80usCnt <= GuideCodeLen)
            {
                IRSendSta = IRSta_Guide;
                IR_SEL_T3PWM;
            }
            // ������4.5ms�͵�ƽ�׶�
            else if((T1_80usCnt > GuideCodeLen) && (T1_80usCnt <= GuideCodeLen+56))
            {
                IRSendSta = IRSta_Guide;
                IR_SEL_IOOUT;
                CLR_IOB_BIT;
            }
            // �����뷢�����,׼����������λ
            else
            {
                IRSendSta = IRSta_T3PWM;
                IR_SEL_T3PWM;
                T1_80usCnt = 0;
            }
            break;
            // ����λ�е�λ0��λ1��0.56ms��38kHz�ز��׶�
        case IRSta_T3PWM:
            if(T1_80usCnt >= 7)     // 7 * 80us = 560us
            {
                IR_SEL_IOOUT;
                CLR_IOB_BIT;
                IRSendSta = IRSta_Data0;
            }
            break;
            // ����λ�е�λ0��λ1�ĵ͵�ƽ�����׶�
        case IRSta_Data0:
            //��������0,�͵�ƽ���� 0.565ms��
            if(!(IRData[cntByte] & (0x80 >> (cntbit % 8))))
            {
                if(T1_80usCnt >= 14)
                {
                    IR_SEL_T3PWM;
                    T1_80usCnt = 0;
                    cntbit++;
                    if(0 == (cntbit % 8))
                        cntByte++;
                }
            }
            //��������1,�͵�ƽ���� 1.685ms��
            else if(IRData[cntByte] & (0x80 >> (cntbit % 8)))
            {
                if(T1_80usCnt >= 28)
                {				
                    IR_SEL_T3PWM;
                    T1_80usCnt = 0;
                    cntbit++;
                    if(0 == (cntbit % 8))
                        cntByte++;
                }
            }	
            // �������?
            if(cntbit >= bitNum)	
                IRSendSta = IRSta_Stop;
            // ��������?
            else
                IRSendSta = IRSta_T3PWM;
            break;
            // ����������ݲ��ַ������
        case IRSta_Stop:
            // һ����뷢�����  ����λ�� 0.56ms �� 38K �ز�
            while(T1_80usCnt < 7)
                ;
            STOP_T1_T3; // stop T1 & T3
            T1_80usCnt = 0;
            IR_SEL_IOOUT;
            CLR_IOB_BIT;
            cntbit = 0;
            cntByte = 0;
            return 1;
            break;
        default:
            return 0;
            break;
        }							
    }
}

/***********************************************************
**  ��������: IRSend_T1_Init
**  ʵ�ֹ���: ������뷢�Ͷ�ʱ��1��ʼ��
**  ��ڲ���: None
**  ���ؽ��: None
***********************************************************/
void IRSend_T1_Init(void)
{
    T1CTL = 0x0E;       // f = 32MHz / 128 = 250 000Hz, T = 4us;ģģʽ,��0~T1CC0����������
    T1CCTL0 = 0x44;     // Timer1ͨ��0�ж�����,�Ƚ�ƥ��ģʽ,�Ƚϱ���ʱ�����λ��
    T1CC0L = 19;        // ��дT1CC0L,��дT1CC0H,T1CNT=0ʱ���¡� 4us*20 = 80us;
    T1CC0H = 0;
    TIMIF |= BIT_1(6);  // TIMIF.T1OVFIM Timer1����ж�����
    IEN1 |= BIT_1(1);   // IEN1.T1IE Timer1���ж�����
    EA = 1;             // ��ȫ�����ж�
    T1_80usCnt = 0;     // 80us����ж�����������
}

/***********************************************************
**  ��������: IRSend_T3_Init
**  ʵ�ֹ���: ������뷢�Ͷ�ʱ��3��ʼ��
**  ��ڲ���: None
**  ���ؽ��: None
***********************************************************/
void IRSend_T3_Init(void)
{
    P1SEL |= BIT_1(3);      // P1.3 us as T3 PWM Out Pin.
    T3CTL = 0x02;           // T3CTL.MODE[10] ģģʽ,��0~T3CC0��������
    T3CTL |= BIT_1(5);      // T3CTL.MODE[001] f = 32MHz / 2 = 16MHz;
    T3CCTL0 |= BIT_1(2);    // T3CCTL0.MODE ����Ƚϱ���ģʽ
    T3CCTL0 |= BIT_1(4);    // T3CCTL0.CMP[010] �Ƚ�ƥ��ʱ,���ȡ��

    T3CC0 = 208;            // Timer3ͨ��0����Ƚ�ƥ������ֵ,16MHz / 208 = 76923.076923Hz; 76923 / 2 =38k(�Ƚ�ƥ��ʱȡ��).
    T3CTL |= BIT_1(4);      // T3CTL.START ����Timer3
    P2SEL |= BIT_1(5);      // P2SEL.PRI2P1 ��USART1��Timer3ռ����ͬ����,Timer3���ȡ�
    T3CCTL0 |= BIT_1(6);    // T3CCTL0.IM Timer3ͨ��0�ж�����
    IEN1 |= BIT_1(3);       // IEN1.T3IE Timer3���ж�����
    EA = 1;                 // ȫ�����ж�����
}

/***********************************************************
**  ��������: IRSendInit
**  ʵ�ֹ���: ������뷢�ͳ�ʼ��
**  ��ڲ���: None
**  ���ؽ��: None
***********************************************************/
void IRSendInit(void)
{
    IR_SEL_IOOUT;
    CLR_IOB_BIT;
}

/***********************************************************
**  ��������: IRSend_T1IntHandle
**  ʵ�ֹ���: Timer1 ������뷢���е��жϴ�����
**  ��ڲ���: None
**  ���ؽ��: None
***********************************************************/
#pragma vector = T1_VECTOR
__interrupt void IRSend_T1IntHandle(void)
{
    T1STAT &= BIT_0(5);     // T1STAT.OVFIF ���Timer1����жϱ�־
    T1STAT &= BIT_0(1);     // T1STAT.CH0IF ���Timer1ͨ��0�жϱ�־
    IRCON &= BIT_0(1);      // IRCON.T1IF   ���Timer1���жϱ�־
    T1_80usCnt++;
}

/***********************************************************
**  ��������: IRSend_T3IntHandle
**  ʵ�ֹ���: Timer3 ������뷢���е��жϴ�����
**  ��ڲ���: None
**  ���ؽ��: None
***********************************************************/
#pragma vector = T3_VECTOR
__near_func __interrupt void IRSend_T3IntHandle(void)
{
    TIMIF &= BIT_0(1);      // TIMIF.T3CH0IF
}


#endif  // defined(HAL_IRENC) && (HAL_IRENC == TRUE)

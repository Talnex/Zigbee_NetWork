/***********************************************************
**  FileName:         IRDecode.c
**  Introductions:    IR Remote decode for MCU of CC2530
**  Last Modify time: 2013.05.03
**  Update:           �����������
**  Author:           GuoZhenjiang<zhenjiang.guo@sunplusapp.com>
**  Modify:
**             2013.05.03   lijian  Change File Name to hal_irdec.c
***********************************************************/
#include <hal_board.h>
#include "hal_irdec.h"
#include <string.h>

#if defined(HAL_IRDEC) && (HAL_IRDEC == TRUE)
static struct {
    uint8 IRIntTskId;
    uint8 IRIntEp;
    OSALIRDecData_t IRCodeTab;
} IRDecodeRes;

volatile uint8 IROverTimeAdd_En=FALSE, IROverTimeOvf_flg=FALSE;
volatile uint16 IROverTimeAdd_cnt=0;
volatile IRDecodeSta_t IRDeodeSta = WaitForGuideTrig1;
static uint8 cnt_Byte, cnt_bit;     // ���ڿ��ƽ�����̵Ļ�������

/***********************************************************
**  ��������: IRDecodeT1Init
**  ʵ�ֹ���: �жϷ����к�������ʼ������
**  ��ڲ���: None
**  ���ؽ��: None
***********************************************************/
void IRDecodeT1Init(uint8 taskid, uint8 ep)
{
    // ��������źͶ˵��
    IRDecodeRes.IRIntTskId = taskid;
    IRDecodeRes.IRIntEp = ep;

    // ��ʼ��Timer1
    P1SEL |= 0x04;      // ����P1.2ΪTimer1�����蹦������
    P1DIR &= ~0x04;     // ���벶�����ű�������Ϊ����
    PERCFG |= 0x40;     // Timer1ռ�õ�2����λ��(ͨ��0ΪP1.2)
    P2SEL |= 0x04;      // ��USART0��Timer1��������ͬ���ţ�Timer1����

    // ��ʱ��1��ʼ��
    T1CTL = (uint8)Tdiv_32 | (uint8)Tmod_stop;    // 1MHz,��ͣ����
    CLR_T1CNT;                      // ���㶨ʱ��1

    // Timer1ͨ��0���������벶������
    T1CCTL0 = 0x01;                 // �����ز���,ͨ��0���벶���ж�δʹ��,��ѯ�жϱ�־����
    T1CCTL0 |= 0x40;                // T1CCTL0.IM ��ʱ��1ͨ��0�жϿ���
    T1STAT = 0;
    IEN1 |= 0x02;                   // IEN1.T1IE ��ʱ��1�ж��ܿ���
    EA = 1;                         // IEN0.EA ȫ�����жϿ���

    // ��ʼ��Time4
    P2SEL |= 0x10;  // ��ʱ��4���ȼ����ڶ�ʱ��1
    T4CTL |= 0xEC;  // 128��Ƶ,��ռ���ֵ,��������ж�,��������
    T4CTL |= 0x10;  // ������ʱ��
    IEN1 |= 0x10;   // �򿪶�ʱ��4���ж�
}

/***********************************************************
**  ��������: Timer1_ISR
**  ʵ�ֹ���: Timer1 �жϴ�����,����Timer1ͨ��0���벶����ʵ�ֺ������
**  ��ڲ���: None
**  ���ؽ��: None
***********************************************************/
HAL_ISR_FUNC_DECLARATION(Timer1_ISR, T1_VECTOR);
HAL_ISR_FUNCTION(Timer1_ISR, T1_VECTOR)
{
#define CLR_T1CH0_IF    T1STAT &= 0xFE
#define CLR_T1CH1_IF    T1STAT &= 0xFD
#define CLR_T1OVF_IF    T1STAT &= 0xDF
#define CLR_T1IF        IRCON &= 0xFD

    static uint8 IRdata[15];            // ���ڴ洢�����ĺ�������
    static uint16 tCapture = 0;         // ���ڴ洢�ж�ÿ����������

    // ��������в��񵽶�ʱ��1ͨ��0������
    if(T1STAT & 0x01)
    {
        switch(IRDeodeSta)
        {
        case WaitForGuideTrig1:
            IRDeodeSta = WaitForGuideTrig2;
            CLR_T1CNT;                      // ���㶨ʱ��1
            T1CTL = (uint8)Tdiv_32 | (uint8)Tmod_free;    // 1MHz,���ɼ���ģʽ
            break;
        case WaitForGuideTrig2:
            tCapture = T1CC0L;                          // ��ȡ��ǰһ������ֵ
            tCapture |= (uint16)T1CC0H << 8;
            CLR_T1CNT;                                  // ���㶨ʱ��1
            T1CTL = (uint8)Tdiv_32 | (uint8)Tmod_free;  // 1MHz,���ɼ���ģʽ
            // �����������4.5ms���������9ms������
            if(((tCapture > T_IR_GUIDE9_MIN) && (tCapture < T_IR_GUIDE9_MAX)) || ((tCapture > T_IR_GUIDE4_MIN) && (tCapture < T_IR_GUIDE4_MAX)))
            {
                IRDeodeSta = Decoding;
                // ׼�����빤��
                cnt_bit = 0;
                cnt_Byte = 0;
            }
            else
                IRDeodeSta = WaitForGuideTrig1;
            IROverTimeAdd_cnt = 0;
            break;
        case Decoding:
            tCapture = T1CC0L;                          // ��ȡ��ǰһ������ֵ
            tCapture |= (uint16)T1CC0H << 8;
            CLR_T1CNT;                                  // ���㶨ʱ��1
            T1CTL = (uint8)Tdiv_32 | (uint8)Tmod_free;  // 1MHz,���ɼ���ģʽ
            // ���յ�λ0?
            if((tCapture > T_IR_0_MIN) && (tCapture < T_IR_0_MAX))
            {
                IRDeodeSta = Decoding;
                IROverTimeAdd_cnt = 0;
                IRdata[cnt_Byte] &= ~(0x80 >> cnt_bit);    // ���浱ǰλ0
                cnt_bit++;
                if(cnt_bit >= 8)    // �������һ���ֽ�
                {
                    IRDecodeRes.IRCodeTab.irCode[cnt_Byte] = IRdata[cnt_Byte];
                    cnt_bit=0;
                    cnt_Byte++;
                    if(cnt_Byte >= 15)  // ���������ն��г���
                    {
                        IRDeodeSta = Decode_BufOverflow;
                        goto OneIRpkgGot;
                    }
                }
            }
            // ���յ�λ1?
            else if((tCapture > T_IR_1_MIN) && (tCapture < T_IR_1_MAX))
            {
                IRDeodeSta = Decoding;
                IROverTimeAdd_cnt = 0;
                IRdata[cnt_Byte] |= 0x80 >> cnt_bit;    // ���浱ǰλ0
                cnt_bit++;
                if(cnt_bit >= 8)    // �������һ���ֽ�
                {
                    IRDecodeRes.IRCodeTab.irCode[cnt_Byte] = IRdata[cnt_Byte];
                    cnt_bit=0;
                    cnt_Byte++;
                    if(cnt_Byte >= 15)  // ���������ն��г���
                    {
                        IRDeodeSta = Decode_BufOverflow;
                        goto OneIRpkgGot;
                    }
                }
            }
            // û�г�ʱ,����λ0,����λ1,����???
            else
            {
                IRDeodeSta = Decode_OverUnknow;
                goto OneIRpkgGot;
            }
            break;
        default :
            IRDeodeSta = WaitForGuideTrig1;
            break;
        }
    OneIRpkgGot:
        {
            switch(IRDeodeSta)     // һ�κ���������
            {
                // ���� ���ն���������½������
            case Decode_BufOverflow:
                // ���� δ֪ԭ���½������
            case Decode_OverUnknow:
                IROverTimeAdd_cnt = 0;
                IRDecodeRes.IRCodeTab.irLen = cnt_Byte;
                cnt_bit = 0;
                cnt_Byte = 0;
                OSALIRDecIntData_t* T1Ch0IntData;
                T1Ch0IntData = (OSALIRDecIntData_t *)osal_msg_allocate(sizeof(OSALIRDecIntData_t));
                T1Ch0IntData->hdr.event = IRDEC_INT_EVENT;
                T1Ch0IntData->endPoint = IRDecodeRes.IRIntEp;
                T1Ch0IntData->data = &IRDecodeRes.IRCodeTab;
                osal_msg_send(IRDecodeRes.IRIntTskId, (uint8*)(T1Ch0IntData));
                IRDeodeSta = WaitForGuideTrig1;
                break;
            default :
                break;
            }
        }
        CLR_T1CH0_IF;         // ���ͨ��0���벶���жϱ�־
    }
    CLR_T1IF;         // ���T1IF
 }

/***********************************************************
**  ��������: Timer4_ISR
**  ʵ�ֹ���: Timer4 �жϴ�����,����Timer1ͨ��0���벶����ʵ�ֺ������
**  ��ڲ���: None
**  ���ؽ��: None
***********************************************************/
HAL_ISR_FUNC_DECLARATION(Timer4_ISR, T4_VECTOR);
HAL_ISR_FUNCTION(Timer4_ISR, T4_VECTOR)
{
    // (32MHz / 128) T = 4us; f = 250000Hz;
    switch(IRDeodeSta)
    {
        // �ȴ��ڶ��δ���,����������Ƿ�ʱ
    case WaitForGuideTrig2:
        IROverTimeAdd_cnt++;
        if(IROverTimeAdd_cnt >= 16)   // Լ16ms��û������
        {
            IROverTimeAdd_cnt = 0;
            IRDeodeSta = WaitForGuideTrig1;
        }
        break;
        // λ0 λ1 ������,����Ƿ���볬ʱ
    case Decoding:
        IROverTimeAdd_cnt++;
        if(IROverTimeAdd_cnt >= 5)   // 4ms��û������
        {
            IROverTimeAdd_cnt = 0;
            IRDecodeRes.IRCodeTab.irLen = cnt_Byte;
            cnt_bit = 0;
            cnt_Byte = 0;
            OSALIRDecIntData_t* T4OvfIntData;
            T4OvfIntData = (OSALIRDecIntData_t *)osal_msg_allocate(sizeof(OSALIRDecIntData_t));
            T4OvfIntData->hdr.event = IRDEC_INT_EVENT;
            T4OvfIntData->endPoint = IRDecodeRes.IRIntEp;
            T4OvfIntData->data = &IRDecodeRes.IRCodeTab;
            osal_msg_send(IRDecodeRes.IRIntTskId, (uint8*)(T4OvfIntData));
            IRDeodeSta = WaitForGuideTrig1;
        }
        break;
    default :
        IROverTimeAdd_cnt = 0;
        break;
    }
    T4OVFIF = 0;    // �����ʱ��4����жϱ�־
    T4IF = 0;       // �����ʱ��4���жϱ�־
}
#endif  //#if defined(HAL_IRDEC) && (HAL_IRDEC == TRUE)

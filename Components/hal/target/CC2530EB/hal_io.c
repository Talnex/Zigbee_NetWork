#include "hal_io.h"
#include "OSAL.h"
#include <string.h>

static struct {
    uint8 ioIntTskId;
    uint8 intInUse[MAX_IOGROUP + 1];
    uint8 endPointMap[MAX_IOPORT + 1];
    void *endPointArgMap[MAX_IOPORT + 1];
} ioIntResMap;
void HalIOInit(uint8 taskId)
{
    memset(&ioIntResMap, 0, sizeof(ioIntResMap));
    ioIntResMap.ioIntTskId = taskId;
}
/***********************************************************
**  ��������: HalIOSetInput
**  ʵ�ֹ���: ���ö˿�Ϊ��ͨ����IO
**  ��ڲ���: group:Port;
**            bit:Bit;
**            pull:(Pull_None:��������; Pull_Up:����; Pull_Down:����;);
**  ���ؽ��: IOInt_None
**  ע������: CC2530��ͨ��IO�����������Ƕ������˿ڵ����ã�
**            ����ʵ�ֽ�ͬһ�˿ڵĲ�ͬλ����Ϊ��������ͬ
***********************************************************/
void HalIOSetInput(uint8 group, uint8 bit, PullSet_t pull)
{
    switch(group)
    {
    case 0:
        //����Ϊͨ������IO
        CLRBIT(P0DIR, bit);
        CLRBIT(P0SEL, bit);
        //�����ڲ�����������״̬
        if(Pull_None == pull)
            SETBIT(P0INP, bit);     //P0INP[7-0]:(0:��������Ч; 1:��Ч;)
        else if(Pull_Up == pull)
        {
            CLRBIT(P0INP, bit);     //P0INP[7-0]:(0:��������Ч; 1:��Ч;)
            CLRBIT(P2INP, 5);       //P2INP[5  ]:(0:Port0 ����; 1:Port0 ����;)
        }
        else if(Pull_Down == pull)
        {
            CLRBIT(P0INP, bit);     //P0INP[7-0]:(0:��������Ч; 1:��Ч;)
            SETBIT(P2INP, 5);       //P2INP[5  ]:(0:Port0 ����; 1:Port0 ����;)
        }
        break;
    case 1:
        //����Ϊͨ������IO
        CLRBIT(P1DIR, bit);
        CLRBIT(P1SEL, bit);
        //�����ڲ�����������״̬
        if(Pull_None == pull)
            SETBIT(P1INP, bit);     //P1INP[7-2]:(0:��������Ч; 1:��Ч;) P[1-0] д��Ч����Ϊ0.
        else if(Pull_Up == pull)
        {
            CLRBIT(P1INP, bit);     //P1INP[7-2]:(0:��������Ч; 1:��Ч;) P[1-0] д��Ч����Ϊ0.
            CLRBIT(P2INP, 6);       //P2INP[6  ]:(0:Port1 ����; 1:Port1 ����;)
        }
        else if(Pull_Down == pull)
        {
            CLRBIT(P1INP, bit);     //P1INP[7-2]:(0:��������Ч; 1:��Ч;) P[1-0] д��Ч����Ϊ0.
            SETBIT(P2INP, 6);       //P2INP[6  ]:(0:Port1 ����; 1:Port1 ����;)
        }
        break;
    case 2:
        //����Ϊͨ������IO
        CLRBIT(P2DIR, bit);
        CLRBIT(P2SEL, bit);
        //�����ڲ�����������״̬
        if(Pull_None == pull)
            SETBIT(P2INP, bit);     //P2INP[4-0]:(0:��������Ч; 1:��Ч;)
        else if(Pull_Up == pull)
        {
            CLRBIT(P2INP, bit);     //P2INP[4-0]:(0:��������Ч; 1:��Ч;)
            CLRBIT(P2INP, 7);       //P2INP[7  ]:(0:Port2 ����; 1:Port2 ����;)
        }
        else if(Pull_Down == pull)
        {
            CLRBIT(P2INP, bit);     //P2INP[4-0]:(0:��������Ч; 1:��Ч;)
            SETBIT(P2INP, 7);       //P2INP[7  ]:(0:Port2 ����; 1:Port2 ����;)
        }
        break;
    default:
        break;
    }
}

void HalIOSetOutput(uint8 group, uint8 bit)
{
   switch(group)
   {
   case 0: P0DIR |= (1 << bit); P0SEL &= ~(1 << bit); break;
   case 1: P1DIR |= (1 << bit); P1SEL &= ~(1 << bit); break;
   case 2: P2DIR |= (1 << bit); P2SEL &= ~(1 << bit); break;
   }
}
uint8 HalIOGetLevel(uint8 group, uint8 bit)
{
    switch(group)
    {
    case 0: return !!(P0 & (1 << bit));
    case 1: return !!(P1 & (1 << bit));
    case 2: return !!(P2 & (1 << bit));
    }
    return 0;
}
void HalIOSetLevel(uint8 group, uint8 bit, uint8 value)
{
    switch(group)
    {
    case 0:
        if(value)
            SETBIT(P0, bit);
        else
            CLRBIT(P0, bit);
        break;
    case 1:
        if(value)
            SETBIT(P1, bit);
        else
            CLRBIT(P1, bit);
        break;
    case 2:
        if(value)
            SETBIT(P2, bit);
        else
            CLRBIT(P2, bit);
        break;
    }
}
/***********************************************************
**  ��������: IOIntteruptSet
**  ʵ�ֹ���: ���ö˿��жϴ�����ʽ
**  ��ڲ���: group:Port;
**            bit:Bit;
**            trigger:(IOInt_Rising:�����ش���; IOInt_Falling:�½��ش���;)
**  ���ؽ��: IOInt_None
**  ע������: CC2530��ͨ��IO�жϴ�����ʽ�Ƕ������˿ڵ����ã�
**            ֻ��P1�ڵĸ���λ�͵���λ������ʽ��������Ϊ��ͬ
**            P0�˿ں�P2�˿ڵ����ж˿ڴ�����ʽ�����һ������Ϊ׼��
***********************************************************/
void HalIOIntSet(uint8 endPoint, uint8 group, uint8 bit, IntSel_t trigger, void *arg)
{
    if(HAL_IOPORT(group, bit) > MAX_IOPORT)
        return;
    if(trigger == IOInt_None)
    {
        CLRBIT(ioIntResMap.intInUse[group], bit);
    }
    else
    {
        SETBIT(ioIntResMap.intInUse[group], bit);
        ioIntResMap.endPointMap[HAL_IOPORT(group, bit)] = endPoint;
        ioIntResMap.endPointArgMap[HAL_IOPORT(group, bit)] = arg;
    }
    switch(group)
    {
    case 0:
        if(trigger == IOInt_None)
            CLRBIT(P0IEN, bit);
        else
        {
            SETBIT(P0IEN, bit);
            if(trigger == IOInt_Rising)
                CLRBIT(PICTL, 0);
            else
                SETBIT(PICTL, 0);
        }
        P0IFG = 0x00;       //���P0��Ӧλ�жϱ�־
        P0IF = 0;           //���P0�˿����жϱ�־
        //SETBIT(IEN1, 5);  //P0���ж�����
        CLRBIT(IEN1, 5);    //P0���жϽ�ֹ
        break;
    case 1:
        if(trigger == IOInt_None)
            CLRBIT(P1IEN, bit);
        else
        {
            uint8 ctlBit = (bit <= 3) ? 1 : 2;
            SETBIT(P1IEN, bit);
            if(trigger == IOInt_Rising)
                CLRBIT(PICTL, ctlBit);
            else
                SETBIT(PICTL, ctlBit);
        }
        P1IFG = 0X00;     //���P0��Ӧλ�жϱ�־
        P1IF = 0;         //���P0�˿����жϱ�־
        //SETBIT(IEN2, 4);  //P1���ж�����
        CLRBIT(IEN2, 4);    //P1���жϽ�ֹ
        break;
    case 2:
        if(trigger == IOInt_None)
            CLRBIT(P2IEN, bit);
        else
        {
            SETBIT(P2IEN, bit);
            if(trigger == IOInt_Rising)
                CLRBIT(PICTL, 3);
            else
                SETBIT(PICTL, 3);
        }
        P2IFG = 0X00;     //���P0��Ӧλ�жϱ�־
        P2IF = 0;         //���P0�˿����жϱ�־
        //SETBIT(IEN2, 1);  //P2���ж�����
        CLRBIT(IEN2, 1);    //P2���жϽ�ֹ
        break;
    default :
        break;
    }
}

void HalIOPortPoll()
{
    OSALIOIntData_t* IOIntData;

    uint8 idx;
    uint8 flag = ioIntResMap.intInUse[0] & P0IFG;
    for(idx = 0; flag && (idx < 8); idx++)
    {
        if(BV(idx) & flag)
        {
            IOIntData = (OSALIOIntData_t *)osal_msg_allocate(sizeof(OSALIOIntData_t));
            IOIntData->hdr.event = IOPORT_INT_EVENT;
            IOIntData->endPoint = ioIntResMap.endPointMap[HAL_IOPORT(0, idx)];
            IOIntData->arg = ioIntResMap.endPointArgMap[HAL_IOPORT(0, idx)];
            osal_msg_send(ioIntResMap.ioIntTskId, (uint8*)(IOIntData));
        }
    }
    flag = ioIntResMap.intInUse[1] & P1IFG;
    for(idx = 0; flag && (idx < 8); idx++)
    {
        if(BV(idx) & flag)
        {
            IOIntData = (OSALIOIntData_t *)osal_msg_allocate(sizeof(OSALIOIntData_t));
            IOIntData->hdr.event = IOPORT_INT_EVENT;
            IOIntData->endPoint = ioIntResMap.endPointMap[HAL_IOPORT(1, idx)];
            IOIntData->arg = ioIntResMap.endPointArgMap[HAL_IOPORT(1, idx)];
            osal_msg_send(ioIntResMap.ioIntTskId, (uint8*)(IOIntData));
        }
    }
    flag = ioIntResMap.intInUse[2] & P2IFG;
    for(idx = 0; flag && (idx < 5); idx++)
    {
        if(BV(idx) & flag)
        {
            IOIntData = (OSALIOIntData_t *)osal_msg_allocate(sizeof(OSALIOIntData_t));
            IOIntData->hdr.event = IOPORT_INT_EVENT;
            IOIntData->endPoint = ioIntResMap.endPointMap[HAL_IOPORT(2, idx)];
            IOIntData->arg = ioIntResMap.endPointArgMap[HAL_IOPORT(2, idx)];
            osal_msg_send(ioIntResMap.ioIntTskId, (uint8*)(IOIntData));
        }
    }
    // �ж��¼��������,���Ӳ���жϱ�־λ��
    P0IFG = 0;
    P1IFG = 0;
    P2IFG = 0;
}

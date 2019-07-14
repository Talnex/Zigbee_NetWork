#ifndef __HAL_IO_H__
#define __HAL_IO_H__

#include <ioCC2530.h>
#include "OSAL.h"

#define IOPORT_INT_EVENT            0xF0    //IO���ж��¼�

#define HAL_IOPORT(group, bit)  (((group) << 3) | (bit))
#define MAX_IOGROUP     2
#define MAX_IOPORT      HAL_IOPORT(MAX_IOGROUP, 4)

typedef struct
{
    osal_event_hdr_t hdr;  //�¼����ͼ�״̬
    uint8 endPoint;
    void *arg;
} OSALIOIntData_t;

//�˿���Ϊ����ʱ�ڲ�������ѡ��
typedef enum
{
    Pull_None,  //��̬����
    Pull_Up,    //����ʹ��
    Pull_Down,  //����ʹ��
}PullSet_t;

/***********************************************************
**  CC2530ÿ��ͨ��IO���Ŷ����Բ����жϣ�
**  ��λ�������жϹرգ�������Ҫ�򿪼���
***********************************************************/
typedef enum          //�жϴ�����ʽѡ��
{
    IOInt_None,       //�ر���Ӧ�ж�
    IOInt_Rising,     //�����ش���
    IOInt_Falling,    //�½��ش���
}IntSel_t;

void HalIOInit(uint8 taskId);
//���ö˿�����ģʽ
void HalIOSetInput(uint8 group, uint8 bit, PullSet_t pull);
void HalIOSetOutput(uint8 group, uint8 bit);
uint8 HalIOGetLevel(uint8 group, uint8 bit);
void HalIOSetLevel(uint8 group, uint8 bit, uint8 value);

//���ö˿��жϴ�����ʽ
void HalIOIntSet(uint8 endPoint, uint8 group, uint8 bit, IntSel_t trigger, void *arg);
//��ѯIO�˿��жϱ�־
void HalIOPortPoll(void);

#endif  //__HAL_IO_H__

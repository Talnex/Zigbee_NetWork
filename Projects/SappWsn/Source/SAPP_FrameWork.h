#ifndef _SAPP_FRAMEWORK_H_
#define _SAPP_FRAMEWORK_H_
#include <hal_board.h>
#include "ZComDef.h"
#include "hal_drivers.h"
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"

#include "nwk.h"
#include "APS.h"
#if defined ( ZIGBEE_FRAGMENTATION )
  #include "aps_frag.h"
#endif

#include "MT.h"
#include "MT_UART.h"
#if defined( MT_TASK )
#include "MT_TASK.h"
#endif
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  #include "ZDNwkMgr.h"
#endif

#include "ZDApp.h"
#include "ZComDef.h"
#include "OSAL_Tasks.h"
#include "aps_groups.h"
#include "OnBoard.h"
/* HAL */
#include "hal_led.h"
#include "hal_io.h"
#include "hal_irdec.h"

// These constants are only for example and should be changed to the
// device's needs
#define CONTROL_ENDPOINT             0xF0
#define TRANSFER_ENDPOINT            1

#define SAPP_PROFID                  0x0F08
#define SAPP_DEVICEID                0x0001
#define SAPP_DEVICE_VERSION          0

#define SAPP_PERIODIC_CLUSTERID      1

// Application Events (OSAL) - These are bit weighted definitions.
#define SAPP_SEND_PERIODIC_MSG_EVT   0x0001

enum {
    DevRouter = 240,
    DevCoordinator
};

typedef struct topo_info_t {
    uint8 type;
    uint8 IEEE[8];
    uint16 PAddr;
    uint16 panid;
    uint8 channel;
} TOPOINFO;

typedef struct func_info_t {
    uint8 type;
    uint8 id;
    uint8 cycle;
} FUNCINFO;
typedef union {
    uint8 ft_data[2];
    struct {
        uint8 ft_type;
        uint8 ft_count;
        FUNCINFO ft_list[0];
    } ft_field;
} FUNCTABLE;
#define ft_type ft_field.ft_type
#define ft_count ft_field.ft_count
#define ft_list ft_field.ft_list
#define createFuncTable(count)  (FUNCTABLE *)osal_mem_alloc(sizeof(FUNCTABLE) + count * sizeof(FUNCINFO))
#define destroyFuncTable(ft)    osal_mem_free(ft)

typedef enum {
    ResMinType = 0,
    ResInit,            // ��ʾģ���ʼ��
    ResSerial,
    ResUserTimer,
    ResControlPkg,
    ResIOInt,
    ResTimerInt,
    ResMaxType,
} RES_TYPE;
struct ep_info_t {
    // ����״̬�����仯ʱ����øú���
    void (*nwk_stat_change)(struct ep_info_t *ep);
    // ���յ�����ʱ����øú���
    void (*incoming_data)(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg);
    // �����Ե��õĺ���
    void (*time_out)(struct ep_info_t *ep);
    // ϵͳ��Դ����ʱ���øú���,ϵͳ��Դָ����:���ڽ��յ�����/�жϵȵ�
    void (*res_available)(struct ep_info_t *ep, RES_TYPE type, void *res);
    // NOTE: cycle��Ա�ᱻ��������,�������Ե���time_out����
    struct func_info_t function;

    // ��ǰ�˵��
    uint8 ep;
    // ��˶˵�󶨵�����ID
    uint8 task_id;
    // �ݼ�����,Ϊ0ʱ����time_out����,�����س�ֵ=cycle
    uint8 timerTick;
    uint8 userTimer;
    endPointDesc_t SampleApp_epDesc;
    SimpleDescriptionFormat_t simpleDesc;
};

extern struct ep_info_t funcList[];
extern const uint8 funcCount;

extern void sapp_taskInitProcess(void);
extern uint16 sapp_controlEpProcess(uint8 task_id, uint16 events);
extern UINT16 sapp_functionEpProcess( uint8 task_id, uint16 events );
extern uint8 SendData(uint8 srcEP, const void *buf, uint16 addr, uint8 dstEP, uint8 Len);
extern void CreateUserTimer(struct ep_info_t *ep, uint8 seconds);
extern void DeleteUserTimer(struct ep_info_t *ep);
extern void ModifyRefreshCycle(struct ep_info_t *ep, uint8 seconds);

#if defined(ZDO_COORDINATOR)
#define CoordinatorNwkStateChangeRoutine    NULL
void CoordinatorIncomingRoutine(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg);
void CoordinatorTimeoutRoutine(struct ep_info_t *ep);
#define CoordinatorResAvailableRoutine      NULL
#elif defined(RTR_NWK)
#define RouterNwkStateChangeRoutine         NULL
void  RouterIncomingRoutine(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg);
void RouterTimeoutRoutine(struct ep_info_t *ep);
#define RouterResAvailableRoutine           NULL

#endif
#endif//_SAPP_FRAMEWORK_H_
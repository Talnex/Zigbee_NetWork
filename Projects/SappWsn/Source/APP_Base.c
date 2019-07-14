#include "APP_Base.h"

#if defined(SAPP_ZSTACK_DEMO)
#include "hal_led.h"
// ������ʵ�鷶������
// ��������
uint16 Hello_ProcessEvent(uint8 task_id, uint16 events);
uint16 Hello_ProcessEvent(uint8 task_id, uint16 events)
{
    if(events & 0x0001)
    {
        // ����LED��˸
        HalLedBlink(HAL_LED_1, 1, 50, 250);
        // ������ʱ��, ����1���Ӻ��ٴδ���������
        osal_start_timerEx(task_id, 0x0001, 1000);
    }
    // �����ʱ���¼���־
    return (events ^ 0x0001);
}
#endif

// �����б�
const pTaskEventHandlerFn tasksArr[] = {
    macEventLoop,
    nwk_event_loop,
    Hal_ProcessEvent,
#if defined( MT_TASK )
    MT_ProcessEvent,
#endif
    APS_event_loop,
#if defined ( ZIGBEE_FRAGMENTATION )
    APSF_ProcessEvent,
#endif
    ZDApp_event_loop,
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
    ZDNwkMgr_event_loop,
#endif
#if defined(SAPP_ZSTACK)
    sapp_controlEpProcess,
    sapp_functionEpProcess,
#endif
#if defined(SAPP_ZSTACK_DEMO)
    // ������ʵ�鷶������
    // �����б�
    Hello_ProcessEvent,
#endif
};
const uint8 tasksCnt = sizeof(tasksArr)/sizeof(tasksArr[0]);

// ��ʼ������
void osalInitTasks( void )
{
    uint8 taskID = 0;

    macTaskInit( taskID++ );
    nwk_init( taskID++ );
    Hal_Init( taskID++ );
#if defined( MT_TASK )
    MT_TaskInit( taskID++ );
#endif
    APS_Init( taskID++ );
#if defined ( ZIGBEE_FRAGMENTATION )
    APSF_Init( taskID++ );
#endif
    ZDApp_Init( taskID++ );
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
    ZDNwkMgr_Init( taskID++ );
#endif
#if defined(SAPP_ZSTACK)
    sapp_taskInitProcess();
#endif
#if defined(SAPP_ZSTACK_DEMO)
    // ������ʵ�鷶������
    // ������ʱ��
    osal_start_timerEx(taskID, 0x0001, 1000);
#endif
}

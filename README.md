# 首先先梳理一遍zstack的流程

## 协议栈的文件包层次结构：

-  App：应用层目录，这是用户创建各种不同工程的区域，在这个目录中包含了应用层的内容和这个项目的主要内容，在协议中一般是以操作系统的任务实现的。

- HAL：硬件层目录，包含有与硬件相关的配置和驱动及操作函数

- MAC：MAC层目录，包含了MAC层的参数配置文件及其MAC的LIB库的函数接口文件

- MT：实现通过串口可控制各层，并与各层进行直接交互

- NWK：网络层目录，包含网络层配置参数文件网络层库的函数接口文件及APS层库的函数接口

- OSAL：协议栈的操作系统

- Profile：AF（Applicationframework应用框架）层目录

- Security：安全层目录，包含安全层处理函数，比如加密函数等

- Services：地址处理函数目录，包括地址模式的定义及地址处理函数

- Tools： 工程配置目录，包括空间划分及Z-Stack相关配置信息

- ZDO：ZDO目录

- ZMac：MAC层目录，包括MAC层参数配置及MAC层LIB库函数回调处理函数

- ZMain：主函数目录，包括入口函数及硬件配置文件

- Output：输出文件目录，由IAR IDE自动生成 

## 一些名词

|    英文     |   中文   |                             含义                             | 备注                                                         |
| :---------: | :------: | :----------------------------------------------------------: | ------------------------------------------------------------ |
|  EndPoint   |   端点   | 是协议栈应用层的入口，即入口地址，也可以理解应用对象（Application Object）存在的地方，它是为实现一个设备描述而定义的一组群集 | 端点0 ：用于整个ZigBee设备的配置和管理，附属在端点0的对象被称为ZigBee设备对象(ZD0)<br />端点255：用于向所有的端点进行广播<br />端点241～254：保留端点<br />其他端点：映射应用对象，并使得应用程序可以跟ZigBee堆栈其他层进行通信。 |
|   Cluster   |    簇    |    一个具体的应用（例如智能家居系统）有大量细节上的小规范    | 例如电灯的控制：开灯、关灯等）这个规范即成为簇               |
| COORDINATOR |  协调器  | 协调器是整个网络的核心，它最主要的作用是启动网络，其方法是其方法是选择一个相对空闲的信道，形成一个PANID |                                                              |
|   Router    |  路由器  | 路由器的主要功能是提供接力作用，能扩展信号的传输范围，因此一般情况下应该一直处于活动状态，不应休眠。终端设备可以睡眠也可以唤醒，因此可以用电池来供电。 |                                                              |
|   Channel   |   信道   | 2.4GHz的射频频段被分为16个独立的信道。每一个设备都有一个默认的信道集（DEFAULT_CHANLIST）。协调器扫描自己的默认信道并选择噪声最小的信道作为自己所建的网络信道。设备节点和路由器也要扫描默认信道集并选择信道上已经存在的网络加入。 |                                                              |
|    PANID    | 网络编号 |                                                              | PANID指网络编号，用于区分不同的网络设备，PANID值与ZDAPP_CONFIG_PAN_ID的值设定有关。如果协调器的ZDAPP_CONFIG_PAN_ID设置为0xFFFF,则协调器将产生一个随机的PANID，如果路由器和终端节点的ZDAPP_CONFIG_PAN_ID设置为0xFFFF,路由器和终端节点将会在自己默认信道上随机的选择一个网络加入，网络协调器的PANID即为自己的PANID。如果协调器的ZDAPP_CONFIG_PAN_ID设置为非0xFFFF值，则协调器根据自己的网络长地址（IEEE地址）或ZDAPP_CONFIG_PAN_ID随机产生PANID的值。不同的是如果路由器和终端节点的ZDAPP_CONFIG_PAN_ID 的值设置为非0xFFFF，则会以ZDAPP_CONFIG_PAN_ID值作为PANID。如果协调器的值设为小于等于0x3FFF的有效值，协调器就会以这个特定的PANID值建立网络，但是如果在默认信道上已经有了该PANID值的网络存在，则协调器会继续搜寻其它的PANID，直到找到不冲突的网络为止，这样就可能产生一个问题如果协调器在默认信道上发生PANID冲突而更换PANID,终端节点并不知道协调器已经更换了PANID，还会继续加入到PANID为ZDAPP_CONFIG_PAN_ID值的网络中 |

在main函数中：

``` c
int main( void )
{
  // Turn off interrupts
 //关闭中断
 osal_int_disable( INTS_ALL );
  //初始化硬件
 // Initialization for board related stuff such as LEDs
 HAL_BOARD_INIT();
  // Make sure supply voltage ishigh enough to run
  //电压检测，确保芯片能正常工作的电压
 zmain_vdd_check();
  // Initialize board I/O
 //初始化板载I/O
 InitBoard( OB_COLD );
  // Initialze HAL drivers
 //初始化硬件驱动
 HalDriverInit();
  // Initialize NV System
 //初始化NV系统
 osal_nv_init( NULL );
  // Initialize the MAC
 //初始化MAC
 ZMacInit();
 // Determine the extended address
 //确定扩展地址（64位IEEE/物理地址）
 zmain_ext_addr();
#if defined ZCL_KEY_ESTABLISH
  // Initialize the Certicom certificate information.
  // 初始化CERT认证系统
 zmain_cert_init();
#endif
  // Initialize basic NVitems
 //初始化基本NV条目
 zgInit();
#ifndef NONWK
  //Since the AF isn't a task, call it's initialization routine
  //如果task里没有AF任务，需要在此调用他的初始化函数
 afInit();
#endif
 // Initialize the operating system
 //初始化操作系统
 osal_init_system();
 ...
 }

```
其中的osal_init_system便是操作系统的初始化，里面包含了os中各个app以及os各组成部分的初始化方法的调用，比如：初始化内存，堆栈等，其中对app的初始化函数为osalInitTasks();
``` c
uint8 osal_init_system( void )
{
  // Initialize the Memory Allocation System
  //初始化内存分配系统
 osal_mem_init();
  // Initialize the message queue
 //初始化消息队列  任务之间的通信靠的就是消息队列
 osal_qHead = NULL;
  // Initialize the timers
 //初始化定时器
 osalTimerInit();
  // Initialize the Power Management System
 //初始化电源管理系统
 osal_pwrmgr_init();
  
  //osal_mem_alloc()该函数是OSAL中的内存管理函数，是一个存储分配函数，返回指向一个缓存的指针，参数是被分配缓存的大小，其tasksCnt的定义如下const uint8tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );tasksEvents指向被分配的内存空间，这里注意tasksArr[]函数指针数组的联系是一一对应的。tasksEvents就是指向第一个被分配的任务的内存空间
 tasksEvents = (uint16 *)osal_mem_alloc( sizeof( uint16 ) * tasksCnt);

  //把申请的内存空间全部设置为0，tasksCnt任务数 * 单个任务占的内存空间（4byte）
 osal_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));

  // Initialize the system tasks.
 //初始化系统任务，重点关注
 osalInitTasks();
 
  // Setup efficient search for the first free block of heap.
 //设置有效的查找堆上的第一个空闲块
 osal_mem_kick();
 return ( SUCCESS );
}
```
任务初始化函数-------osalInitTasks();
``` c
void osalInitTasks( void )
{
 uint8 taskID = 0;
 //下面就是Z-Stack协议栈中，从MAC层到ZDO层的初始化函数，其中的参数都是任务的ID，不过ID号是依次递增的
 macTaskInit(taskID++ ); //mac_ID = 0
 nwk_init( taskID++ ); //nwk_ID = 1
 Hal_Init( taskID++ ); //Hal_ID = 2
#if defined( MT_TASK )
 MT_TaskInit( taskID++ );//mt_ID = 3
#endif
 APS_Init( taskID++ ); //APS_ID =4
#if defined ( ZIGBEE_FRAGMENTATION )
  APSF_Init(taskID++ ); //ZDO_ID =5
#endif
 ZDApp_Init( taskID++ ); ;//ZDO_ID =6
#if defined ( ZIGBEE_FREQ_AGILITY ) ||defined ( ZIGBEE_PANID_CONFLICT )
 ZDNwkMgr_Init( taskID++ ); //ZDO_ID =7
#endif
//协议栈工程下如果选择Coordinator或EndDevice或Router工程则只会进入这个
#if defined(SAPP_ZSTACK)
    sapp_taskInitProcess();//ZDO_ID =8
#endif
//协议栈工程下如果选择Deemo工程则只会进入这个
#if defined(SAPP_ZSTACK_DEMO)
    // 任务建立实验范例代码
    // 启动定时器
    osal_start_timerEx(taskID, 0x0001, 1000);//ZDO_ID =8
#endif
}
```
追踪到sapp_taskInitProcess，这里便是app的初始化函数了，在这个协议栈中，app只有一个，但这个app是一个管理很多小功能的APP，所有的小功能被放在functionlist里统一管理
``` c
void sapp_taskInitProcess(void)
{
 #if defined ( BUILD_ALL_DEVICES )
    // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
    // We are looking at a jumper (defined in SampleAppHw.c) to be jumpered
    // together - if they are - we will start up a coordinator. Otherwise,
    // the device will start as a router.
    if ( readCoordinatorJumper() )
        zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
    else
        zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
    // HOLD_AUTO_START is a compile option that will surpress ZDApp
    //  from starting the device and wait for the application to
    //  start the device.
    ZDOInitDevice(0);
#endif

    // 构造功能列表
    funcTableBuffer = createFuncTable(funcCount);
    funcTableBuffer->ft_type = 0x01;
    funcTableBuffer->ft_count = funcCount;
    int i;
    for(i = 0; i < funcCount; i++)
    {
        funcTableBuffer->ft_list[i].type = funcList[i].function.type;
        funcTableBuffer->ft_list[i].id = funcList[i].function.id;
        funcTableBuffer->ft_list[i].cycle = funcList[i].function.cycle;
    }
    controlTaskId = tasksCnt - 2;
    functionTaskId = tasksCnt - 1;
    HalIOInit(functionTaskId);
    createEndPoint(&controlEndPointInfo, &controlTaskId, CONTROL_ENDPOINT);
    for(i = 0; i < funcCount; i++)
    {
        struct ep_info_t *ep = &funcList[i];
        createEndPoint(ep, &functionTaskId, i + 1);
        if(ep->res_available)
          (*ep->res_available)(ep, ResInit, NULL);
    }
//这里选择是否注册串口事件
#if defined(ZDO_COORDINATOR)// || defined(RTR_NWK)
//    RegisterForKeys( SampleApp_TaskID );
    MT_UartRegisterTaskID(controlTaskId);
#endif
}

```

好了所有的初始化任务完成了，现在回到main中,再经过几个初始化后就进入osal_start_system开始正式启动OS了

``` c
...
 // Initialize the operating system
 //初始化操作系统
 osal_init_system();
  // Allow interrupts
 //使能中断
 osal_int_enable( INTS_ALL );
  // Final board initialization
 //最终板载初始化
 InitBoard( OB_READY );
  // Display informationabout this device
 //显示设备信息
 zmain_dev_info();
  /*Display the device info on the LCD */
  //添加LCD液晶屏的支持
#ifdef LCD_SUPPORTED
 zmain_lcd_init();
#endif
#ifdef WDT_IN_PM1
  /*If WDT is used, this is a good place to enable it. */
 //看门狗的初始化设置
 WatchDogEnable( WDTIMX );
#endif
  osal_start_system(); // No Return from here没有返回，即进入操作系统
 return 0;  // Shouldn't get here.//不会运行到这
```
进入osal_start_system发现这里有个死循环，也就是为什么main函数不return的原因
``` c
void osal_start_system( void )
{
#if !defined ( ZBIT ) && !defined (UBIT )
     for(;;) // 一直循环，是“轮询”中的轮，即不断循环执行
#endif
     {
          osal_run_system();
     }
}
```
进入osal_run_system中，osal_start_system函数是ZigBee协议栈的灵魂，实现的方法是不断查询事件表，如果有事情发生就调用相应的事件处理函数。
``` c
void osal_run_system( void )
{
  uint8 idx = 0;
  osalTimeUpdate();
  Hal_ProcessPoll();

  do {
    if (tasksEvents[idx])  // 这里就是“轮询”中的询，即不断查询，而且这个查询是有优先级顺序的
    {
      break;//如果有事件发生则跳出循环
    }
  } while (++idx < tasksCnt);

//判断是主动跳出循环还是全部判断完跳出的循环
  if (idx < tasksCnt)
  {
    uint16 events;
    halIntState_t intState;

    HAL_ENTER_CRITICAL_SECTION(intState);
    events = tasksEvents[idx];
    tasksEvents[idx] = 0;  // 清除事件标志
    HAL_EXIT_CRITICAL_SECTION(intState);

    activeTaskID = idx;
    events = (tasksArr[idx])( idx, events );//调用相应的事件处理函数，tasksArr[]是一个函数指针数组，每一个元素都是函数指针
    activeTaskID = TASK_NO_TASK;

    HAL_ENTER_CRITICAL_SECTION(intState);
    tasksEvents[idx] |= events;  // 如果没有处理完可以再次设置标志
    HAL_EXIT_CRITICAL_SECTION(intState);
  }
#if defined( POWER_SAVING )
  else  // Complete pass through all task events with no activity?
  {
    osal_pwrmgr_powerconserve();  // Put the processor/system into sleep
  }
#endif

  /* Yield in case cooperative scheduling is being used. */
#if defined (configUSE_PREEMPTION) && (configUSE_PREEMPTION == 0)
  {
    osal_task_yield();
  }
#endif
}
```
这里可以去看我们的taskArr里有什么了
``` c
// 任务列表
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
    //重点，这两个便是我们自己定义的事件处理函数
    sapp_controlEpProcess,
    sapp_functionEpProcess,
#endif
#if defined(SAPP_ZSTACK_DEMO)
    // Deemo工程对应的事件处理函数
    Hello_ProcessEvent,
#endif
};
//学习一下这里sizeof的用法，之后会经常用到
const uint8 tasksCnt = sizeof(tasksArr)/sizeof(tasksArr[0]);
```

这里的taskArr（包含系统级任务如macEventLoop等和用户app如sapp_functionEpProcess）非常像我们平常使用的操作系统里的任务管理器里的任务（图中包含了系统自带应用如windowServer何kernel_task等，也包括了用户app如QQ），对于我们的OS来说，多个网络应用是通过端口号（图中如果有一份数据包发给QQ则会发送给0.0.0.0:1194）来区分的，而对于OSAL来说，接收到一份数据包决定将数据包发送给哪个app则是通过数据包中指定的EndPoint决定的。

![屏幕快照 2019-07-15 09.54.06](https://i.loli.net/2019/07/15/5d2bdd25358af70922.png)

比如这一份数据包，就会被发送到EndPoint为0x01的task里。

![屏幕快照 2019-07-15 10.12.28.png](https://i.loli.net/2019/07/15/5d2be13032b1d13971.png)

进入第一个app的处理函数sapp_controlEpProcess，这里主要是为了配合zigbee调试助手使用，原来是让协调器接收PC中的ZigBee调试助手里发送的命令帧，以及解析命令帧里命令，01对应发送functionlist，02对应发送拓扑信息，03对应发送function的数据？关于zigbee调试助手的原理在最后面会讲一下。

``` c
uint16 sapp_controlEpProcess(uint8 task_id, uint16 events)
{
    afIncomingMSGPacket_t *MSGpkt;
   //如果是系统事件
    if ( events & SYS_EVENT_MSG )
    {
        //message要通过osal_msg_send发送也要对应通过osal_msg_receive接收
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(task_id);
        while ( MSGpkt )
        {
            //HalUARTWrite(0, &MSGpkt->hdr.event,1);
            switch ( MSGpkt->hdr.event )
            {
#if defined(ZDO_COORDINATOR)
            //如果是串口事件
            case CMD_SERIAL_MSG:
                uartMsgProcesser((uint8 *)MSGpkt);
                HalLedBlink( HAL_LED_1, 2, 50, 90 );
                break;
#endif

#if ! defined(ZDO_COORDINATOR) && defined(RTR_NWK)
            //如果是串口事件
            case CMD_SERIAL_MSG:
                uartMsgProcesser1((uint8 *)MSGpkt);
                HalLedBlink( HAL_LED_1, 2, 50, 90 );
                break;
#endif
                // Received when a messages is received (OTA) for this endpoint
            //如果收到了无线数据包
            case AF_INCOMING_MSG_CMD:
            {
                // TODO: QueryProfile or QueryTopo
               //先判断是不是跟自己一个cluster
                switch(MSGpkt->clusterId)
                {
                case SAPP_PERIODIC_CLUSTERID:
                    switch(MSGpkt->cmd.Data[0])
                    {
                    case 0x01:
                        // CtrlQueryProfile
                        //这个也是配合zigbee调试助手使用的，目的是显示functionlist里有几个function
                        SendData(CONTROL_ENDPOINT, funcTableBuffer->ft_data, MSGpkt->srcAddr.addr.shortAddr, MSGpkt->srcAddr.endPoint, sizeof(FUNCTABLE) + funcCount * sizeof(FUNCINFO));
                        break;
                    case 0x02:
                        // CtrlQueryTopo
                        //如果是查询拓扑信息的命令帧则把自己的拓扑数据发送给源地址(也就是协调器)这个是配合zigbee调试助手显示拓扑结构功能使用的，原理在之后会讲
                        SendData(CONTROL_ENDPOINT, (unsigned char *)&topoBuffer, MSGpkt->srcAddr.addr.shortAddr, MSGpkt->srcAddr.endPoint, sizeof(TOPOINFO));
                        break;
                    case 0x03:
                        // CtrlQuerySpecialFunction
                        //也是配合zigbee调试助手使用的，应该是发送function里的数据
                        {
                            uint8 i;
                            for(i = 0; i < funcTableBuffer->ft_count; i++)
                            {
                                if((funcTableBuffer->ft_list[i].type == MSGpkt->cmd.Data[1])
                                   && (funcTableBuffer->ft_list[i].id == MSGpkt->cmd.Data[2]))
                                {
                                    // 0x03, EndPoint, rCycle
                                    uint8 specialFunc[3] = { 0x03, i + 1, funcTableBuffer->ft_list[i].cycle };
                                    SendData(CONTROL_ENDPOINT, specialFunc, MSGpkt->srcAddr.addr.shortAddr, MSGpkt->srcAddr.endPoint, sizeof(specialFunc));
                                    break;
                                }
                            }
                        }
                        break;
                    default:
                        {
                            int i;
                            for(i = 0; i < funcCount; i++)
                            {
                                struct ep_info_t *ep = &funcList[i];
                                if(ep->res_available)   (*ep->res_available)(ep, ResControlPkg, MSGpkt);//执行对应的资源可用函数
                            }
                        }
                        break;
                    }
                    HalLedBlink( HAL_LED_2, 1, 50, 250 );
                    break;
                }
                break;
            }
            // Received whenever the device changes state in the network
            //如果是网络状态改变信息
            case ZDO_STATE_CHANGE:
            {
                devStates_t st = (devStates_t)(MSGpkt->hdr.status);
                if ( (st == DEV_ZB_COORD)
                        || (st == DEV_ROUTER)
                        || (st == DEV_END_DEVICE) )
                {
//                    topoBuffer->type = 0x02;
                  //改变自己存储的拓扑结构数据
                    memcpy(topoBuffer.IEEE, NLME_GetExtAddr(), 8);
#if !defined(ZDO_COORDINATOR)
                    topoBuffer.PAddr = NLME_GetCoordShortAddr();
#else
                    topoBuffer.PAddr = 0xFFFF;
#endif
                    osal_memcpy(&topoBuffer.panid, &_NIB.nwkPanId, sizeof(uint16));
                    osal_memcpy(&topoBuffer.channel, &_NIB.nwkLogicalChannel, sizeof(uint8));
                    //向协调器发送拓扑信息
                    SendData(CONTROL_ENDPOINT, (unsigned char *)&topoBuffer, 0x0000, TRANSFER_ENDPOINT, sizeof(TOPOINFO));
                    HalLedBlink( HAL_LED_2, 4, 50, 250 );
                }
            }
            break;
            default:
                break;
            }
            // Release the memory
            osal_msg_deallocate( (uint8 *)MSGpkt );
            // Next - if one is available
            MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( task_id );
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    // 定时器时间到, 遍历所有端点看是否有userTimer
    if(events & SAPP_SEND_PERIODIC_MSG_EVT)
    {
        int i;
        uint8 hasUserTimer = 0;
        for(i = 0; i < funcCount; i++)
        {
            struct ep_info_t *ep = &funcList[i];
            if(ep->userTimer && ep->res_available)
            {
                hasUserTimer = 1;
                ep->userTimer = ep->userTimer - 1;
                if(ep->userTimer <= 1)
                {
                    ep->userTimer = 0;
                    (*ep->res_available)(ep, ResUserTimer, NULL);//执行对应的超时函数
                }
            }
        }
        if(hasUserTimer)
        {
            // 重新启动定时器
            osal_start_timerEx(task_id, SAPP_SEND_PERIODIC_MSG_EVT, 1000);
        }
        else
        {
            isUserTimerRunning = 0;
            osal_stop_timerEx(task_id, SAPP_SEND_PERIODIC_MSG_EVT);
        }
        // return unprocessed events
        return (events ^ SAPP_SEND_PERIODIC_MSG_EVT);
    }
    // Discard unknown events
    return 0;
}

```

最后进入sapp_functionEpProcess看看，这里主要是根据事件执行的functionlist里对应的定义的那些函数

``` c
uint16 sapp_functionEpProcess(uint8 task_id, uint16 events)
{
    afIncomingMSGPacket_t *MSGpkt;
    if(events & SYS_EVENT_MSG)
    {
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( task_id );
        while ( MSGpkt )
        {
            switch ( MSGpkt->hdr.event )
            {
            // 接收到数据包
            case AF_INCOMING_MSG_CMD:
                {
                    switch ( MSGpkt->clusterId )
                    {
                    case SAPP_PERIODIC_CLUSTERID:
                        if(MSGpkt->endPoint <= funcCount)
                        {
                            struct ep_info_t *ep = &funcList[MSGpkt->endPoint - 1];
                            if(ep->incoming_data)
                                (*ep->incoming_data)(ep, MSGpkt->srcAddr.addr.shortAddr, MSGpkt->srcAddr.endPoint, &MSGpkt->cmd);//执行对应的incoming处理函数
                            HalLedBlink( HAL_LED_2, 1, 50, 250 );
                        }
                        
                        break;
                    }
                }
                break;

            case ZDO_STATE_CHANGE:
                {
                    curNwkState = (devStates_t)(MSGpkt->hdr.status);
                    if ( (curNwkState == DEV_ZB_COORD)
                            || (curNwkState == DEV_ROUTER)
                            || (curNwkState == DEV_END_DEVICE) )
                    {
                        int i;
                        int hasTimeOut = 0;
                        for(i = 0; i < funcCount; i++)
                        {
                            struct ep_info_t *ep = &funcList[i];
                            if(ep->nwk_stat_change)
                                (*ep->nwk_stat_change)(ep);
                            // 重置端点计数器
                            if(ep->time_out && ep->function.cycle)
                            {
                                ep->timerTick = ep->function.cycle;
                                hasTimeOut = 1;
                            }
                        }
                        if(hasTimeOut)
                        {
                            // 加入网络成功,启动定时器,为各个端点提供定时
                            osal_start_timerEx(task_id,
                                               SAPP_SEND_PERIODIC_MSG_EVT,
                                               1000);
                        }
                    }
                    else
                        osal_stop_timerEx(task_id, SAPP_SEND_PERIODIC_MSG_EVT);
                }
                break;
            //IO事件
            case IOPORT_INT_EVENT:
              {
                OSALIOIntData_t* IOIntData;
                IOIntData =(OSALIOIntData_t*)MSGpkt;
                if(IOIntData->endPoint <= funcCount)
                {
                    struct ep_info_t *ep = &funcList[IOIntData->endPoint - 1];
                    if(ep->res_available)
                        (*ep->res_available)(ep, ResIOInt, IOIntData->arg);//执行对应的资源可用处理函数
                }
              }
              break;
#if defined(HAL_IRDEC) && (HAL_IRDEC == TRUE)
            case IRDEC_INT_EVENT:   //
              {
                OSALIRDecIntData_t* TimerIntData = (OSALIRDecIntData_t*)MSGpkt;
                if(TimerIntData->endPoint <= funcCount)
                {
                    struct ep_info_t *ep = &funcList[TimerIntData->endPoint - 1];
                    if(ep->res_available)
                        (*ep->res_available)(ep, ResTimerInt, TimerIntData->data);
                }
              }
              break;
#endif
            default:
                break;
            }
            // Release the memory
            osal_msg_deallocate( (uint8 *)MSGpkt );
            // Next - if one is available
            MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( task_id );
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    // 定时器时间到, 遍历所有端点看是否有需要调用time_out
    if(events & SAPP_SEND_PERIODIC_MSG_EVT)
    {
        int i;
        for(i = 0; i < funcCount; i++)
        {
            struct ep_info_t *ep = &funcList[i];
            if(ep->time_out && ep->function.cycle)
            {
                // 端点需要周期执行
                ep->timerTick = ep->timerTick - 1;
                if(ep->timerTick == 0)
                {
                  // 定时时间到,执行time_out函数
                  (*ep->time_out)(ep);
                  ep->timerTick = ep->function.cycle;
                }
            }
#if 0
            if(ep->userTimer && ep->res_available)
            {
                ep->userTimer = ep->userTimer - 1;
                if(ep->userTimer <= 1)
                {
                    (*ep->res_available)(ep, ResUserTimer, NULL);
                    ep->userTimer = 0;
                }
            }
#endif
        }
        // 重新启动定时器
        osal_start_timerEx(task_id, SAPP_SEND_PERIODIC_MSG_EVT, 1000);
        // return unprocessed events
        return (events ^ SAPP_SEND_PERIODIC_MSG_EVT);
    }
    // Discard unknown events
    return 0;
}

```

# Zigbee 串口事件

UART(通用串口)是由zigbee协议栈中的MT层主管的，因此要在预编译选项中添加上ZTOOL_P1或ZAPP_P1和MT_TASK

相关的文件有：

- MT_UART.c
- MT_UART.h

略过硬件层的一些关于串口的初始化后，我们又回到了OSAL的初始化函数，在这里面有个非常重要的函数MT_TaskInit，这里面是对MT层的一系列设置。

```c
void osalInitTasks( void )
{
 uint8 taskID = 0;
 //下面就是Z-Stack协议栈中，从MAC层到ZDO层的初始化函数，其中的参数都是任务的ID，不过ID号是依次递增的
 macTaskInit(taskID++ ); //mac_ID = 0
 nwk_init( taskID++ ); //nwk_ID = 1
 Hal_Init( taskID++ ); //Hal_ID = 2
#if defined( MT_TASK )
 MT_TaskInit( taskID++ );//mt_ID = 3
 ...
```

追踪进去后：

``` c
void MT_TaskInit(uint8 task_id)
{
  MT_TaskID = task_id;

  /* Initialize the Serial port */
  //初始化Uart
  MT_UartInit();

  /* Register taskID - Do this after UartInit() because it will reset the taskID */
  //注册事件
  MT_UartRegisterTaskID(task_id);

  osal_set_event(task_id, MT_SECONDARY_INIT_EVENT);
}

```

其中的UartInit中

``` c
void MT_UartInit ()
{
  halUARTCfg_t uartConfig;

  /* Initialize APP ID */
  App_TaskID = 0;

  /* UART Configuration */
  //设置为已配置
  uartConfig.configured           = TRUE;
  //设置为默认波特率115200
  uartConfig.baudRate             = MT_UART_DEFAULT_BAUDRATE;
  //设置是否采用流控及流控设置
  uartConfig.flowControl          = MT_UART_DEFAULT_OVERFLOW;
  uartConfig.flowControlThreshold = MT_UART_DEFAULT_THRESHOLD;
  //设置最大发送缓冲区长度128
  uartConfig.rx.maxBufSize        = MT_UART_DEFAULT_MAX_RX_BUFF;
  //设置最大接收缓冲区长度128
  uartConfig.tx.maxBufSize        = MT_UART_DEFAULT_MAX_TX_BUFF;
  //设置超时时间
  uartConfig.idleTimeout          = MT_UART_DEFAULT_IDLE_TIMEOUT;
  uartConfig.intEnable            = TRUE;
#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  //重要：设置串口回调函数
  uartConfig.callBackFunc         = MT_UartProcessZToolData1;
#elif defined (ZAPP_P1) || defined (ZAPP_P2)
  uartConfig.callBackFunc         = MT_UartProcessZAppData;
#else
  uartConfig.callBackFunc         = NULL;
#endif

  /* Start UART */
#if defined (MT_UART_DEFAULT_PORT)
  //开启UART
  HalUARTOpen (MT_UART_DEFAULT_PORT, &uartConfig);
#else
  /* Silence IAR compiler warning */
  (void)uartConfig;
#endif

  /* Initialize for ZApp */
#if defined (ZAPP_P1) || defined (ZAPP_P2)
  /* Default max bytes that ZAPP can take */
  MT_UartMaxZAppBufLen  = 1;
  MT_UartZAppRxStatus   = MT_UART_ZAPP_RX_READY;
#endif

}
```

我们的

``` c
//自定义的串口接收回调函数
void MT_UartProcessZToolData1 ( uint8 port, uint8 event )
{

  uint8 flag = 0,i,j = 0;  //flag判断有没有数据，j记录数据长度
  uint8 buf[128];         //缓冲128
  (void)event;

  while(Hal_UART_RxBufLen(port)) //检测串口数据是否完成
  {
    HalUARTRead(port,&buf[j],1);//数据接收到buf
    j++;
    flag = 1;
  }
  if(flag == 1)   //有数据时
  {//分配内存，结构体+内容+长度
    pMsg = (mtOSALSerialData_t *)osal_msg_allocate( sizeof
                                                   ( mtOSALSerialData_t )+j+1);
    pMsg->hdr.event = CMD_SERIAL_MSG;
    pMsg->msg = (uint8*)(pMsg+1); //把数据定位到结构体
    pMsg->msg [0]= j;             //记录数据长度
    for(i=0;i<j;i++)
      pMsg->msg [i+1]= buf[i];
    osal_msg_send( App_TaskID, (byte *)pMsg ); // 登记任务并发往上层
    osal_msg_deallocate ( (uint8 *)pMsg ); // 释放内存
  }
}

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
/***************************************************************************************************
 * @fn      MT_UartProcessZToolData
 *
 * @brief   | SOP | CMD |Data Length| cmdEP | Address | EndPoint |  Data  | FSC |
 *          |  1  |  2  |    1      |   2   |    2    |     1    | 1 ~119 |  1  |
 *
 *          Parses the data and determine either is SPI or just simply serial data
 *          then send the data to correct place (MT or APP)
 *
 * @param   port     - UART port
 *          event    - Event that causes the callback
 *
 *
 * @return  None
 ***************************************************************************************************/
void MT_UartProcessZToolData ( uint8 port, uint8 event )
{
    uint8  ch;
    uint8  bytesInRxBuffer;

    (void)event;  // Intentionally unreferenced parameter

    while (Hal_UART_RxBufLen(port))
    {
        HalUARTRead (port, &ch, 1);

        switch(state)
        {
        case SOP_STATE:
            if(ch == MT_UART_SOF)
                state = LEN_STATE;
            break;
        case LEN_STATE:
            if(ch < 7)
            {
                // invalid length field
                state = SOP_STATE;
                break;
            }
            pMsg = (mtOSALSerialData_t *)osal_msg_allocate(sizeof(mtOSALSerialData_t) +
                                                           ch + 3);//SOP+LEN+FSC
            tempDataLen = 0;
            /* Allocate memory for the data */
//            pMsg = (mtOSALSerialData_t *)osal_msg_allocate(sizeof(mtOSALSerialData_t) + sizeof(mtUserSerialMsg_t) +
//                                                           ch - 5);
            if (pMsg)
            {
                /* Fill up what we can */
                pMsg->hdr.event = CMD_SERIAL_MSG;
                pMsg->msg = (uint8*)(pMsg + 1);
                pMsgContent = (mtUserSerialMsg_t *)pMsg->msg;
                pMsgContent->sop = MT_UART_SOF;
                pMsgContent->len = ch;
                state = DATA_STATE;
            }
            else
            {
                pMsgContent = NULL;
                state = SOP_STATE;
                return;
            }
            break;
        case DATA_STATE:
            pMsgContent->dataBody[tempDataLen++] = ch;
            /* Check number of bytes left in the Rx buffer */
            bytesInRxBuffer = Hal_UART_RxBufLen(port);

            /* If the remain of the data is there, read them all, otherwise, just read enough */
            if (bytesInRxBuffer <= pMsgContent->len - tempDataLen)
            {
                HalUARTRead (port, &pMsgContent->dataBody[tempDataLen], bytesInRxBuffer);
                tempDataLen += bytesInRxBuffer;
            }
            else
            {
                HalUARTRead (port, &pMsgContent->dataBody[tempDataLen], pMsgContent->len - tempDataLen);
                tempDataLen += (pMsgContent->len - tempDataLen);
            }
            /* If number of bytes read is equal to data length, time to move on to FCS */
            if ( tempDataLen == pMsgContent->len )
                state = FCS_STATE;
            break;
        case FCS_STATE:
            /* Make sure it's correct */
            {
                pMsgContent->fsc = ch;
                uint8 fcs = MT_UartCalcFCS(0, &pMsgContent->len, 1);
                fcs = MT_UartCalcFCS(fcs, pMsgContent->dataBody, pMsgContent->len);
                if(fcs == ch)
                    osal_msg_send(App_TaskID, (byte *)pMsg);
                else
                    osal_msg_deallocate((uint8 *)pMsg);
            }
            /* Reset the state, send or discard the buffers at this point */
            state = SOP_STATE;
            break;
        default:
            break;
        }
    }
}
```




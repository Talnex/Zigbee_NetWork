#ifndef _SAPP_DEVICE_H_
#define _SAPP_DEVICE_H_
#include "SAPP_FrameWork.h"
#ifdef __cplusplus
extern "C"
{
#endif

// ��������ֵ����
enum {
    DevTemp = 1,                    // �����¶�
    DevHumm,                        // ����ʪ��
    DevILLum,                       // ���ն�
    DevRain,                        // ���
    DevIRDist,                      // ������
    DevGas,                         // ȼ��
    DevSmoke,                       // ����
    DevFire,                        // ����
    DevIRPers,                      // �������
    DevVoice,                       // ����ʶ��
    DevExecuteB,                    // ���������ִ����
    DevExecuteA,                    // ģ�������ִ����
    DevRemoter,                     // ����ң��
    Dev125kReader,                  // 125kHz������
    DevSpeaker,                     // ��������
    DevTest,                        // ���ܲ���
    DevBroadcastSend,               // �㲥����
    DevBroadcastReceive,            // �㲥����
    DevIRDecode,                    // ����ң�ؽ���
    DevMaxNum,
};

#if !defined( ZDO_COORDINATOR ) && !defined( RTR_NWK )
// �ڵ㹦�ܶ���
//#define HAS_GAS                   // ��˹������
//#define HAS_TEMP                  // �¶ȴ�����
//#define HAS_HUMM                  // ʪ�ȴ�����
//#define HAS_RAIN                  // ��δ�����
//#define HAS_FIRE                  // ���洫����
//#define HAS_SMOKE                 // ��������
//#define HAS_ILLUM                 // ���նȴ�����
//#define HAS_IRPERS                // ������⴫����
//#define HAS_IRDIST                // �����ഫ����
//#define HAS_VOICE                 // ����������, �޸� HAL_UART_DMA �Ķ���Ϊ2
//#define HAS_EXECUTEB              // ִ����
//#define HAS_EXECUTEA              // ģ��ִ����(Ԥ����չ)
//#define HAS_REMOTER               // ����ң��(Ԥ����չ)
//#define HAS_TESTFUNCTION          // ���⹦��
//#define HAS_BROADCASTSEND         // �㲥����
//#define HAS_BROADCASTRECEIVE      // �㲥����
//#define HAS_125KREADER            // 125K���ӱ�ǩ�Ķ���
//#define HAS_SPEAKER               // ����������
//#define HAS_IRDecode              // �������
#endif

#ifdef __cplusplus
}
#endif
#endif//_SAPP_DEVICE_H_

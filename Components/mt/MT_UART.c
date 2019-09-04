/***************************************************************************************************
  Filename:       MT_UART.c
  Revised:        $Date: 2009-03-12 16:25:22 -0700 (Thu, 12 Mar 2009) $
  Revision:       $Revision: 19404 $

  Description:  This module handles anything dealing with the serial port.

  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.

***************************************************************************************************/

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "ZComDef.h"
#include "OSAL.h"
#include "hal_uart.h"
#include "MT.h"
#include "MT_UART.h"
#include "OSAL_Memory.h"


/***************************************************************************************************
 * MACROS
 ***************************************************************************************************/

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/
/* State values for ZTool protocal */
#define SOP_STATE      0x00
#define LEN_STATE      0x01
#define DATA_STATE     0x02
#define FCS_STATE      0x03

/***************************************************************************************************
 *                                         GLOBAL VARIABLES
 ***************************************************************************************************/
/* Used to indentify the application ID for osal task */
uint8 App_TaskID;

/* ZTool protocal parameters */
uint8 state = SOP_STATE;
mtOSALSerialData_t  *pMsg;
mtUserSerialMsg_t   *pMsgContent;
uint8  tempDataLen;

#if defined (ZAPP_P1) || defined (ZAPP_P2)
uint16  MT_UartMaxZAppBufLen;
bool    MT_UartZAppRxStatus;
#endif


/***************************************************************************************************
 *                                          LOCAL FUNCTIONS
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      MT_UartInit
 *
 * @brief   Initialize MT with UART support
 *
 * @param   None
 *
 * @return  None
***************************************************************************************************/
void MT_UartInit ()
{
  halUARTCfg_t uartConfig;

  /* Initialize APP ID */
  App_TaskID = 0;

  /* UART Configuration */
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_9600;
  uartConfig.flowControl          = MT_UART_DEFAULT_OVERFLOW;
  uartConfig.flowControlThreshold = MT_UART_DEFAULT_THRESHOLD;
  uartConfig.rx.maxBufSize        = MT_UART_DEFAULT_MAX_RX_BUFF;
  uartConfig.tx.maxBufSize        = MT_UART_DEFAULT_MAX_TX_BUFF;
  uartConfig.idleTimeout          = MT_UART_DEFAULT_IDLE_TIMEOUT;
  uartConfig.intEnable            = TRUE;
#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  uartConfig.callBackFunc         = MT_UartProcessZToolData1;
#elif defined (ZAPP_P1) || defined (ZAPP_P2)
  uartConfig.callBackFunc         = MT_UartProcessZAppData;
#else
  uartConfig.callBackFunc         = NULL;
#endif

  /* Start UART */
#if defined (MT_UART_DEFAULT_PORT)
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

/***************************************************************************************************
 * @fn      MT_SerialRegisterTaskID
 *
 * @brief   This function registers the taskID of the application so it knows
 *          where to send the messages whent they come in.
 *
 * @param   void
 *
 * @return  void
 ***************************************************************************************************/
void MT_UartRegisterTaskID( byte taskID )
{
  App_TaskID = taskID;
}

/***************************************************************************************************
 * @fn      SPIMgr_CalcFCS
 *
 * @brief   Calculate the FCS of a message buffer by XOR'ing each byte.
 *          Remember to NOT include SOP and FCS fields, so start at the CMD field.
 *
 * @param   byte *msg_ptr - message pointer
 * @param   byte len - length (in bytes) of message
 *
 * @return  result byte
 ***************************************************************************************************/
byte MT_UartCalcFCS(uint8 lastResult, uint8 *msg_ptr, uint8 len )
{
  uint8 x;
  uint8 xorResult = lastResult;

  for ( x = 0; x < len; x++, msg_ptr++ )
    xorResult = xorResult ^ *msg_ptr;

  return ( xorResult );
}
//�Զ���Ĵ��ڽ��ջص�����
void MT_UartProcessZToolData1 ( uint8 port, uint8 event )
{

  uint8 flag = 0,i,j = 0;  //flag�ж���û�����ݣ�j��¼���ݳ���
  uint8 buf[128];         //����128
  (void)event;

  while(Hal_UART_RxBufLen(port)) //��⴮�������Ƿ����
  {
    HalUARTRead(port,&buf[j],1);//���ݽ��յ�buf
    j++;
    flag = 1;
  }
  if(flag == 1)   //������ʱ
  {//�����ڴ棬�ṹ��+����+����
    pMsg = (mtOSALSerialData_t *)osal_msg_allocate( sizeof
                                                   ( mtOSALSerialData_t )+j+1);
    pMsg->hdr.event = CMD_SERIAL_MSG;
    pMsg->msg = (uint8*)(pMsg+1); //�����ݶ�λ���ṹ��
    pMsg->msg [0]= j;             //��¼���ݳ���
    for(i=0;i<j;i++)
      pMsg->msg [i+1]= buf[i];
    osal_msg_send( App_TaskID, (byte *)pMsg ); // �Ǽ����񲢷����ϲ�
    osal_msg_deallocate ( (uint8 *)pMsg ); // �ͷ��ڴ�
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
#if 0
void MT_UartProcessAppData ( uint8 port, uint8 event )
{
   uint8  *bytesInRxBuffer;
  static uint8 *msg;

  (void)event;  // Intentionally unreferenced parameter
  msg = (uint8 *)osal_msg_allocate(5);
  if(msg){
    bytesInRxBuffer = msg;
    *bytesInRxBuffer ++ = CMD_SERIAL_MSG;
  }
  while (Hal_UART_RxBufLen(port))
  {
    HalUARTRead (port, bytesInRxBuffer , 5);
    osal_msg_send( App_TaskID, (byte *)msg );
  }

}
#endif

#endif
#if defined (ZAPP_P1) || defined (ZAPP_P2)
/***************************************************************************************************
 * @fn      MT_UartProcessZAppData
 *
 * @brief   | SOP | CMD  |   Data Length   | FSC  |
 *          |  1  |  2   |       1         |  1   |
 *
 *          Parses the data and determine either is SPI or just simply serial data
 *          then send the data to correct place (MT or APP)
 *
 * @param   port    - UART port
 *          event   - Event that causes the callback
 *
 *
 * @return  None
 ***************************************************************************************************/
void MT_UartProcessZAppData ( uint8 port, uint8 event )
{

  osal_event_hdr_t  *msg_ptr;
  uint16 length = 0;
  uint16 rxBufLen  = Hal_UART_RxBufLen(MT_UART_DEFAULT_PORT);

  /*
     If maxZAppBufferLength is 0 or larger than current length
     the entire length of the current buffer is returned.
  */
  if ((MT_UartMaxZAppBufLen != 0) && (MT_UartMaxZAppBufLen <= rxBufLen))
  {
    length = MT_UartMaxZAppBufLen;
  }
  else
  {
    length = rxBufLen;
  }

  /* Verify events */
  if (event == HAL_UART_TX_FULL)
  {
    // Do something when TX if full
    return;
  }

  if (event & ( HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT))
  {
    if ( App_TaskID )
    {
      /*
         If Application is ready to receive and there is something
         in the Rx buffer then send it up
      */
      if ((MT_UartZAppRxStatus == MT_UART_ZAPP_RX_READY ) && (length != 0))
      {
        /* Disable App flow control until it processes the current data */
         MT_UartAppFlowControl (MT_UART_ZAPP_RX_NOT_READY);

        /* 2 more bytes are added, 1 for CMD type, other for length */
        msg_ptr = (osal_event_hdr_t *)osal_msg_allocate( length + sizeof(osal_event_hdr_t) );
        if ( msg_ptr )
        {
          msg_ptr->event = SPI_INCOMING_ZAPP_DATA;
          msg_ptr->status = length;

          /* Read the data of Rx buffer */
          HalUARTRead( MT_UART_DEFAULT_PORT, (uint8 *)(msg_ptr + 1), length );

          /* Send the raw data to application...or where ever */
          osal_msg_send( App_TaskID, (uint8 *)msg_ptr );
        }
      }
    }
  }
}

/***************************************************************************************************
 * @fn      SPIMgr_ZAppBufferLengthRegister
 *
 * @brief
 *
 * @param   maxLen - Max Length that the application wants at a time
 *
 * @return  None
 *
 ***************************************************************************************************/
void MT_UartZAppBufferLengthRegister ( uint16 maxLen )
{
  /* If the maxLen is larger than the RX buff, something is not right */
  if (maxLen <= MT_UART_DEFAULT_MAX_RX_BUFF)
    MT_UartMaxZAppBufLen = maxLen;
  else
    MT_UartMaxZAppBufLen = 1; /* default is 1 byte */
}

/***************************************************************************************************
 * @fn      SPIMgr_AppFlowControl
 *
 * @brief
 *
 * @param   status - ready to send or not
 *
 * @return  None
 *
 ***************************************************************************************************/
void MT_UartAppFlowControl ( bool status )
{

  /* Make sure only update if needed */
  if (status != MT_UartZAppRxStatus )
  {
    MT_UartZAppRxStatus = status;
  }

  /* App is ready to read again, ProcessZAppData have to be triggered too */
  if (status == MT_UART_ZAPP_RX_READY)
  {
    MT_UartProcessZAppData (MT_UART_DEFAULT_PORT, HAL_UART_RX_TIMEOUT );
  }

}

#endif //ZAPP

/***************************************************************************************************
***************************************************************************************************/

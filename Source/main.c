#include <iocc2530.h>
#include "hal_mcu.h"
#include "hal_assert.h"
#include "hal_board.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include <stdio.h>
#include "info.h"
#include "uart1.h"
#include "clock.h"
#include "htu21d.h"
#include "iic.h"

#define RF_CHANNEL            19                                // 2.4 GHz RF channel
#define PAN_ID                0x2002
#define SEND_ADDR             0x2530
#define RECV_ADDR             0x2522
#define COMMAND               0x10

#define NODE_TYPE             0                                 //0:���սڵ㣬��0�����ͽڵ�

char temp='1';

void io_init(void)
{
  P2SEL &= ~0x01;
  P2DIR &= ~0x01;
}

static basicRfCfg_t basicRfConfig;
int ledstatus = 0;
 uint8 pRxData[128];
void rfSendData(void)
{
    int rlen;
    uint8 pTxData[] = 'c'; 
    char tx[]="����a��ȡ�¶�\n"; 
    char tx2[]="4\n";
    int i;
   basicRfReceiveOff();

    while (TRUE) {
     hal_led_on(1);
     UartSendString(tx,sizeof tx);
      halMcuWaitMs(500);
      if(temp=='1'){
     temp= uart1_recv_char();
      }
     pTxData[0]= temp;
      uart0_send_char(temp);
     basicRfSendPacket(0xffff, (uint8*)pTxData, sizeof pTxData);//�㲥�������ݰ�   
     halMcuWaitMs(500);
      basicRfReceiveOn();
     halMcuWaitMs(500);
     while(!basicRfPacketIsReady());
     rlen =basicRfReceive(pRxData, sizeof pRxData, NULL);  
     if(rlen>0)
     {
         hal_led_off(1);
       uart0_send_string(pRxData);
     }    
     basicRfReceiveOff();
    halMcuWaitMs(500);
    
    }
}

void rfRecvData(void)
{
  uint8 pRxData[128];
  uint8 txData[128]='1';
  int rlen;

    while (TRUE) {
       basicRfReceiveOn();
       printf("   ");
        while(!basicRfPacketIsReady());
        rlen = basicRfReceive(pRxData, sizeof pRxData, NULL);
        printf("   ");
        if(rlen > 0 && pRxData[0] == 'a')
        {
            basicRfReceiveOff();
            hal_led_on(1);   
            sprintf(txData,"�¶�%.2f  ID2",htu21d_get_data(TEMPERATURE)/100.0);
            printf("Temp:");
            printf(txData);
            halMcuWaitMs(500);
            hal_led_off(1);
              halMcuWaitMs(2000);
            halMcuWaitMs(500);
            basicRfSendPacket(0xffff, (uint8*)txData, sizeof txData);
            halMcuWaitMs(500);                    
            basicRfReceiveOn();

         } 

        } 
}

void main(void)
{
    halMcuInit();

    io_init();
    uart1_init();
    hal_led_init();
    iic_init();
    htu21d_init();
    
    hal_uart_init();
    
    
    //lcd_dis();
    
    if (FAILED == halRfInit()) {
        HAL_ASSERT(FALSE);
    }
    // Config basicRF
    basicRfConfig.panId = PAN_ID;
    basicRfConfig.channel = RF_CHANNEL;
    basicRfConfig.ackRequest = TRUE;
#ifdef SECURITY_CCM
    basicRfConfig.securityKey = key;
#endif

    
    // Initialize BasicRF
#if NODE_TYPE
    basicRfConfig.myAddr = SEND_ADDR;
#else
    basicRfConfig.myAddr = RECV_ADDR; 
#endif
    
    if(basicRfInit(&basicRfConfig)==FAILED) {
      HAL_ASSERT(FALSE);
    }
#if NODE_TYPE
  rfSendData();
#else
  rfRecvData();   
#endif
}


#pragma vector = URX0_VECTOR
__interrupt void UART0_ISR(void)
{
  URX0IF = 0;       //���жϱ�־
  //temp = U0DBUF;    //�����ڽ��ջ�������ݴ�ŵ�temp��,temp��char���ͣ���8λ����λ
  
}

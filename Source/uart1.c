#include "uart1.h"

unsigned int recvCnt;
char recvBuf[];

void uart1_init(void){

  
  PERCFG = 0x00;          //����UART0�ı���λ��ΪP0�ڣ���UART0��RX-P0_2��TX-P0_3
  P0SEL  = 0x3c;          //����P0_2��P0_3Ϊ���蹦��
  P2DIR &= ~0xc0;         //����P0�˿�������ΪUART0
  
  /*����USART0�ķ�ʽ*/
  U0CSR |= 0x80;          //SPIͬ��ģʽ  
  
  /*����USART0�Ĳ����ʣ�115200*/
  U0GCR |= 11;            
  U0BAUD |= 216;
  
  /*��UART0�жϱ�־λ��0*/
  UTX0IF = 0;             //���UART0�����жϱ�־λ
  URX0IF = 0;             //���UART0�����жϱ�־λ
  
  /*�����жϣ��Լ�ʹ�ܴ���0�����ж�*/
  IEN0 |= 0x80;           //�����ж�
  IEN0 |= 0x04;           //ʹ�ܴ���0�����ж�
  
  /*���д���0����*/
  U0CSR |= 0x40;

}

void uart0_send_char(char ch){
  U0DBUF = ch;
  while(UTX0IF == 0);
  UTX0IF = 0;
}
void uart0_send_string(char *data){
  while(*data != '\0'){
    uart0_send_char(*data++);
  }
}
void UartSendString(char *Data, int len)
{
  int j;
  for (j = 0; j < len; j++)
  {
    U0DBUF = *Data++;
    for (; 0 == UTX0IF ;);
    UTX0IF = 0;
  }
}
char uart1_recv_char(){
  char ch;
  while(URX0IF == 0);
  ch = U0DBUF;
  URX0IF = 0;
  return ch;
}

#include "uart.h"

#define LO8(var) ((unsigned char)((var)&0xFF))
#define HI8(var) ((unsigned char)((var>>8)&0xFF))

void InitUART (void)
{
  UBRR0L=LO8(Baudrate_Register); // ��������� �������� ����������������� �����
  UBRR0H=HI8(Baudrate_Register);
  // (0xD0 1101 0000 by Default)
  // UCSR0B = RXCIE0, TXCIE0, UDRIE0, RXEN0, TXEN0, UCSZ0, RXB80, TXB80
  // RXEN0=1  �� ��������� �������� �������
  // TXEN0=0  �� ��������� ���������� ��������
  // RXCIE0=1 Receive Interrupt Enable
  // TXCIE0=1 Transmit Interrupt Enable

  UCSR0B = (1<<RXEN0)|(0<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0);

  // UCSR0C = URSEL0, UMSEL0, UPM01, UPM00, USBS0, UCSZ01, UCSZ00, UCPOL0
  // URSEL0=1 Register Select UCSRC or UBRRH
  // UMSEL0 - Synchronous/Asynchronous Mode of UART (0=Async)
  // UPM01..00 - Parity Mode (0=Disabled)
  // USBS0 - Stop Bits (0=One Stop Bit, 1=Two Stop Bits)
  // UCSZ01..00=11 - Character Size (11=8 Bit)
  // UCPOL0 - Clock Polarity (���������� ������ ��� ���������� ������)
  
  UCSR0C = (1<<URSEL0)|(1<<UCSZ01)|(1<<UCSZ00);
  
}

// ===========================================================================
//  UART RXC INTERRUPT
// ���������� UART �� ��������� ������
// � ���������� �� ������ ����� �� UART ����� ����������� �������
// ����� ���������� �� ���� Modbus, � ����� �������� ������ �������
// ��� ������� �������, � ����� ��� ���� ����� ���������� � �����
// ������ UART_Buf. ����� ���� ��� � ����� ����� ������� ��������� ����,
// ������� ~ 3 �� �������� (��������� ������2) � �������� ����������,
// ����� �� ��������� ��������� ������. ��� ����, �� ��������� ��������� ������,
// � ������� ���������� �������2 ����������� ���� EINF � ���, ��� ���������� ��������.
// ��� ��������� ���������� ������ ������, ����� �������� ��������, 
// �������� ���� EINF.

#pragma vector=USART0_RXC_vect
__interrupt void USART_RX(void)
{
  
  unsigned char c;
  c=UDR0; // ����� ����� ��������� �� �������� ���������

// �� ����� ���������� �������� ������ � �������� ������� ������� Modbus
// ������������� ���������� ���� ������ ErrorUART ��� ����������� ��������
/*
  if ( (HowMany_RX==0) && (c!=Net_Addr) )  // ���� ��� ������ �������� ����, �� ��������� ������� �����.
    {
      // ���� ������ �������� ���� �� ����� �������� ������, ��
      TimeOut_RX=0; // ��������� �������� �������� � ����
      HowMany_RX=0;  // ����� �������� �������� ���� � ������������ �� ����������.
      return;
    }
*/

  // ���� �� ��, �� ���������� � ����� �������� ���� � ����������� ���������,
  // � ����� ����������������� ������� �������� �� ������
  LED_RX=0; // �������� LED ���������
  UART_Buf[HowMany_RX]=c;  // ���������� �������� ���� � �����
  HowMany_RX++;            // ����������� ������� ������
  TimeOut_RX=TIMEOUT; // ������������� �������� �������� �� ������ (~3 ms)
  // ������� �������� ������������ ������ ��� ����� ������ ���������� �����
  // � ����� UART_Buf. ����� ������ ������ ����� �� �������� 9600 ���
  // ~ 1 ��, ����� �������, ������������ �� ���������� �������2 ������� ��������
  // ������ �������� 3 �� (���������� �������2 = ~ 1 ��), �������� ��������������
  // ���������� ������ ���������� �����. ����� EINF � 0xFF (�� ������ ����).
  // ����� ����� ����������� �����, ����� ����� �������� (��������) ����������
  // (� ��� ����� ���� ������ �� ����� ������) �� ������ ���������� � �����
  // ������ � ����������� ������������ ������� ������. ����� ������� �����
  // EINF � ���, ��� �����-�� ���������� � ����� ��������, ����� �������
  // ��������� ������� � ��������� ��������� Modbus, ����� �������� ��������
  // ��� ������ ���������� ��������� ������.
}

//===========================================================================
//  UART TXC INTERRUPT
// ���������� UART �� ��������� ��������
// ����� �������� ����� ������, ����������� ���������� �
// ���������� ��������, ���-�� �������� ���� ������������ � ����,
// ������� ���������.
// �� ����� �������� �������� ��������.

#pragma vector=USART0_TXC_vect
__interrupt void USART_TX(void)
{
  if (HowMany_TX >= Pointer_TX)
    {
      // ������� ��������
      unsigned char c=UART_Buf[Pointer_TX]; // ����� ���� �� ������ �� �������� ���������
      Pointer_TX++;           // ����������� ���������
      UDR0=c; // ������ � ������� ����������� - ��������
    }
  else
    {
      // �� ��������� ��������
      UCSR0B&=~(1<<TXEN0); // ���������� ����������� TXEN0=0
      UCSR0B|=(1<<RXEN0); // ��������� ���������
      HowMany_RX=0;
      HowMany_TX=0;
      LED_TX=1;  // ����� LED_TX
    }
}

//==========================================================================
// ��������� ������ �������� �� UART
// howmanyTX - ������� ���� ����� ��������
// ������ ��� �������� ��� ������ ���� � ������ UART_Buf
// ����� ���� ��������� ����������� ���������� �� TXC
// � ���������� ������ � ������ ���������� �� �����������.
// ���������� ��������� ��������.
// �� ����� �������� �������� ��������.

void Transmit (unsigned char howmanytx)
{

//  if (howmanytx!=0) // ���� ���������� ������������ ���� ����� ����, �� ����� �����
//    {
      LED_TX=0; // �������� LED - ������ ��������
      HowMany_TX=(howmanytx-1); // ���������, ������� ���� ���������� � ����������
      Pointer_TX=1; // ��������� � ������ ��� ���������� = 0+1
      UDR0=UART_Buf[0]; // �������� ������ ���� �� ������
      // ����������� ����������� (������ ��� RS485) - ����. �����, ���. ��������
      UCSR0B&=~(1<<RXEN0);
      UCSR0B|=(1<<TXEN0); // ������ �����������, ����� �������������� ���������� TXC
//    }
}




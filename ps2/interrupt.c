#include "interrupt.h"

//========================================================
// ������ ������������ ��� �������� ��������.
// ����� ���� ��������� ������� � ������ � ���, ��� ��������� ������� ������.
// ����������� ������ ��� ���, ����� ���������� ������� ������� ������ 1 ��.

void InitTimer2 (void)
{

  // ========================== TIMER/COUNTER 2 (Timer Mode)
  // (0x03 by Default 0000 0011)
  // f=8.0 MHz/32/256 = 977 Hz (976,5625)
  // TCCR2 = FOC2=0, WGM20, COM21, COM20, WGM21, CS22, CS21, CS20
  // FOC2 - Force Output Compare (��� PWM �����), ������ � ����
  // WGM20 (PWM2) - Mode Select
  // COM21 - Compare Match Output Mode (����� ������ ���������� �������� ����� OC2)
  // COM20 - Compare Match Output Mode, � ������� ������ COM20=COM21=0
  // WGM21 (CTC2) - Mode Select
  // CS22 CS21 CS20 - Clock Select Prescaler
  //    000 - Timer/Counter Stopped
  //    001 - 1:1
  //    010 - 1:8
  //    011 - 1:32
  //    100 - 1:64
  //    101 - 1:128
  //    110 - 1:256
  //    111 - 1:1024
  // CS21=1 & CS20=1 - Prescaler 32x

  TCCR2B=(1<<CS21)|(1<<CS20);
  
  // ========================== TIMERS/COUNTERS INTERRUPT SETUP
  // Timer Counter Interrupt Mask Register
  // (0x8C by Default 1000 1100)
  // TIMSK = TOIE1, OCIE1A, OCIE1B, OCIE2,   TICIE1, TOIE2, TOIE0, OCIE0
  // TOIE1=1 Timer1 Interrupt Enable
  // TICIE=1 Input1 Capture Interrupt Enable
  // TOIE2=1 Timer2 Interrupt Enable
  TIMSK2 = (1<<TOIE2);
  
  // ETIMSK (0x24 By Default 0010 0100)
  //  ETIMSK = -, -, TICIE3, OCIE3A,     OCIE3B, TOIE3, -, -
  // TICIE3=1 Input3 Capture Interrupt Enable
  // TOIE3=1  Timer3 Interrupt Enable
  //ETIMSK = (1<<TICIE3)|(1<<TOIE3);
  
  SecundaOK=0;
  Register_OneSec = SEK_CONST; // ������������� ���������� ��������
  
}


//========================================================================
// TIMER2 OVERFLOW INTERRUPT
// ��������� ����
// ���������� � �������� ���������� 8e6/256/32=1,024 ms

#pragma vector=TIMER2_OVF_vect
__interrupt void TC2_ovf(void)
{
  // ��������� �������� ��� ��������� UART
  // ���� �� ����� � ���������� ������� ����� ����, ��
  // �������, ��� ���������� ��������� ��� ����������
  // ���� ������� �� ����� ����, �� ���������� �������� �������� ��������

  if (TimeOut_RX!=0)
    {
      TimeOut_RX--;
      if (TimeOut_RX==0)
        {
          UCSR0B&=~(1<<RXEN0); // ���������� ���������
          ReceiveOK=0xFF;  // ���������� ���� ��� ���� �������
        }
    }

  if (TimeOut_KBD!=0)
    {
      TimeOut_KBD--;
      if (TimeOut_KBD==0) // ���� ������� ����� ��������, �� ������� ��� ���� �������
        {
          bitcount=11; // ����� ���� ������������� - ���������� �� ������ ����� ��������� � ������
        }
    }
  
  LED_counter++;
  if (LED_counter==255)
    {
//      LED_counter=0;  
      LED_PORT^=(1<<LED_4); // ������� (Debug Code)
    }

  Register_OneSec--; //  ��������� ������� ��� ���������� �������
  if (Register_OneSec==0) // ������ �� �������?
    {
      Register_OneSec=SEK_CONST; // ���� ��������� ������� ������ ���� - �������������
      SecundaOK=0xFF; // ������������� ���� ����� �������
    }

}

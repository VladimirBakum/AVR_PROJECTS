//----------------------------------
// APIS Washer AVTF104 Board
// By (R)soft
// v2.0 19/06/2015
// ��� �������� ����� ver 2.0
// For ATmega328P or ATmega88(PA)

#define SOUND_ON

#include <iom88pa.h>
#include <intrinsics.h> // for __**** procedures
#include <iomacro.h>

#include "avrlib\i2c.h"
#include "avrlib\i2c.c"

#include "prgwash.h"
#include "eeprom.h"
#include "apis.h"

#include "timers.h"
#include "adc.h"
//#include "nec.h"
#include "pwm.h"


//*********************************************************************

void main( void )
{
  InitPorts();
  InitTimers();
  InitADC();
  InitEEPROM(); // �������� � ������������� EEPROM

  //========= Init I2C Part
  // initialize i2c function library
  i2cInit();

  MaskProg = 0xFF; // ����� ��������� ��������� �� ��������� ���������
  PWMCount = 0xFF;
  Timer_I2C = I2C_TIMER_CONST;
  msec = 0;
  sec = 0;
  minute = 0;
  hour = 0;
  timer = 0; // ����� ������� ������� ������
//  SecretCode = SECRET_CODE;
  SecretCode = 0;
  SecretMode = 0;

  ProgramCounter = 0;
  ProgramNumber = 0;
  cnt = 0; // ������� Do While = 0
  
//  ReceivedFlag=0; // ����� ����� �������� �������

  // Enable INT1 for ZeroCross (rising edge) & INT0 for IRDA (falling edge)
  EICRA = (1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(0<<ISC00);

  // INT0 �������� - ����� ������ � ������ ��
  EIMSK = (1<<INT1)|(0<<INT0);  // Interrupt Mask Register (Enable INT1 & INT0)
  __enable_interrupt();

  // Init Indication Registers - ��������� �����������
  Reg.Seg1 = DIG_A;
  Reg.Seg2 = DIG_P;
  Reg.Seg3 = DIG_I;    
  Reg.Seg4 = DIG_S;    
  Reg.Red1 = 0x00;
  Reg.Red2 = 0x00;
  Reg.Red3 = 0x00;
  Reg.Ris1 = 0x00;
  Reg.Ris2 = 0x00;
  Reg.Ris3 = 0x00;
  Reg.Ris4 = 0x00;
  Reg.Modes = 0x00;
  Reg.Spin = 0x00;
  Reg.Termo = 0x00;
  Reg.Timer = 0x00;
  Reg.Lock = 0x00;
  Reg.Key = 0x00;
  Reg.Points = 0x00;
  Reg.Buzzer = 0x00;
  Reg.Gradus = 0x00;

  i2cMasterSendNI(0x40, sizeof (Reg.Buf), Reg.Buf);  // �������� �������� ���������
  Timer_Buzzer = 10;  // ����� ��������������� �����
  delay_timer(2); // �� ����� ������� ����������� 

  Reg.Modes = 0xFF; // ������ ���������� ������

  //========================= �������� ���� ================================  
  while (1)
  {

    adcLine = adc_read(6);
    adcTemperature = adc_read(7);

/*
    // �������� �� ������ - ����� ������� �����������
    if (adcTemperature==0)
    {
      Reg.Seg1 = DIG_E; // Error1 - ������ ����������� ������� (�������� ��� 0x000)
      Reg.Seg2 = DIG_r;
      Reg.Seg3 = DIG_r;
      Reg.Seg4 = DIG_1;
      i2cMasterSendNI(0x40, 4, Reg.Buf);  // �������� �������� ���������
      while (1) { __no_operation(); } // ������ ����� ������ � ��������
    }

    // �������� �� ������ - ������ ����������� ���������
    if (adcTemperature==0x3FF)
    {
      Reg.Seg1 = DIG_E; // Error2 - ������ ����������� ��������� (������� ��� 0x3FF)
      Reg.Seg2 = DIG_r;
      Reg.Seg3 = DIG_r;
      Reg.Seg4 = DIG_2;
      i2cMasterSendNI(0x40, 4, Reg.Buf);  // �������� �������� ���������
      while (1) { __no_operation(); } // ������ ����� ������ � ��������
    }
*/
    
    temperature = TemperatureConvert( adcTemperature );
    
    //-------------- ��������� ������ � ������ �� � ���������� ������ --------------------
    if (KeyFlag) // ���� ���� �������� ������� ��������� ��� ������ ������
    {

      KeyFlag=0;

      // --- �������� ������� STOP ��� ����� ��������� ������������ ������������� ----
      if (ProgramNumber && Key==kStart && !RepeatFlag) // ���� ��������� �������� � ������ ������ START/STOP
      {
        ProgramStartStop(8, PROGRAM8); // �������� � ����� ���� (� ������� � ����������� 8 ���������)
      }

      if (ProgramNumber==0) // ���� ��������� �� ��������
      {
        ManualMode(); // ����� ������� �������� � �������������� ���������� ������
      }

      Key=0;  // ����� ���� ������� ��� ����������� � ������������
    }
    //------------ End of ��������� ������ � ������ �� ------------------

    //--------- ��������� State ��������� � �������� ����� ----------------

    if (ForwardState==0) FORWARD=0; else FORWARD=1;
    if (ReverseState==0) REVERSE=0; else REVERSE=1;
    // ���� ���� ������ �������� - ����� 4 ����������
    if (ForwardState || ReverseState) Reg.Ris4 = 0xFF;
    else                              Reg.Ris4 = 0;

    // � ���� ������ ������� ���������� ����������� ������ ���������������,
    // �.�. ���. 1 ������������� ���������� � ���. 0 - ���.

    // ����� ����
    if (LockState==0) LOCK = 1; // ���� ����, �� ���������
    else              LOCK = 0;

    if (adcLine < LINE_ON_TRESHOLD) Reg.Key = 0xFF;      // ���� ���� ����, �� ���������� ��������
    else                            Reg.Key = 0;         // ����� �����

    // ����� �������� - ����� 3 ����������
    if (PompState==0) { POMP = 1; Reg.Ris3 = 0; } // ���� ����, �� ���������
    else              { POMP = 0; Reg.Ris3 = 0xFF; }

    // ����� ���� ��� ��������������� ������ - ����� 1 ����������
    if (WaterState1==0)  { WATER1 = 1; Reg.Ris1 = 0; }  // ���� ����, �� ���������
    else                 { WATER1 = 0; Reg.Ris1 = 0xFF; }

    // ����� ���� ��� �������� ������ - ����� 2 ����������
    if (WaterState2==0)  { WATER2 = 1; Reg.Ris2 = 0; }  // ���� ����, �� ���������
    else                 { WATER2 = 0; Reg.Ris2 = 0xFF; }

    if (LEVEL1) Reg.Red1 = 0;
    else        Reg.Red1 = 0xFF;  // ������ ���������� �������� ������������

    if (LEVEL2) Reg.Red2 = 0;
    else        Reg.Red2 = 0xFF;

    if (LEVEL3) Reg.Red3 = 0;
    else        Reg.Red3 = 0xFF;

/*
    // �������� �� ������ - ������ ������ ����������� ��� � ������
    if (LEVEL1==1 && LEVEL2==1 && LEVEL3==1)
    {
      Reg.Seg1 = DIG_E; // Error3 - ������ ������ ����������� ��� � ������ (LEVEL1=LEVEL2=LEVEL3=1)
      Reg.Seg2 = DIG_r;
      Reg.Seg3 = DIG_r;
      Reg.Seg4 = DIG_3;
      i2cMasterSendNI(0x40, 4, Reg.Buf);  // �������� �������� ���������
      while (1) { __no_operation(); } // ������ ����� ������ � ��������
    }
*/
    
    // ��������� ������� ������ � �������� ����� (��� ������������ ����)
    // ���� ����� ������� ������������ �� "��� ������" ��� ����������� ����������, �� 
    // ������ ���� ������ ����������. ������� HeatingState ����� ������������� � ���������,
    // �� �������� � ������� ����� ����� ������������� �������� ������ HEATING, ��� �����
    // ��������� ��� ��������� �������� ������. ���� ��������� ��������, ����� � ���������
    // ������ ���� ����� �� ������ �������, �� ����� ����� ������� ���� � ������ ������
    // �������� � LEVEL1, �������������� ������ ���������, ����� �������� ���. ��, �����
    // ����� ��������� "��������" �.�. ����� ����� ������� ����, �� ��� "��������" ��������.
    // � ���� �������� ����� ������ ������ ������ ������, ������ ����, � ��������� ���������.
    if ( (LEVEL1==0)&&(LEVEL2==1) ) HeatingState = 0;
    if ( (LEVEL1==1)&&(LEVEL2==1) ) HeatingState = 0; // ����������� ��������� (������ ��������)

    // LEVEL3 ����������, ����� LEVEL2=0. �.�. �� ������������� � LEVEL2 �� LEVEL3, � ����������� � LEVEL2.
//    if ( LEVEL3==0 && LEVEL2==0 ) PompState = 0xFF; // ���� ������������ ����, �� ��� �����

    // ���
    if (HeatingState==0)
    {
      HEATING = 0; 
      if (TempMode!=8)
      {
        if (TempMode!=9) Reg.Termo = 0;
      }
    }
    else
    {
      HEATING = 1;
      if (TempMode!=8)
      {
        if (TempMode!=9) Reg.Termo = 0xFF; // ������ ���������� ������� �����������
      }
    }
    
    //------------ End of ��������� State ��������� --------------------

    //------------ ��������� ������ -----------------------------------
    // ����� ������ �������� ������ ����� ���������
    // MaskProg=0xFF - ���������� ��������� � ����������� �� ��������� ����� ��������
    // MaskProg=0 - ���������� ��������� �� ���� ���������� ����� ��������
    //              (������� ��� �������������� ��������� � ������ ������� ���������)
    switch (Reg_Prog & MaskProg)
    {
    // ��� �������, � ���� ������ ������������ ��������� LINE (����� ����)
    case 1:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_L;
                Reg.Seg4 = HEX[LockState&0x01];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_1;
              }
              break;
    // ��� �������, � ���� ������ ������������ ���� ���������� �������
    case 2:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_S;
                Reg.Seg2 = DIG_P;
                Reg.Seg3 = HEX[(RotorState>>4)&0x0F];
                Reg.Seg4 = HEX[RotorState&0x0F];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_2;
              }
              break;
    // ��� �������, � ���� ������ ������������ ��������� ������� ������ ����
    case 3:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_H;
                Reg.Seg2 = DIG_2;
                Reg.Seg3 = DIG_o;
                Reg.Seg4 = HEX[WaterState1&0x01];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_3;
              }
              break;
    // ��� �������, � ���� ������ ������������ ��������� �����
    case 4:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_P;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = HEX[PompState&0x01];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_4;
              }
              break;
    // ��� �������, � ���� ������ ������������ ��������� ���� ����
    case 5:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_H;
                Reg.Seg2 = DIG_E;
                Reg.Seg3 = DIG_A;
                Reg.Seg4 = HEX[HeatingState&0x01];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_5;
              }
              break;
    // ��� �������, � ���� ������ ������������ �������� ��� adcLine
    case 6:
              if (SecretMode==0xFF) { SecretMode = 0; SecretCode = 0; } // ����� ������� ��� �������� �����
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = HEX[(adcLine>>8)&0x0F];
                Reg.Seg3 = HEX[(adcLine>>4)&0x0F];
                Reg.Seg4 = HEX[adcLine&0x0F];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_6;
              }
              break;
    // ��� �������, � ���� ������ ����� ������ ��������� ��� (Secret Code)
    case 7:
              if (SpinMode==8)  { SpinMode = 0; Reg.Spin = 0; }   // ����� ������� ��� �������� �����
              if (TempMode==8)  { TempMode = 0; Reg.Termo = 0; }  // ����� ������� ��� �������� �����
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretMode==0xFF) // ���� ���������� ��������� �����, �� ���������� ���� ���������� ����
              {
                Reg.Seg1 = DIG_S;
                Reg.Seg2 = DIG_c;
                Reg.Seg3 = HEX[(SecretCode>>4)&0x0F];
                Reg.Seg4 = HEX[SecretCode&0x0F];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_7;
              }
              break;
    // ��������� �8 - ��������������� ������
    case 8:
              if (SpinMode==9)  { SpinMode = 0; Reg.Spin = 0; }   // ����� ������� ��� �������� �����
              if (TempMode==9)  { TempMode = 0; Reg.Termo = 0; }  // ����� ������� ��� �������� �����
              if (SecretMode==0xFF) { SecretMode = 0; SecretCode = 0; } // ����� ������� ��� �������� �����
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 8)  // ���� ��������� �������� �� ���������� PC & ������ �����
              {
                if (HeatingState != 0) // ���� ������� ���, �� ���������� ����������� �������
                {
                  Reg.Gradus = 0xFF;
                  Reg.Seg1 = DIG_t;
                  Reg.Seg2 = DIG_SPACE;
                  Reg.Seg3 = DEC[temperature]>>8; 
                  Reg.Seg4 = DEC[temperature]&0xFF;
                }
                else
                {
                  Reg.Seg1 = DEC[ProgramCounter]>>8;
                  Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                  Reg.Seg3 = DEC[timer]>>8; 
                  Reg.Seg4 = DEC[timer]&0xFF;
                }
              }
              else
              {
                if (TempMode==8)  // ���� ������ ����� ��������� t, �� ���������� �������� t
                {
                  Reg.Gradus = 0xFF;
                  Reg.Seg1 = DIG_t;
                  Reg.Seg2 = DIG_SPACE;
                  Reg.Seg3 = DEC[SetTemperature]>>8; 
                  Reg.Seg4 = DEC[SetTemperature]&0xFF;
                }
                else  // ����� ������� ����� ��� �������
                {
                  if (SpinMode==8)  // ���� ������ ����� ��������� ��������, �� ���������� �������� ��������
                  {
                    Reg.Seg1 = DIG_SPACE;
                    Reg.Seg2 = DEC[SetSpin]&0xFF;
                    Reg.Seg3 = DIG_0;
                    Reg.Seg4 = DIG_0;
                  }
                  else
                  {
                    Reg.Seg1 = DIG_P;
                    Reg.Seg2 = DIG_r;
                    Reg.Seg3 = DIG_SPACE;
                    Reg.Seg4 = DIG_8;
                  }
                }
              }
              break;
    // ��������� �9 - ������� ������
    case 9:
              if (SpinMode==8)  { SpinMode = 0; Reg.Spin = 0; }   // ����� ������� ��� �������� �����
              if (TempMode==8)  { TempMode = 0; Reg.Termo = 0; }  // ����� ������� ��� �������� �����
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 9)  // ���� ��������� �������� �� ���������� PC & ������ �����
              {
                if (HeatingState != 0) // ���� ������� ���, �� ���������� ����������� �������
                {
                  Reg.Gradus = 0xFF;
                  Reg.Seg1 = DIG_t;
                  Reg.Seg2 = DIG_SPACE;
                  Reg.Seg3 = DEC[temperature]>>8; 
                  Reg.Seg4 = DEC[temperature]&0xFF;
                }
                else
                {
                  Reg.Seg1 = DEC[ProgramCounter]>>8;
                  Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                  Reg.Seg3 = DEC[timer]>>8; 
                  Reg.Seg4 = DEC[timer]&0xFF;
                }
              }
              else
              {
                if (TempMode==9)  // ���� ������ ����� ��������� t, �� ���������� �������� t
                {
                  Reg.Gradus = 0xFF;
                  Reg.Seg1 = DIG_t;
                  Reg.Seg2 = DIG_SPACE;
                  Reg.Seg3 = DEC[SetTemperature]>>8; 
                  Reg.Seg4 = DEC[SetTemperature]&0xFF;
                }
                else  // ����� ������� ����� ��� �������
                {
                  if (SpinMode==9)  // ���� ������ ����� ��������� ��������, �� ���������� �������� ��������
                  {
                    Reg.Seg1 = DIG_SPACE;
                    Reg.Seg2 = DEC[SetSpin]&0xFF;
                    Reg.Seg3 = DIG_0;
                    Reg.Seg4 = DIG_0;
                  }
                  else
                  {
                    Reg.Seg1 = DIG_P;
                    Reg.Seg2 = DIG_r;
                    Reg.Seg3 = DIG_SPACE;
                    Reg.Seg4 = DIG_9;
                  }
                }
              }
              break;
    // ��� �������, � ���� ������ ������������ ������� ����������� � ��������
    // � ����� ����� ������� �� ������ ���������� - HEX �������� ��� ����������.
    case 10:
              if (SpinMode!=0)  { SpinMode = 0; Reg.Spin = 0; }   // ����� ������� ��� �������� �����
              if (TempMode!=0)  { TempMode = 0; Reg.Termo = 0; }  // ����� ������� ��� �������� �����
              Reg.Gradus = 0xFF;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              Reg.Seg1 = DIG_SPACE;
              Reg.Seg2 = DIG_SPACE;
              if (temperature==0xFF) { Reg.Seg3 = DIG_MINUS; Reg.Seg4 = DIG_MINUS; }
              else  { Reg.Seg3 = DEC[temperature]>>8; Reg.Seg4 = DEC[temperature]&0xFF; }
              break;
    case 11:
              if (SpinMode==12)  { SpinMode = 0; Reg.Spin = 0; } // ����� ������� ��� �������� �����
              Reg.Timer =  0xFF;
              Reg.Points = 0x80;
              Reg.Gradus = 0x00;
              if (!ClockMode)
              {
                Reg.Seg1 = DEC[minute]>>8;
                Reg.Seg2 = DEC[minute]&0xFF;
                Reg.Seg3 = DEC[sec]>>8;
                Reg.Seg4 = DEC[sec]&0xFF;
              }
              else
              {
                Reg.Seg1 = DEC[hour]>>8;
                Reg.Seg2 = DEC[hour]&0xFF;
                Reg.Seg3 = DEC[minute]>>8;
                Reg.Seg4 = DEC[minute]&0xFF;
              }
              break;
    // ��������� �12 - ����������� ����������, ���� � ���������� �����
    case 12:
              if (SpinMode==13)  { SpinMode = 0; Reg.Spin = 0; } // ����� ������� ��� �������� �����
              Reg.Gradus = 0x00;
              Reg.Timer =  0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 12)  // ���� ��������� �������� �� ���������� PC & ������ �����
              {
                Reg.Seg1 = DEC[ProgramCounter]>>8;
                Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                Reg.Seg3 = DEC[timer]>>8; 
                Reg.Seg4 = DEC[timer]&0xFF;
              }
              else
              {
                if (SpinMode==12)  // ���� ������ ����� ��������� ��������, �� ���������� �������� ��������
                {

                  Reg.Seg1 = DIG_SPACE;
                  Reg.Seg2 = DEC[SetSpin]&0xFF; // ���������� ������� ������������� �������� ��������
                  Reg.Seg3 = DIG_0;
                  Reg.Seg4 = DIG_0;
                }
                else  // ����� ������� �����
                {
                  Reg.Seg1 = DIG_P;
                  Reg.Seg2 = DIG_r;
                  Reg.Seg3 = DIG_1;
                  Reg.Seg4 = DIG_2;
                }
              }
              break;
    // ��������� �13 - ������ ����������, ���� � ���������� �����
    case 13:
              if (SpinMode==12 || SpinMode==14)  { SpinMode = 0; Reg.Spin = 0; } // ����� ������� ��� �������� �����
              Reg.Gradus = 0x00;
              Reg.Timer =  0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 13)  // ���� ��������� �������� �� ���������� PC & ������ �����
              {
                Reg.Seg1 = DEC[ProgramCounter]>>8;
                Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                Reg.Seg3 = DEC[timer]>>8; 
                Reg.Seg4 = DEC[timer]&0xFF;
              }
              else
              {
                if (SpinMode==13)  // ���� ������ ����� ��������� ��������, �� ���������� �������� ��������
                {

                  Reg.Seg1 = DIG_SPACE;
                  Reg.Seg2 = DEC[SetSpin]&0xFF; // ���������� ������� ������������� �������� ��������
                  Reg.Seg3 = DIG_0;
                  Reg.Seg4 = DIG_0;
                }
                else  // ����� ������� �����
                {
                  Reg.Seg1 = DIG_P;
                  Reg.Seg2 = DIG_r;
                  Reg.Seg3 = DIG_1;
                  Reg.Seg4 = DIG_3;
                }
              }
              break;
    // ��������� �14 - ����������� �����
    case 14:
              if (SpinMode==13 || SpinMode==15)  { SpinMode = 0; Reg.Spin = 0; } // ����� ������� ��� �������� �����
              Reg.Gradus = 0x00;
              Reg.Timer =  0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 14)  // ���� ��������� �������� �� ���������� PC & ������ �����
              {
                Reg.Seg1 = DEC[ProgramCounter]>>8;
                Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                Reg.Seg3 = DEC[timer]>>8; 
                Reg.Seg4 = DEC[timer]&0xFF;
              }
              else
              {
                if (SpinMode==14)  // ���� ������ ����� ��������� ��������, �� ���������� �������� ��������
                {

                  Reg.Seg1 = DIG_SPACE;
                  Reg.Seg2 = DEC[SetSpin]&0xFF; // ���������� ������� ������������� �������� ��������
                  Reg.Seg3 = DIG_0;
                  Reg.Seg4 = DIG_0;
                }
                else  // ����� ������� �����
                {
                  Reg.Seg1 = DIG_P;
                  Reg.Seg2 = DIG_r;
                  Reg.Seg3 = DIG_1;
                  Reg.Seg4 = DIG_4;
                }
              }
              break;
    // ��������� �14 - ���������� �����
    case 15:
              if (SpinMode==14)  { SpinMode = 0; Reg.Spin = 0; } // ����� ������� ��� �������� �����
              Reg.Gradus = 0x00;
              Reg.Timer =  0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 15)  // ���� ��������� �������� �� ���������� 
              {
                Reg.Seg1 = DEC[ProgramCounter]>>8;
                Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                Reg.Seg3 = DEC[timer]>>8; 
                Reg.Seg4 = DEC[timer]&0xFF;
              }
              else // ����� ���������� ������� ��� ����� ��������� ���������
              {
                if (SpinMode==15)  // ���� ������ ����� ��������� ��������, �� ���������� �������� ��������
                {

                  Reg.Seg1 = DIG_SPACE;
                  Reg.Seg2 = DEC[SetSpin]&0xFF; // ���������� ������� ������������� �������� ��������
                  Reg.Seg3 = DIG_0;
                  Reg.Seg4 = DIG_0;
                }
                else  // ����� ������� �����
                {
                  Reg.Seg1 = DIG_P;
                  Reg.Seg2 = DIG_r;
                  Reg.Seg3 = DIG_1;
                  Reg.Seg4 = DIG_5;
                }
              }
              break;
    // ��������� �16 - ���� ����
    case 16:
              if (SpinMode==15)  { SpinMode = 0; Reg.Spin = 0; }
              Reg.Gradus = 0x00;
              Reg.Timer =  0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 16)  // ���� ��������� �������� �� ���������� PC & ������ �����
              {
                Reg.Seg1 = DEC[ProgramCounter]>>8;
                Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                Reg.Seg3 = DEC[timer]>>8; 
                Reg.Seg4 = DEC[timer]&0xFF;
              }
              else
              {
                Reg.Seg1 = DIG_P;
                Reg.Seg2 = DIG_r;
                Reg.Seg3 = DIG_1;
                Reg.Seg4 = DIG_6;
              }
    }
    
    
    
    
    //------------ I2C Section ------------------------
    // �������� ������ 50 �� ���������� ��������� ��������� � ������������ ����������
    Timer_I2C--; // ������ ������ I2C
    switch (Timer_I2C)
    {
    case 0: Timer_I2C = I2C_TIMER_CONST;  // ���� ������� ������ ���� - �������������
            break;

    case 11: 
            i2cMasterReceive(0x41, 3, BufRead); // ��������� 3 ����� (��� ������ ����� ������ �� ���� ������)
            // ���������� ������ �� I2C. �� ������������ �������� �� ������
            // ��������� ������ �� I2C. ����� ������ ������ � ���������� NI (NonInterrupt) �� ������� ���
            // ��������� NI ������������ ������ ����������� ����������.
            if ( (BufRead[0]>0) && (BufRead[0]<17) )  // ���� ������� �������� � ������. ���������, �� ����������
            {
              Reg_Prog = BufRead[0];
              Reg_But1 = BufRead[1];
              Reg_But2 = BufRead[2]; 
            }
            break;

    case 17: 
              i2cMasterSendNI(0x40, sizeof (Reg.Buf), Reg.Buf);  // �������� ��� �������� ���������
              break;
    }
    //----------- End of I2C section ------------------

    // ���������� ��������� ������    
    WashingProcess();

    
  }
}




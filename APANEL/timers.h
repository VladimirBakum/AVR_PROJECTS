// APIS Panel I/O timers header

//===========================================================================
// 8-bit Timer0 (1:256) for Delays Indication
// 8-bit Timer2 (1:128) for Dynamic Indication

void InitTimers (void)
{
  //========================== Timer0 Setup 1:256
  // f=8MHz/256/256=122,07
  //    000 - Timer/Counter Stopped
  //    001 - 1:1
  //    010 - 1:8
  //    011 - 1:64
  //    100 - 1:256
  //    101 - 1:1024
  //    110 - Ext T0
  //    111 - Ext T0

  TCCR0=(1<<CS02)|(0<<CS01)|(0<<CS00); // 1:256

  // ========================== Timer2 Setup 1:128
  // f=8.0 MHz/128/256 (4,096 ms)

  // FOC2 - Force Output Compare (��� PWM �����), ������ � ����
  // COM21 - Compare Match Output Mode (����� ������ ���������� �������� ����� OC2)
  // COM20 - Compare Match Output Mode, � ������� ������ COM20=COM21=0
  // WGM20 (PWM2) - Mode Select
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

  TCCR2=(1<<CS22)|(0<<CS21)|(1<<CS20); // 1:128
  
  
  // ========================== TIMERS/COUNTERS INTERRUPT SETUP
  // Timer Counter Interrupt Mask Register
  // TOIE0=1 Timer0 overflow Interrupt Enable
  // TOIE2=1 Timer2 overflow Interrupt Enable
  TIMSK = (0<<OCIE2)|(1<<TOIE2)|(0<<OCIE0)|(1<<TOIE0);
}



//========================================================================
// TIMER0 OVERFLOW INTERRUPT
// ��������� ����
// ���������� � �������� ���������� 8e6/256/64=2,048 ms

#pragma vector=TIMER0_OVF_vect
__interrupt void TC0_ovf(void)
{
  Reg_Secunda--; // ��������� ������� ��� ���������� �������
  if (Reg_Secunda==0) // ������ �� �������?
  {
    Reg_Secunda=SEK_CONST; // ���� ��������� ������� ������ ���� - �������������
    FlagSecunda = ~FlagSecunda; // ������������� ���� ����� �������
  }

  Reg_Half--;
  if (Reg_Half==0)
  {
    Reg_Half=HALF_CONST;
    FlagHalf = ~FlagHalf;
  }
  
  Reg_Quadro--;
  if (Reg_Quadro==0)
  {
    Reg_Quadro=QUADRO_CONST;
    FlagQuadro = ~FlagQuadro;
  }

}


//========================================================================
// TIMER2 OVERFLOW INTERRUPT
// ����� ������ � ������������ ���������
// ���������� � �������� ���������� 1/(8e6/128/256)=4,096 ms

#pragma vector=TIMER2_OVF_vect
__interrupt void TC2_ovf(void)
{
  Reg_ADC=ReadADC();  // ������ ���
  ProgramSelect(Reg_ADC);  // ������ � ������� Reg_Prog � �����. �� ��������� ���

    //============== ��������� ������ ==================================    
  if (!SW1) Reg_But1.BUT1=1;
  else      Reg_But1.BUT1=0;
    
  if (!SW2) Reg_But1.BUT2=1;
  else      Reg_But1.BUT2=0;
    
  if (!SW3) Reg_But1.BUT3=1;
  else      Reg_But1.BUT3=0;
    
  if (!SW4) Reg_But1.BUT4=1;
  else      Reg_But1.BUT4=0;
    
  if (!SW5) Reg_But1.BUT5=1;
  else      Reg_But1.BUT5=0;
    
  if (!SW6) Reg_But1.BUT6=1;
  else      Reg_But1.BUT6=0;
    
  if (!SW7) Reg_But1.BUT7=1;
  else      Reg_But1.BUT7=0;
    
  if (!SW8) Reg_But1.BUT8=1;
  else      Reg_But1.BUT8=0;
    
  if (!SW9) Reg_But2=0xFF; 
  else      Reg_But2=0;

  //-------------------------------------------------
  // ����� �������� ��������� ������ ������� �������
  // �������������� �������� ��� �������� �
  // ���������� � ������� COM0 ��� �����������.
  // � ����������� �� ������������� �������� � ���������,
  // �����. ������ ������� � ������ ��������
  
  // ��������� ������ Termo
  if (Reg.Termo==0xFF) COM0.TERMO=0; else COM0.TERMO=1;
  if (Reg.Termo==0x80 & FlagSecunda)    COM0.TERMO = ~COM0.TERMO;
  if (Reg.Termo==0xC0 & FlagHalf!=0)    COM0.TERMO = ~COM0.TERMO;
  if (Reg.Termo==0xE0 & FlagQuadro!=0)  COM0.TERMO = ~COM0.TERMO;

  // ��������� ������ Timer
  if (Reg.Timer==0xFF) COM0.TIMER=0; else COM0.TIMER=1;
  if (Reg.Timer==0x80 & FlagSecunda)    COM0.TIMER = ~COM0.TIMER;
  if (Reg.Timer==0xC0 & FlagHalf!=0)    COM0.TIMER = ~COM0.TIMER;
  if (Reg.Timer==0xE0 & FlagQuadro!=0)  COM0.TIMER = ~COM0.TIMER;

  // ��������� ������ Lock
  if (Reg.Lock==0xFF) COM0.LOCK=0; else COM0.LOCK=1;
  if (Reg.Lock==0x80 & FlagSecunda)    COM0.LOCK = ~COM0.LOCK;
  if (Reg.Lock==0xC0 & FlagHalf!=0)    COM0.LOCK = ~COM0.LOCK;
  if (Reg.Lock==0xE0 & FlagQuadro!=0)  COM0.LOCK = ~COM0.LOCK;

  // ��������� ������ Key
  if (Reg.Key==0xFF) COM0.KEY=0; else COM0.KEY=1;
  if (Reg.Key==0x80 & FlagSecunda)    COM0.KEY = ~COM0.KEY;
  if (Reg.Key==0xC0 & FlagHalf!=0)    COM0.KEY = ~COM0.KEY;
  if (Reg.Key==0xE0 & FlagQuadro!=0)  COM0.KEY = ~COM0.KEY;

  // ��������� ������ Points
  if (Reg.Points==0xFF) COM0.POINTS=0; else COM0.POINTS=1;
  if (Reg.Points==0x80 & FlagSecunda)    COM0.POINTS = ~COM0.POINTS;
  if (Reg.Points==0xC0 & FlagHalf!=0)    COM0.POINTS = ~COM0.POINTS;
  if (Reg.Points==0xE0 & FlagQuadro!=0)  COM0.POINTS = ~COM0.POINTS;

  // ��������� ������ Buzzer
  // ������ ������� ��� ��������
  if (Reg.Buzzer==0xFF) COM0.BUZZER=0; else COM0.BUZZER=1;

  // ��������� ������ Gradus
  // ������ ������� ��� ��������
  if (Reg.Gradus==0xFF) COM0.GRADUS=0; else COM0.GRADUS=1;

  // ��������� ������ Spin
  if (Reg.Spin==0xFF) SPIN=0; else SPIN=1;
  if (Reg.Spin==0x80 & FlagSecunda) SPIN = ~SPIN;
  if (Reg.Spin==0xC0 & FlagHalf!=0)    SPIN = ~SPIN;
  if (Reg.Spin==0xE0 & FlagQuadro!=0)  SPIN = ~SPIN;

  // ��������� ������ �������
  if (Reg.Modes==0xFF) MODES=0; else MODES=1;


  //------------ ������������ ��������� COM0...COM4 -----------------------------
  // ������ ���������� (4 ��) �������������� ���� ����������, ����� 5 ���������
  switch (DynamicStage)
  {
    case 0: 
      COM0_OUT=1;      
      SetSegments(Reg.Seg1);
      if (Reg.Red1==0xFF) LEDS=0; else LEDS=1; // ��������� �������� ����������
      COM1_OUT=0; // ��������� COM1 - ������ ����������
      break;
    case 1: 
      COM1_OUT=1;
      SetSegments(Reg.Seg2);
      if (Reg.Red2==0xFF) LEDS=0; else LEDS=1; // ��������� �������� ����������
      if (Reg.Ris1==0xFF) RIS=1;  else RIS=0;  // ��������� ����� ������� (��������� �����)
      COM2_OUT=0; // ��������� COM2
      break;
    case 2: 
      COM2_OUT=1;
      SetSegments(Reg.Seg3);
      if (Reg.Red3==0xFF) LEDS=0; else LEDS=1; // ��������� �������� ����������
      if (Reg.Ris2==0xFF) RIS=1;  else RIS=0;  // ��������� ����� �������    
      COM3_OUT=0; // ��������� COM3
      break;
    case 3:
      COM3_OUT=1;
      SetSegments(Reg.Seg4);
      if (Reg.Ris3==0xFF) RIS=1;  else RIS=0;  // ��������� ����� �������
      COM4_OUT=0; // ��������� COM4
      break;
    case 4:
      COM4_OUT=1;
      SetSegments(COM0.byte); // ������
      if (Reg.Ris4==0xFF) RIS=1;  else RIS=0;  // ��������� ����� �������        
      COM0_OUT=0; // ��������� COM0
      break;
  }

  DynamicStage++;
  if (DynamicStage>4) DynamicStage=0;
  //----------------------------------------------------------------------
    
}

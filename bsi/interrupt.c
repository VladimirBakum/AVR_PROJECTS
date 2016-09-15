//=======================================================================
//  TIMER3 CAPTURE INTERRUPT
// ������� ��������� ���������� �������� ������� � ������ �������

#pragma vector=TIMER3_CAPT_vect
__interrupt void TC3_capture(void)
{

  unsigned int tmpLO = ICR3; // ����� �������� ��, ��� ���������
  unsigned int tmpHI = Capture_Reg_HI2;

  if (ETIFR&(1<<TOV3)) // ���� TOV3=1
    {
      Faza2=0;
    }
  else
    {
      if (Period2==0)
        {
          if (Faza2)
            {
              Period2=0xFF;
              ICR2A = (tmpHI*65536)|(tmpLO);
              Faza2=0;
            }
          else  // ���� Faza1=0
            {
              ICR2B = (tmpHI*65536)|(tmpLO);
              Faza2=0xFF;
              Register_TPZ2=BSI_Settings.ZTimeMeasure[1];
            }
        }
    }

  BSI_ResultEx.BSI_Results.Z_cnt[1]++;

}

//=======================================================================
//  TIMER3 OVERFLOW INTERRUPT
// ������������ �������� ������� � ������ �������

#pragma vector=TIMER3_OVF_vect
__interrupt void TC3_ovf(void)
{
  Capture_Reg_HI2++; // ICR32 ICR33 �������������� ������� �����
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

//  PORTD^=(1<<WDI); // ������� ����� WDI (Debug Code)

  Register_OneSec--; // RG1SL RG1SH ��������� ������� ��� ���������� �������
  if (Register_OneSec==0) // ������ �� �������?
    {
      Register_OneSec=SEK_CONST; // ���� ��������� ������� ������ ���� - �������������
      SecundaOK=0xFF; // ������������� ���� ����� �������
    }

  Register_WDT--;
  if (Register_WDT==0)
    {
      Register_WDT = WDT_CONST; // ������������ �������� WDT
      Flag_WDT = 0xFF; // ���������� ���� WDT
    }
  
  if (Register_TPZ1!=0)  // ���� ������� TPZ1 ��� �� ����, ��
    {
      Register_TPZ1--;       // ��������� ������� TPZ1
      if (Register_TPZ1==0)  // ���� ������� �������� �� ���� �� ERR_IZM_PER: - ������ - ������� TPZ1
        {
          Faza1=0;  // ������ ��� FAZA1
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ1us=0;     // ������ ��� FT1
          BSI_ResultEx.BSI_Results.Z_Tus[0]=0; // ������ T_PER1
        }
    }

  if (Register_TPZ2!=0)
    {
      Register_TPZ2--;
      if (Register_TPZ2==0)
        {
          Faza2=0;
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ2us=0;
          BSI_ResultEx.BSI_Results.Z_Tus[1]=0;
        }
    }
}

// -----------------------------------------------------------------
//  TIMER1 CAPTURE INTERRUPT
// ������� ��������� ���������� ������� ������� � ������ �������

#pragma vector=TIMER1_CAPT_vect
__interrupt void TC1_capture(void)
{
  // �� ����� ������� (���������� ICR1) �������� 21 ���� (10 ������ ST � ���� IN)
  unsigned int tmpLO = ICR1; // ����� �������� ��, ��� ���������
  unsigned int tmpHI = Capture_Reg_HI1;
  
  if (TIFR&(1<<TOV1)) // ���� TOV1=1
    {
      Faza1=0;
    }
  else  // TOV1=0
    {
      if (Period1==0)
        {
          if (Faza1)
            {
              Period1=0xFF;
              ICR1A = (tmpHI*65536)|(tmpLO);
              Faza1=0;
            }
          else  // ���� Faza1=0
            {
              ICR1B = (tmpHI*65536)|(tmpLO);
              Faza1=0xFF;
              Register_TPZ1=BSI_Settings.ZTimeMeasure[0]; // ������������� ��������������� �������
            }
        }
    }

  BSI_ResultEx.BSI_Results.Z_cnt[0]++;
}

// -----------------------------------------------------------------
//  TIMER1 OVERFLOW INTERRUPT
// ������������ ������� ������� � ������ �������
// ���������� �������� �� 32 ���

#pragma vector=TIMER1_OVF_vect
__interrupt void TC1_ovf(void)
{
  Capture_Reg_HI1++; // ICR12 ICR13 �������������� ������� �����
}

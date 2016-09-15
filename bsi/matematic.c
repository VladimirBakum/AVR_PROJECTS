#include "matematic.h"
// #include "divide.h"

/*
void ClearSRAM (void)
{
  memset (BUFF,0x00,sizeof(BUFF));
}
*/

//============================================================================
// ��������� ���������� ����������� �� ������������� (100� & 50�)
// �� �����: unsigned long - �������������, ��� 50-������ ��� ��� ��������� (�)
// �� ������: unsigned int - �����������. ���� =0 �� ������.
// (Proc_TSP0)

unsigned int Pt_3910 (unsigned long resistance)
{
  unsigned long result;
  result = (resistance * 29153UL) / 8UL;
  result = (result + 2206179461UL) / 65536UL;
  result = (result * resistance) /  64UL;
  result = (result + 630046652UL) / 16384UL;
  result = (result * resistance);
  result = (result + 197216040UL) / 65536UL;
  return (unsigned int) result;
}

//============================================================================
// ��������� ���������� ����������� �� ������������� (100�)
// �� �����: unsigned long - �������������, ��� 50-������ ��� ��� ��������� (�)
// �� ������: unsigned int - �����������. ���� =0 �� ������
// (Proc_TSP1)

unsigned int Cu_4280 (unsigned long resistance)
{
 
  unsigned long result;

  if (resistance >= 38289UL) // ok
    {
      result = (76599UL * resistance) / 131072UL;
      result = (result + 3939UL);
      return (unsigned int) result;
    }

  if (resistance < 38285UL)
    {
      result = (resistance * 1815UL) / 65536UL;
      result = (result + 36033UL);
      result = (result * resistance) / 65536UL;
      result = (result + 4647UL);
      return (unsigned int) result;
    }

  return (26315); // ��� 38285...38288 263.15 �������� (-10 �������� �������)
}


//==========================================================================
// ��������� ���������� ����������� �� ������������� (Pt100 & Pt50)
// �� �����: unsigned long - �������������, ��� 50-������ ��� ��� ��������� (�)
// �� ������: unsigned int - �����������. ���� =0 �� ������.
// (Proc_TSP2)

unsigned int Pt_3850 (unsigned long resistance)
{
  unsigned long result;
  result = (resistance * 57080UL) / 4UL;
  result = (result + 902791633UL) / 65536UL;
  result = (result * resistance) /  64UL;
  result = (result + 653227277UL) / 16384UL;
  result = (result * resistance + 161076900UL) / 65536UL;
  return (unsigned int) result;
}

//==========================================================================
// ��������� ���������� ����������� �� ������������� (Cu100 & Cu50)
// �� �����: unsigned long - �������������, ��� 50-������ ��� ��� ��������� (�)
// �� ������: unsigned int - �����������. ���� =0 �� ������.
// (Proc_TSP3)

unsigned int Cu_4260 (unsigned long resistance)
{
  unsigned long result;
  result = (resistance * 38446UL) / 65536UL;
  result = (result + 3850UL);
  return (unsigned int) result;
}


//=====================================================================
// ��������� ���������� ����������� �������� �� ������ ����
// unsigned int InputData - ������� ������ ���
// unsigned char channel - ����� 0...3
// volatile unsigned char AverageKI[] - �� ������� ��������� ��������� (0...15)
// ���������� ����������:
// volatile unsigned char AverageIndexI[] - ������� �������� �������� � ������ �� ������ ������
// volatile unsigned char BufFullI[] - ����-��������� ������������� ������
// �� ������ - unsigned int, ������� ��������

unsigned int MovingAverage_I (unsigned int InputData, unsigned char channel)
{

  if (BSI_Settings.AverageKI[channel]==0 || BSI_Settings.AverageKI[channel]==1)
    {
      return(InputData);  // ���� ���������� ��������� ���������� = 0 ��� 1 �� ����� ������ ������� ������
    }

  unsigned char index=AverageIndexI[channel]; // Index - ��������� �� ������� ��������
  unsigned long dividend=AverageSumI[channel]; // ��������� � ������� �����
  unsigned long divisor=AverageIndexI[channel]; // �� ������� �����
  divisor++;

  if (AverageBufFullI[channel]) // ���� ����� �����, �� ������� �������� ������ ��������
    {
      divisor = BSI_Settings.AverageKI[channel]; // �������� ����� ������������� �������� ���-�� ���������
      dividend = dividend - AverageBufI[channel][index]; // �������� ������ ��������
    }

  AverageBufI[channel][index] = InputData; // ���������
  dividend = dividend + InputData;
  AverageSumI[channel] = dividend;

  unsigned long result = (dividend / divisor); // � ����� �������� ������ � ������
  
  AverageIndexI[channel]++;
  
  unsigned char bb = BSI_Settings.AverageKI[channel] & 0x0F; // ���� � ���������� ���� �������� ������� ��������, �� ���������
  if ( AverageIndexI[channel] >= bb ) // ��������, ���������� �� �����
    {
      AverageIndexI[channel] = 0; // ���������� ���������
      AverageBufFullI[channel] = 0xFF; // ������������� ���� � ���, ��� ����� ��������
    }

  return (unsigned int) result;

}


//=============================================================================
// ��������� ���������� ����������� �������� �� ������ �����������
// unsigned int InputData - ������� ������ ���
// unsigned char channel - ����� 0...1
// volatile unsigned char AverageKT[] - �� ������� ��������� ��������� (0...15)
// ���������� ����������:
// volatile unsigned char AverageIndexT[] - ������� �������� �������� � ������ �� ������ ������
// volatile unsigned char BufFullT[] - ����-��������� ������������� ������
// �� ������ - unsigned int, ������� ��������

unsigned int MovingAverage_T (unsigned int InputData, unsigned char channel)
{

  if (BSI_Settings.AverageKT[channel]==0 || BSI_Settings.AverageKT[channel]==1)
    {
      return(InputData);  // ���� ���������� ��������� ���������� = 0 ��� 1 �� ����� ������ ������� ������
    }

  unsigned char index = AverageIndexT[channel]; // Index - ��������� �� ������� ��������
  unsigned long dividend = AverageSumT[channel]; // ��������� � ������� �����
  unsigned long divisor = AverageIndexT[channel]; // �� ������� �����
  divisor++;

  if (AverageBufFullT[channel]) // ���� ����� �����, �� ������� �������� ������ ��������
    {
      divisor=BSI_Settings.AverageKT[channel]; // �������� ����� ������������� �������� ���-�� ���������
      dividend = dividend - AverageBufT[channel][index];
    }

  AverageBufT[channel][index] = InputData; // ���������
  dividend = dividend + InputData;
  AverageSumT[channel] = dividend;

  unsigned long result = (dividend / divisor);

  AverageIndexT[channel]++;
  AverageIndexT[channel]&=0x0F; // ������������ ������ �� ����� 15

  unsigned char bb = BSI_Settings.AverageKT[channel] & 0x0F;
  if ( AverageIndexT[channel] >= bb ) // ��������, ���������� �� �����
    {
      AverageIndexT[channel] = 0; // ���������� ���������
      AverageBufFullT[channel] = 0xFF; // ������������� ���� � ���, ��� ����� ��������
    }

  return (unsigned int) result;
  
}


//========================================================================
// ������������� ���������� ��� ����������� ��������, ������� �������
// � ����������� ����������. ����� I.
// ����� ������� �������� MovingAvegage ���������� �������� ���������
// ����������.

void InitMovingAverage_I (void)
{
  // ������� ���������� ������� �������� ����������
  // 16*6=96 ���� AverageBuf[BSI_N_ICHANNEL+BSI_N_TCHANNEL][MAX_AVERAGE_WORDS];

  unsigned char i;
  for (i=0;i<MAX_AVERAGE_WORDS;i++)
    {
      AverageBufI[0][i]=0;
      AverageBufI[1][i]=0;
      AverageBufI[2][i]=0;
      AverageBufI[3][i]=0;      
    }

  for (i=0;i<(BSI_N_ICHANNEL);i++)
    {
      AverageIndexI[i]=0;   // ��������� ���-�� ��������� � ������ = 0
      AverageSumI[i]=0;  // ����� �� ������ ���������� = 0
      AverageBufFullI[i]=0;     // ���� ���������� ������ = 0
    }

}

//========================================================================
// ������������� ���������� ��� ����������� ��������, ������� �������
// � ����������� ����������. ����� T.
// ����� ������� �������� MovingAvegage ���������� �������� ���������
// ����������.

void InitMovingAverage_T (void)
{
  // ������� ���������� ������� �������� ����������
  // 16*6=96 ���� AverageBuf[BSI_N_ICHANNEL+BSI_N_TCHANNEL][MAX_AVERAGE_WORDS];

  unsigned char i;
  for (i=0;i<MAX_AVERAGE_WORDS;i++)
    {
      AverageBufT[0][i]=0;
      AverageBufT[1][i]=0;
    }

  for (i=0;i<(BSI_N_TCHANNEL);i++)
    {
      AverageIndexT[i]=0;   // ��������� ���-�� ��������� � ������ = 0
      AverageSumT[i]=0;  // ����� �� ������ ���������� = 0
      AverageBufFullT[i]=0;     // ���� ���������� ������ = 0
    }

}


//=====================================================================
// ������� � ������������� ���������� ��������� ����������� ��������
// ��� ����������� �������� ������.
// unsigned char channel - ���������� ����� 0...3

void ClearMovingAverage_I (unsigned char channel)
{
  AverageIndexI[channel]=0;           // ��������� ���-�� ��������� � ������ = 0
  AverageSumI[channel]=0;  // ����� �� ������ ���������� = 0
  AverageBufFullI[channel]=0;     // ���� ���������� ������ = 0

  unsigned char i;
  for (i=0;i<(MAX_AVERAGE_WORDS);i++)
    {  
      AverageBufI[channel][i];
    }

}

//=====================================================================
// ������� � ������������� ���������� ��������� ����������� ��������
// ��� ����������� �������������� ������.
// unsigned char channel - ���������� ����� 0...1

void ClearMovingAverage_T (unsigned char channel)
{
  AverageIndexT[channel]=0;           // ��������� ���-�� ��������� � ������ = 0
  AverageSumT[channel]=0;  // ����� �� ������ ���������� = 0
  AverageBufFullT[channel]=0;     // ���� ���������� ������ = 0

  unsigned char i;
  for (i=0;i<(MAX_AVERAGE_WORDS);i++)
    {
      AverageBufT[channel][i];
    }
}

//=====================================================================
// ������������� ����������� �� ������ �� ������� �������
// �� �����: unsigned char channel - ����� ������, �� �������� �������� ����������������
// 0...3 - ������� ������
// �� ������: ����������� ��������� unsigned int

unsigned int Temperature_Compensation_I (unsigned char channel)
{
  signed char Tk;
  if (BSI_ResultEx.BSI_Results.Status.Flag.signTintern==1) // ���� ���� ���������������� ����������, ��
    {
      Tk=BSI_Settings.KHI[channel]; // �������� ����������� Kh (20...60 ��������)
    }
  else
    {
      Tk=BSI_Settings.KLI[channel]; // ����� �������� ����������� Kl (-40...20 ��������)
    }

  unsigned long ads=BSI_ResultEx.CodeIAverage[channel];
  unsigned long result;
  result = ads * TerIn_2dT; // ������� ������� �������� �� ��������� ������ �
  result = (result / 65536);
  result = (result * Tk);
  result = (result / 64);
  result = (result + ads);  // ������������� ��������� ��������� � ������ ���������� Tk
  return (unsigned int) result;
}

//=====================================================================
// ������������� ����������� �� ������ �� ������������� �������
// �� �����: unsigned char channel - ����� ������, �� �������� �������� ����������������
// 0...1 - ������������ ������
// �� ������: ����������� ��������� unsigned int

unsigned int Temperature_Compensation_T (unsigned char channel)
{
  signed char Tk;
  if (BSI_ResultEx.BSI_Results.Status.Flag.signTintern==1) // ���� ���� ���������������� ����������, ��
    {
      Tk=BSI_Settings.KHT[channel]; // �������� ����������� Kh (20...60 ��������)
    }
  else
    {
      Tk=BSI_Settings.KLT[channel]; // ����� �������� ����������� Kl (-40...20 ��������)
    }

  unsigned long ads=BSI_ResultEx.CodeTAverage[channel];
  unsigned long result; // ��������� 32-������

  result = ads * TerIn_2dT; // ������� ������� �������� �� ��������� ������ �
  result = (result / 65536);  // ������������� ��������� ��������� � ������ ���������� Tk
  result = (result * Tk);
  result = (result / 64);
  result = (result + ads);
  return (unsigned int) result; // ��������� �� ������ ���������� � 16 �����
}


//======================================================================
// ���������� ������ �������� ������
// �� ����� - unsigned char channel 0..3
// ������ ������������ ��� unsigned int

unsigned int Matematic_I (unsigned char channel)
{
  unsigned int corrected_ADC_value=Temperature_Compensation_I(channel);

  unsigned long result;
  unsigned long a=BSI_Settings.AI[channel];
  signed char b=BSI_Settings.BI[channel];
  result = (corrected_ADC_value + b);  // ��������� 32-������
  result = (result * a); // ��� ���������/������� �������� � ������
  result = (result / 65536UL);
  return (unsigned int) result;
}


//=================================================
// ������ ������������� R � � (�����������)

unsigned int Matematic_RT (unsigned char channel)
{
//===============================================
// ���������� ������ �������������� ������
// �� ����� - unsigned char channel 0..1
// ������ ������������ ��� unsigned int
// === MATEMATIC_R === ���������� ������������� R

  unsigned long corrected_ADC_value=Temperature_Compensation_T(channel);
  unsigned long a=BSI_Settings.AT[channel];
  signed char b=BSI_Settings.BT[channel];
  unsigned long result = (corrected_ADC_value * 65536UL) / a; // ��������� 32-������
  
  unsigned int  resultR = (unsigned int)result + b;
  
  BSI_ResultEx.R[channel] = resultR; // ��������� ��������� ������������ �������������

//=========== Matematic_TSP ====================

  unsigned char tsp=BSI_Settings.typeT[channel]; // ��� ���������������������
  
  if (tsp&0x04) // ���� ������� ��� ���� ��� = 1 �� 50-����� ��������������������
    {
      if (resultR >= 32768) // ���� ���� ������������ �� ���������� 0 - ������ (�������� �� 2 �� �����)
        {
          return (0);
        }
      else  // ����� ���������
        {
          resultR = resultR * 2; 
        }
    }
  
  switch (tsp&0x03) // ��������� ������ ���� 0,1,2,3
    {
      case TSENS_100P: // TSP0 100� 1.3910 (TSP0+4 50� 1.3910)
        {
          return Pt_3910(resultR);
        }
      case TSENS_100M: // TSP1 100� 1.4280 (TSP1+4 50� 1.4280)
        {
          return Cu_4280(resultR);
        }
      case TSENS_Pt100: // TSP2 Pt100 1.3850 (TSP2+4 Pt50 1.3850)
        {
          return Pt_3850(resultR);
        }
      case TSENS_Cu100: // TSP3 Cu100 1.4260 (TSP3+4 Cu50 1.4260)
        {
          return Cu_4260(resultR);
        }
    }
return 0;
}

//********************************************************************************************************************************************************
//***********                Z channel            ********************************************************************************************************
//********************************************************************************************************************************************************

//==========================================================================
// ��������� ���������� ������.
// �������� �� �������-�������� ������� � �������� �������� ����������

void Calc_Z1(void)
{
  unsigned long result;
  if (ICR1A >= ICR1B) // >=
    {
      result = (ICR1A - ICR1B); // ��������� ����� ������� � ���, ��������� �� 8
    }
  else
    {
      result = (ICR1B - ICR1A);
    }

  if (result < (BSI_Settings.ZTime[0].min<<3)) // �������� Tmin1*8
    {
      if (nError1) // ���� �� �������� ����, ��
        {
          nError1--;  // ������� ������ ��������� � �������
          return;
        }
      else // Brk1==0
        {
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmin1=0;
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ1us=0;
          return;
        }
    }

  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmin1=1; // ��������� ����� Tmin1 OK

  if (result >= (BSI_Settings.ZTime[0].max<<3)) // �������� Tmax1*8
    {
      if (nError1!=0)
        {
          nError1--;
          return;
        }
      else
        {
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmax1=0;
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ1us=0;
          return;
        }
    }

  BSI_ResultEx.BSI_Results.Z_Tus[0] = result;
  
  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmax1=1;

  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ1us=1;
  
  nError1 = BSI_Settings.nZ_errors_max[0];
}

//============================================================================

void Calc_Z2(void)
{
  unsigned long result;
  if (ICR2A >= ICR2B)
    {
      result = (ICR2A - ICR2B);
    }
  else
    {
      result = (ICR2B - ICR2A);
    }

  if (result < (BSI_Settings.ZTime[1].min<<3) ) // �������� Tmin2
    {
      if (nError2!=0)
        {
          nError2--;
          return;
        }
      else
        {
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmin2=0;
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ2us=0;
          return;
        }
    }

  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmin2=1;

  if (result >= (BSI_Settings.ZTime[1].max<<3) ) // �������� Tmax1
    {
      if (nError2!=0)
        {
          nError2--;
          return;
        }
      else
        {
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmax2=0;
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ2us=0;
          return;
        }
    }

  BSI_ResultEx.BSI_Results.Z_Tus[1] = result;
  
  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmax2=1;
  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ2us=1;

  
  nError2 = BSI_Settings.nZ_errors_max[1];
}

//=================================================================================
// ��������� ���������� ���������� ������
// prescaler - 0...7
// Watchdog timer �� ��������� ������ �������, ����� ��������� ���������� �����
// �� ����� ������ ������ ��������, ����� ��� ��� �� �����.
// 111 = 2,1  �
// 110 = 1,0  � 
// 101 = 0,52 � 
// 100 = 0,26 c
// 011 = 0,13 � 

__monitor void WDT_Prescaler_Change(unsigned char prescaler)
{
  __watchdog_reset();
  // Start timed equence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // Set new prescaler(time-out) value
  WDTCR = (1<<WDE) | prescaler;
}

//===================================================================
// �������� ����������� ������� ���� �������� �� ����� ���� �������
// ������ � �� � �����. ��������� ������ ���� ����.

unsigned int CheckConfigCRC (void)
{
  return CRC16((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings));
}
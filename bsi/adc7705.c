// #include "adc7705.h"


/*
void SPI_Send (void)
{
  SPDR=0xFF;
  while(!SPSR_SPIF);
}
*/


void Reset_ADC1 (void)
{
  ADC_PORT&=~(1<<RST1);
  delay_us(4);
  ADC_PORT|=(1<<RST1);
}

void Reset_ADC2 (void)
{
  ADC_PORT&=~(1<<RST2);
  delay_us(4);
  ADC_PORT|=(1<<RST2);
}

void Reset_ADC3 (void)
{
  ADC_PORT&=~(1<<RST3);
  delay_us(4);
  ADC_PORT|=(1<<RST3);
}

/*
****************************************************************************
**************                                             *****************
**************                 ������ ���                  *****************
**************                                             *****************
****************************************************************************
*/

//===============================================================
// ������ ��� 1 ����� 1

void StartConversion_AD11 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS1);  // CS1=0 �������� ���1
//  delay_us(1);
  
  SPDR=Command7705_Clk+0; // Clock Register
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[0]; // 0..3
  tmp&=0x03; // �� ������ ������ ������� ������ ���� ������
  tmp|=Command7705_C_Clk; // 0x04
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS1); // CS1=1 ��������� ���1
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS1);  // CS1=0 �������� ���1
//  delay_us(1);
  
  SPDR=Command7705_SetT+0;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[0]<<3);
  tmp&=0x38; // �� ������ ������ ������� ������ ���� ������
  tmp|=(BSI_Settings.BiUniPolar.bit.BU0<<B_U);
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS1); // CS1=1 ��������� ���1  

  //-----------------------------------------------
}

//====================================================================
// ������ �� ��� 1 ����� 1
// ���������� unsigned int - ��������� � ����� ����

unsigned int ReadData_AD11 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS1);  // CS1=0 �������� ���1
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // ������ ������� ���� ����������

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // ������ ������� ���� ����������

  ADC_PORT|=(1<<CS1); // CS1=1 ��������� ���1  

  //--------------------------------------------

  return (tmpH<<8|tmpL); // ����������� ���� ���� � ����� � ����������
}


//===============================================================
// ������ ��� 1 ����� 2

void StartConversion_AD12 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS1);  // CS1=0 �������� ���1
//  delay_us(1);
  
  SPDR=Command7705_Clk+1;
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[1];
  tmp&=0x03; // �� ������ ������ ������� ������ ���� ������
  tmp|=Command7705_C_Clk;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS1); // CS1=1 ��������� ���1
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS1);  // CS1=0 �������� ���1
//  delay_us(1);
  
  SPDR=Command7705_SetT+1;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[1]<<3);
  tmp&=0x38; // �� ������ ������ ������� ������ ���� ������
  tmp|=(BSI_Settings.BiUniPolar.bit.BU1<<B_U);
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS1); // CS1=1 ��������� ���1  

  //-----------------------------------------------
}

//====================================================================
// ������ �� ��� 1 ����� 2
// ���������� unsigned int - ��������� � ����� ����

unsigned int ReadData_AD12 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS1);  // CS1=0 �������� ���1
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // ������ ������� ���� ����������

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // ������ ������� ���� ����������

  ADC_PORT|=(1<<CS1); // CS1=1 ��������� ���1  

  //--------------------------------------------

  return (tmpH<<8|tmpL); // ����������� ���� ���� � ����� � ������
}

//===================================================
// ���������� �������������� �������� ��� 1

void Conversion_I1 (void)
{
  Channel_NI12=0;  // ����� ������ 1 (=0)
  Reset_ADC1();
  StartConversion_AD11();
}

//===================================================
// ���������� �������������� �������� ��� 2

void Conversion_I2 (void)
{
  Channel_NI12=1;  // ����� ������ 2 (=1)
  Reset_ADC1();
  StartConversion_AD12();
}

//=========================================================================
// ���������� ����� �������� ���������� ��� �� ���� /DRDY.
// ��������� �������� ������ �� ��������� ����� NUMKAN.NI12 ������ ���1,
// ���������� �������� �� ���������� ������� ��������� (��������, �������),
// ��������� ��������� ���������� ����������� ��������,
// ������ ����������� �������������� ������� � ������������� �����. �����
// ���������� �/��� ������.
// �� ������ ��������� ��������� ������ ����� ���� �� ���, ����� ���
// ��������� ������ ������� ���������� � ������ ������.
// ����� ������� ��� ������ ������ ��������� ����������� � �����������
// ����������� ��� ������ ���1.

void Calc_ADC1 (void)
{
  unsigned int tmp; // ���������� ��� �������� �������� ���
  if (Channel_NI12) // ���� ��� ������ ���������� �� ��� ������ �����
    {
      // ====== ����� 2 of ADC1 ========

      tmp=ReadData_AD12();  // ������ �� ���1 ����� 2
      BSI_ResultEx.CodeI[1]=tmp; // ��������� ����� �� ADC_I[1]
      ReadyDataADC.DI2=1; // ��������� ����� "������ ������" �� ������ 2
      
      // � tmp - ��������� ��� ��� ����������� �������� ��������
      if ( (BSI_Settings.UPHI[1] >= tmp) && (tmp >= BSI_Settings.UPLI[1]) ) // �������� �� ���������� �������� ��� (������� "������" � "������")
        {
          BSI_ResultEx.CodeIAverage[1]=MovingAverage_I(tmp,1); // ���� ��� ��� � �������� �������, �� ��������� ���������� �������
          BSI_ResultEx.BSI_Results.I[1]=Matematic_I(1);  // � ������ ����������� ���������� �� ������ 2
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI2=1;         // Set OK Flag
        }
      else  // ����� (��� ������ ���� �� ���������� ������) ��� ������������� ������ ����� ����������� ��������
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyI2) // ���� ��� ���������� =1 �� ������ �����
            {
              ClearMovingAverage_I(1); // ������ ����� ����������� �������� ����� 2
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI2=0;   // ����� ���������� ��� DI2
        }
    }
  else  // ����� ������ �����
    {
      // ====== ����� 1 of ADC1 ========
      
      tmp=ReadData_AD11();
      BSI_ResultEx.CodeI[0]=tmp; // ��� ������� ������ ������ ������� =0, ��������� ���������
      ReadyDataADC.DI1=1; // ��������� ����� "������ ������" �� ������ 1
//======
//      ADC_I[0]=0x28E1; Debug Value
//======

      if ( (BSI_Settings.UPHI[0] >= tmp) && (tmp >= BSI_Settings.UPLI[0]) )
        {
          BSI_ResultEx.CodeIAverage[0]=MovingAverage_I(tmp,0); // ���� ��� ��� � �������� �������, �� ��������� ���������� �������
          BSI_ResultEx.BSI_Results.I[0]=Matematic_I(0);  // ���������� ���� �� ������� ������ (������ ����)
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI1=1; // ���������� ���� ��
        }
      else
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyI1) // ���� ��� DI1=1 �� ������ �����
            {
              ClearMovingAverage_I(0); // ������ ����� ����������� �������� ����� 1
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI1=0;   // ���������� ��� DI1 (������ ������)
        }
    }

  //----------------
  if (Channel_NI12) // ���� ��� ������ ����� 2, �� ��������� ������
    {
      Conversion_I1();
    }
  else            // ����� ��� ������ ������, ��������� ������ �����
    {
      Conversion_I2();
    }
}

/*
****************************************************************************
**************                                             *****************
**************                 ������ ���                  *****************
**************                                             *****************
****************************************************************************
*/


//===============================================================
// ������ ��� 2 ����� 1 (������ ������� �����)

void StartConversion_AD21 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS2);  // CS2=0 �������� ���2
//  delay_us(1);
  
  SPDR=Command7705_Clk+0; // Clock Register
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[0+2]; // ������� 2 - ������ ��� (�������� 0...3)
  tmp&=0x03; // �� ������ ������ ������� ������ ���� ������
  tmp|=Command7705_C_Clk; // 0x04
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS2); // CS2=1 ��������� ���2
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS2);  // CS2=0 �������� ���2
//  delay_us(1);
  
  SPDR=Command7705_SetT+0;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[0+2]<<3); // ������� 2 - ������ ���
  tmp&=0x38; // �� ������ ������ ������� ������ ���� ������
  tmp|=(BSI_Settings.BiUniPolar.bit.BU2<<B_U);
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS2); // CS2=1 ��������� ���2

  //-----------------------------------------------
}

//====================================================================
// ������ �� ��� 2 ����� 1
// ���������� unsigned int - ��������� � ����� ����

unsigned int ReadData_AD21 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS2);  // CS2=0 �������� ���2
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // ������ ������� ���� ����������

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // ������ ������� ���� ����������

  ADC_PORT|=(1<<CS2); // CS2=1 ��������� ���2

  //--------------------------------------------

  return (tmpH<<8|tmpL); // ����������� ���� ���� � ����� � ����������
}


//===============================================================
// ������ ��� 2 ����� 2 (��������� ������� �����)

void StartConversion_AD22 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS2);  // CS2=0 �������� ���2
//  delay_us(1);
  
  SPDR=Command7705_Clk+1;
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[1+2]; // ������� 2 - ������ ���
  tmp&=0x03;
  tmp|=Command7705_C_Clk;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS2); // CS2=1 ��������� ���2
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS2);  // CS2=0 �������� ���2
//  delay_us(1);
  
  SPDR=Command7705_SetT+1;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[1+2]<<3); // ������� 2 - ������ ���
  tmp&=0x38; // �� ������ ������ ������� ������ ���� ������
  tmp|=(BSI_Settings.BiUniPolar.bit.BU3<<B_U);
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS2); // CS2=1 ��������� ���2

  //-----------------------------------------------
}

//====================================================================
// ������ �� ��� 2 ����� 2
// ���������� unsigned int - ��������� � ����� ����

unsigned int ReadData_AD22 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS2);  // CS2=0 �������� ���2
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // ������ ������� ���� ����������

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // ������ ������� ���� ����������

  ADC_PORT|=(1<<CS2); // CS2=1 ��������� ���2

  //--------------------------------------------

  return (tmpH<<8|tmpL); // ����������� ���� ���� � ����� � ������
}

//===================================================
// ���������� �������������� �������� ������ 3 ���2

void Conversion_I3 (void)
{
  Channel_NI34=0;  // ����� ������ 1 (���=0)
  Reset_ADC2();
  StartConversion_AD21();
}

//===================================================
// ���������� �������������� �������� ������ 4 ���2

void Conversion_I4 (void)
{
  Channel_NI34=1;  // ����� ������ 2 (���=1)
  Reset_ADC2();
  StartConversion_AD22();
}

//=========================================================================
// ���������� ����� �������� ���������� ��� �� ���� /DRDY.
// ��������� �������� ������ �� ��������� ����� NUMKAN.NI34 ������ ���2,
// ���������� �������� �� ���������� ������� ��������� (��������, �������),
// ��������� ��������� ���������� ����������� ��������,
// ������ ����������� �������������� ������� � ������������� �����. �����
// ���������� �/��� ������.
// �� ������ ��������� ��������� ������ ����� ���� �� ���, ����� ���
// ��������� ������ ������� ���������� � ������ ������.
// ����� ������� ��� ������ ������ ��������� ����������� � �����������
// ����������� ��� ������ ���2.

void Calc_ADC2 (void)
{
  unsigned int tmp; // ���������� ��� �������� �������� ���
  if (Channel_NI34) // ���� ��� ������ ���������� �� ��� ������ ���
    {
      // ====== ����� 2 of ADC2 ========

      tmp=ReadData_AD22();  // ������ �� ���2 ����� 2
      BSI_ResultEx.CodeI[1+2]=tmp; // ��������� ����� �� ADC_I[] ������� 2 �.�. ���2
      ReadyDataADC.DI4=1; // ��������� ����� "������ ������" �� �������� ������ 4
  
      // � tmp - ��������� ��� ��� ����������� �������� ��������
      if ( (BSI_Settings.UPHI[1+2] >= tmp) && (tmp >= BSI_Settings.UPLI[1+2]) ) // �������� �� ����������
                                                // �������� ��� (������� "������" � "������")
        {
          BSI_ResultEx.CodeIAverage[1+2]=MovingAverage_I(tmp,1+2); // ���� ��� ��� � �������� �������, �� ��������� ���������� �������
          BSI_ResultEx.BSI_Results.I[1+2]=Matematic_I(1+2);  // � ������ ����������� ���������� �� ������ 4
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI4=1;         // Set OK Flag
        }
      else  // ����� (��� ������ ���� �� ���������� ������) ��� ������������� 
            // ������ ����� ����������� ��������
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyI4) // ���� ��� DI4=1 �� ������ �����
            {
              ClearMovingAverage_I(1+2); // ������ ����� ����������� �������� ����� 4
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI4=0;   // ����� ���������� ��� DI4
        }
    }
  else  // ����� ������ ���
    {
      // ====== ����� 1 of ADC2 ========
      
      tmp=ReadData_AD21();
      BSI_ResultEx.CodeI[0+2]=tmp; // ��� ������� ������ ������ ������� =0, ��������� ���������
      ReadyDataADC.DI3=1; // ��������� ����� "������ ������" �� ������ 3

      if ( (BSI_Settings.UPHI[0+2] >= tmp) && (tmp >= BSI_Settings.UPLI[0+2]) )
        {
          BSI_ResultEx.CodeIAverage[0+2]=MovingAverage_I(tmp,0+2); // ���� ��� ��� � �������� �������, �� ��������� ���������� �������
          BSI_ResultEx.BSI_Results.I[0+2]=Matematic_I(0+2);  // ���������� ���� (������ ����)
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI3=1; // ���������� ���� ��
        }
      else
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyI3) // ���� ��� DI=1 �� ������ �����
            {
              ClearMovingAverage_I(0+2); // ������ ����� ����������� ��������
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI3=0;   // ���������� ��� DI (������ ������)
        }
    }

  //----------------
  if (Channel_NI34) // ���� ��� ������ ����� 4, �� ��������� ������
    {
      Conversion_I3();
    }
  else            // ����� ��� ������ ������, ��������� ����� ����� ������
    {
      Conversion_I4();
    }
}

/*
****************************************************************************
**************                                             *****************
**************                 ������ ���                  *****************
**************                                             *****************
****************************************************************************
*/


//===============================================================
// ������ ��� 3 ����� 1 (������ ������������� �����)

void StartConversion_AD31 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS3);  // CS3=0 �������� ���3
//  delay_us(1);
  
  SPDR=Command7705_Clk+0; // Clock Register
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[0+4]; // ������� 4 - ������ ��� (�������� 0...3)
  tmp&=0x03; // �� ������ ������ ������� ������ ���� ������
  tmp|=Command7705_C_Clk; // 0x04
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS3); // CS3=1 ��������� ���3
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS3);  // CS3=0 �������� ���3
//  delay_us(1);
  
  SPDR=Command7705_SetT+0;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[0+4]<<3); // ������� 4 - ������ ���
  tmp&=0x38; // �� ������ ������ ������� ������ ���� ������
  tmp|=(BSI_Settings.BiUniPolar.bit.BU4<<B_U); // BU4 - ADC3 channel0
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS3); // CS3=1 ��������� ���3

  //-----------------------------------------------
}

//====================================================================
// ������ �� ��� 3 ����� 1
// ���������� unsigned int - ��������� � ����� ����

unsigned int ReadData_AD31 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS3);  // CS3=0 �������� ���3
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // ������ ������� ���� ����������

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // ������ ������� ���� ����������

  ADC_PORT|=(1<<CS3); // CS3=1 ��������� ���3

  //--------------------------------------------

  return (tmpH<<8|tmpL); // ����������� ���� ���� � ����� � ����������
}


//===============================================================
// ������ ��� 3 ����� 2 (������ ������������� �����)

void StartConversion_AD32 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS3);  // CS3=0 �������� ���3
//  delay_us(1);
  
  SPDR=Command7705_Clk+1;
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[1+4]; // ������� 4 - ������ ���
  tmp&=0x03;
  tmp|=Command7705_C_Clk;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS3); // CS3=1 ��������� ���3
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS3);  // CS3=0 �������� ���3
//  delay_us(1);
  
  SPDR=Command7705_SetT+1;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[1+4]<<3); // ������� 4 - ������ ���
  tmp&=0x38; // �� ������ ������ ������� ������ ���� ������
  tmp|=(BSI_Settings.BiUniPolar.bit.BU5<<B_U); // BU5 - ADC3 channel 1
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS3); // CS3=1 ��������� ���3

  //-----------------------------------------------
}

//====================================================================
// ������ �� ��� 3 ����� 2
// ���������� unsigned int - ��������� � ����� ����

unsigned int ReadData_AD32 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS3);  // CS3=0 �������� ���3
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // ������ ������� ���� ����������

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // ������ ������� ���� ����������

  ADC_PORT|=(1<<CS3); // CS3=1 ��������� ���3

  //--------------------------------------------

  return (tmpH<<8|tmpL); // ����������� ���� ���� � ����� � ������
}

//===================================================
// ���������� �������������� �������������� ������ 1 ���3

void Conversion_T1 (void)
{
  Channel_NT12=0;  // ����� ������ 1 (���=0)
  Reset_ADC3();
  StartConversion_AD31();
}

//===================================================
// ���������� �������������� �������������� ������ 2 ���3

void Conversion_T2 (void)
{
  Channel_NT12=1;  // ����� ������ 2 (���=1)
  Reset_ADC3();
  StartConversion_AD32();
}

//=========================================================================
// ���������� ����� �������� ���������� ��� �� ���� /DRDY.
// ��������� �������� ������ �� ��������� ����� NUMKAN.NT12 ������ ���3,
// ���������� �������� �� ���������� ������� ��������� (��������, �������),
// ��������� ��������� ���������� ����������� ��������,
// ������ ����������� �������������� ������� � ������������� �����. �����
// ���������� �/��� ������.
// �� ������ ��������� ��������� ������ ����� ���� �� ���, ����� ���
// ��������� ������ ������� ���������� � ������ ������.
// ����� ������� ��� ������ ������ ��������� ����������� � �����������
// ����������� ��� ������ ���3.

void Calc_ADC3 (void)
{
  unsigned int tmp; // ���������� ��� �������� �������� ���
  if (Channel_NT12) // ���� ��� ������ ���������� �� ��� ������ ���
    {
      // ====== ����� 2 of ADC3 ========

      tmp=ReadData_AD32();  // ������ �� ���3 ����� 2
//======
//      tmp=34272; // Debug Value
//======
      BSI_ResultEx.CodeT[1]=tmp; // ��������� ����� �� tmp
      ReadyDataADC.DT2=1; // ��������� ����� "������ ������" �� �������������� ������ 2
  
      // � tmp - ��������� ��� ��� ����������� �������� ��������
      if ( (BSI_Settings.UPHT[1] >= tmp) && (tmp >= BSI_Settings.UPLT[1]) ) // �������� �� ���������� �������� ��� (������� "������" � "������")
        {
          BSI_ResultEx.CodeTAverage[1]=MovingAverage_T(tmp,1); // ���� ��� ��� � �������� �������, �� ��������� ���������� �������
          BSI_ResultEx.BSI_Results.T[1]=Matematic_RT(1);  // � ������ ����������� ���������� �� �������������� ������ 2
          if (BSI_ResultEx.BSI_Results.T[1]!=0) // �������� �� 0 (������)
            {
              BSI_ResultEx.BSI_Results.Status.Flag.ReadyT2=1;         // Set OK Flag
            }
          else
            {
              BSI_ResultEx.BSI_Results.Status.Flag.ReadyT2=0; // ���� ������ ������������
            }
        }
      else  // ����� (��� ������ ���� �� ���������� ������) ��� ������������� 
            // ������ ����� ����������� ��������
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyT2) // ���� ��� DT2=1 �� ������ �����
            {
              ClearMovingAverage_T(1); // ������ ����� ����������� �������� ����� 6
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyT2=0;   // ����� ���������� ��� DT
        }
    }
  else  // ����� ������ ���
    {
      // ====== ����� 1 of ADC3 ========
      
      tmp=ReadData_AD31();
      BSI_ResultEx.CodeT[0]=tmp; // ��� ������� ������ ������ ������� =0, ��������� ���������
      ReadyDataADC.DT1=1; // ��������� ����� "������ ������" �� �������������� ������ 1

      if ( (BSI_Settings.UPHT[0] >= tmp) && (tmp >= BSI_Settings.UPLT[0]) )
        {
          BSI_ResultEx.CodeTAverage[0]=MovingAverage_T(tmp,0); // ���� ��� ��� � �������� �������, �� ��������� ���������� �������
          BSI_ResultEx.BSI_Results.T[0]=Matematic_RT(0);  // ���������� ����������� (������ ����)
          if (BSI_ResultEx.BSI_Results.T[0]!=0) // �������� �� 0 (������)
            {
              BSI_ResultEx.BSI_Results.Status.Flag.ReadyT1=1;         // Set OK Flag
            }
          else
            {
              BSI_ResultEx.BSI_Results.Status.Flag.ReadyT1=0; // ���� ������ ������������
            }
        }
      else
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyT1) // ���� ��� DT1=1 �� ������ �����
            {
              ClearMovingAverage_T(0); // ������ ����� ����������� ��������
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyT1=0;   // ���������� ��� DT (������ ������)
        }
    }

  //----------------
  if (Channel_NT12)
    {
      Conversion_T1();  // ���� ��� ������ ����� 2, �� ��������� ������
    }
  else
    {
      Conversion_T2(); // ����� ��� ������ ������, ��������� ����� ����� ���
    }
}
























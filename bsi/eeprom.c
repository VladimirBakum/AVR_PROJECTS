
// ===================== EEPROM Functions & Procedures ===============================

// ������ ����������� ������������ �� EEPROM �������� � BSI_ResultEx.ArchiveStatus
// =00 - �������� ����� � EEPROM,
// =01 - ������ ����� � EEPROM, 
// =02 - ��������� ����� �� ������ ��������.
// �������� ����� - ������ �� EEPROM �������� ����� ��������� ��������, ��������� ��.
// ���� �� �� �������, �� ������� ��� ����� ����� � ������ �� ������ �����, ��������� ��.
// ���� �� ����� �� �������, �� ������� ��� EEPROM �����/��������� � ������ ����� �� ������ ��������,
// ��������� ��. ����� ������ ���������� �������� �� ������������� ���� ������� (0,1 ��� 2).
// ���� �� ������������� � ���, ����� ����� �����.

//__root __eeprom unsigned int OFFSET_ADDR @0x1FC = (unsigned int)&BSI_ResultEx.BSI_Results.Status.word;
//__root __eeprom sBSI_Settings EEP_Settings@ 0x180 ={

// 0x3FFF - ����� ���������� ����� ������ ��������
__root __flash unsigned int ChekSum_EEPROM @ 0x3FFE = 0x55AA;
__root __flash char Version[] @ 0x3FF0 = "BSI_Ver1.0";

// ��������� ���������� � ��������� �� ���������
__root __flash sBSI_Settings PGM_Settings@ PGM_SETTINGS_ADDR ={
  .Dummy=0, // Dummy
  .on_off.bit.I1=1, .on_off.bit.I2=1, .on_off.bit.I3=1, .on_off.bit.I4=1, .on_off.bit.T1=1, .on_off.bit.T2=1, .on_off.bit.Tint=1, // Enable channels
  .AverageKI[0]=AVERAGE_FACTOR_I,
  .AverageKI[1]=AVERAGE_FACTOR_I,
  .AverageKI[2]=AVERAGE_FACTOR_I,
  .AverageKI[3]=AVERAGE_FACTOR_I,
  .AverageKT[0]=AVERAGE_FACTOR_T,
  .AverageKT[1]=AVERAGE_FACTOR_T,
  .typeT[0]=1,
  .typeT[1]=1,
  .AI[0]=50174,
  .AI[1]=50193,
  .AI[2]=50194,
  .AI[3]=50181,
  .AT[0]=54932,
  .AT[1]=54917,
  .BI[0]=1,
  .BI[1]=1,
  .BI[2]=0,
  .BI[3]=0,
  .BT[0]=-14,
  .BT[1]=-27,
  .KHI[0]=0,
  .KHI[1]=0,
  .KHI[2]=0,
  .KHI[3]=0,
  .KHT[0]=0,
  .KHT[1]=0,
  .KLI[0]=0,
  .KLI[1]=0,
  .KLI[2]=0,
  .KLI[3]=0,
  .KLT[0]=0,
  .KLT[1]=0,
  .UPHI[0]=64928,
  .UPHI[1]=64928,
  .UPHI[2]=64928,
  .UPHI[3]=64928,
  .UPHT[0]=52480,
  .UPHT[1]=52480,
  .UPLI[0]=608,
  .UPLI[1]=608,
  .UPLI[2]=608,
  .UPLI[3]=608,
  .UPLT[0]=608,
  .UPLT[1]=608,
  .gainADC[0]=1,
  .gainADC[1]=1,
  .gainADC[2]=1,
  .gainADC[3]=1,    
  .gainADC[4]=6,
  .gainADC[5]=6,
  .fsADC[0]=0,
  .fsADC[1]=0,
  .fsADC[2]=0,
  .fsADC[3]=0,
  .fsADC[4]=0,
  .fsADC[5]=0,
  .BiUniPolar.bit.BU0=1, .BiUniPolar.bit.BU1=1, .BiUniPolar.bit.BU2=1, .BiUniPolar.bit.BU3=1, .BiUniPolar.bit.BU4=1, .BiUniPolar.bit.BU5=1, 
  .FICP.bit.Front1=0, .FICP.bit.Filter1=0, .FICP.bit.Front2=0, .FICP.bit.Filter2=0,
  .nZ_errors_max[0]=2,
  .nZ_errors_max[1]=2,
  .ZTime[0].max=1000000, // 0,1 ��
  .ZTime[0].min=1000, // 1000 ��
  .ZTime[1].max=1000000,
  .ZTime[1].min=1000,
  .ZTimeMeasure[0]=53255,
  .ZTimeMeasure[1]=53255,
  .CheckSum=0x1234  // ������������ ����������� ����� ������������ (����� �������� ���������, ��� ����������� �� PGM � EEPROM ��������� ���������)
};

// ������ � SRAM �������� �� ����������� ����������� ������ &BSI_Settings.
// ��� ������ � ����������� ������� ������������ � EEPROM,
// ���������� ���� ������� ��������� ����� ������������ � EEPROM.
// ����� ������������ ~ 100 ����, ���� ����������� �����, ����������� �� 128 ����.
// ������ ������������ � EEPROM ����� 0x00,0x80,0x100,0x180.
// � EEPROM ����� ��������� �� ������� ����� ������������ (��� �������������).

//=============================================================================
// ��������� ���������� � EEPROM ������������ �� SRAM. ������� ��.
// � ��������� ���� ������ ������������ ������ ���� ����������� ����������� �����.
// ����� ������ �� ���������� ������ �������, ��������� ������� __monitor ��� ����������
// ���������� �� ����� ������.

__monitor void Write_Config_To_EEPROM (unsigned int addr)
{
  
  unsigned char c;
  unsigned char *p=(unsigned char *)(unsigned int)&BSI_Settings;
  unsigned char __eeprom *zp=(unsigned char __eeprom *)addr;
  unsigned char len=sizeof(BSI_Settings);
  while (len--)
    {
      __watchdog_reset();
      PORTD^=(1<<WDI); // ������� watchdog
      c=*p++;
      *zp++=c;
    };
}

// ��������� ������ �� EEPROM ������������ � SRAM
// ����� ������� ���������� ������������� ��������� ����������
// � �������� �������. ��������� ��� ����� - ����������� �����.
// ���������� ���������� �� ����������� ������������:
// ���� =0 �� �� � �������, ����� - ������ � EEPROM � �������.

__monitor void Read_Config_From_EEPROM (unsigned int addr)
{
  unsigned char c;
  unsigned char *p=(unsigned char *)(unsigned int)&BSI_Settings;
  unsigned char __eeprom *zp=(unsigned char __eeprom *)addr;
  unsigned char len=sizeof(BSI_Settings);
  while (len--)
    {
      __watchdog_reset();
      PORTD^=(1<<WDI); // ������� watchdog
      c=*zp++;
      *p++=c;
    };
  
}

// ��������� ������ �� ������ �������� ������������ � SRAM.
// ��������� ��� ����� - ����������� �����, �� �����������.
// addr ���������� ����� 0x3F80 (����� � �� ���������� ���������).

__monitor void Read_Config_From_PGM (unsigned int addr)
{
  unsigned char c;
  unsigned char *p=(unsigned char *)(unsigned int)&BSI_Settings;
  unsigned char __flash *zp=(unsigned char __flash *)addr;
  unsigned char len=sizeof(BSI_Settings);
  while (len--)
    {
      __watchdog_reset();
      PORTD^=(1<<WDI); // ������� watchdog
      c=*zp++;
      *p++=c;
    };
}

/*
// ������������� �������� � ������
// �������������� �� ��� ���, ���� �� ���� �������� �������������
// �������� �� EEPROM.

void InitSettings (void)
{
  
  unsigned char i;
  for (i=0;i<(BSI_N_ICHANNEL);i++)
    {
      BSI_Settings.UPLI[i]=608;  // ������� ���, ������ ������.
      BSI_Settings.UPHI[i]=64928; // ������� ������ �������� ���.
      BSI_Settings.gainADC[i]=1; // Gain=1 ��� ���.
      BSI_Settings.fsADC[i]=0; // ������� ������ ���.
    }
  for (i=0;i<(BSI_N_TCHANNEL);i++)
    {
      BSI_Settings.UPLT[i]=608;
      BSI_Settings.UPHT[i]=52480;
      BSI_Settings.gainADC[i+4]=6;  // ������������� ������ - ���� =6
      BSI_Settings.fsADC[i+4]=0;
    }

  // ������� ���������
  BSI_Settings.AI[0]=50174;
  BSI_Settings.AI[1]=50193;  // ������� ���������� ������������� ��� 1 � 2 �������� ������
  BSI_Settings.AI[2]=50194;
  BSI_Settings.AI[3]=50181;
  BSI_Settings.BI[0]=1;
  BSI_Settings.BI[1]=1;
  BSI_Settings.BI[2]=0;
  BSI_Settings.BI[3]=0;
  BSI_Settings.KLI[0]=BSI_Settings.KLI[1]=0;
  BSI_Settings.KLI[2]=BSI_Settings.KLI[3]=0;
  BSI_Settings.KHI[0]=BSI_Settings.KHI[1]=0;
  BSI_Settings.KHI[2]=BSI_Settings.KHI[3]=0;

  // ������������� ���������
  BSI_Settings.AT[0]=54932;
  BSI_Settings.AT[1]=54917;  
  BSI_Settings.BT[0]=-14; // ���� -14
  BSI_Settings.BT[1]=-27; // ���� -27
  BSI_Settings.KLT[0]=BSI_Settings.KLT[1]=0;
  BSI_Settings.KHT[0]=BSI_Settings.KHT[1]=0;
  
  BSI_Settings.typeT[0]=0;
  BSI_Settings.typeT[1]=0;

  // ��������� ���������
  BSI_Settings.ZTime[0].min=1000;     // Tmin
  BSI_Settings.ZTime[0].max=1000000;  // Tmax
  BSI_Settings.ZTime[1].min=8000;
  BSI_Settings.ZTime[1].max=8000000;
  BSI_Settings.ZTimeMeasure[0]=53255; //TPZ
  BSI_Settings.ZTimeMeasure[1]=53255;
  BSI_Settings.nZ_errors_max[0]=2; // nErr max
  BSI_Settings.nZ_errors_max[1]=2;
  BSI_Settings.FICP.bit.Filter1=0;
  BSI_Settings.FICP.bit.Front1=0;
  BSI_Settings.FICP.bit.Filter2=0;
  BSI_Settings.FICP.bit.Front2=0;

  BSI_Settings.BiUniPolar.byte=0xFF; // All Bipolar
  BSI_Settings.on_off.byte=0xFF; // �������� ��� ������

}
*/

//  BSI_ResultEx.BSI_Results.I[0]=1;
//  BSI_ResultEx.CodeI[0]=0;
//  BSI_ResultEx.BSI_Results.T[0]=0;
//  BSI_Settings.ZTimeMeasure[0]=0;
//  BSI_Settings.ZTime[0].min=0;
//  BSI_Settings.BiUniPolar.bit.BU0=0;
//  BSI_Settings.on_off.bit.Tint=0; // ���� CONFIG
//  BSI_Data.Byte=1;
//  BSI_Results.Status.Flag.LevelZ2=0;
//  BSI_Results.T[0]=0;
//  BSI_Results.Tin




//=============================================================================================
// ��������� ������ ������������ ������� �� EEPROM � SRAM.
// ���� �� �� ������� � �������� ����� �������, �� �������� ������ ��������� �����
// � ���� �� �������, �� ����������������� �������� ����� �������.
// ���� �� ��������� ����� ������� �� �������, �� �������� ������ �� ��������� ��
// ������ �������� � ����������������� �������� � ��������� ����� �������.
// �������� ������ � ������� ����� ������������ �������� ���������:
// 1. �������� �������� ����� ������������, ��������� ��.
// 2. ���� �� �� �������, �� ������ ������ ����� �������, ������� ��.
// 3. ���� �� �� �������, �� ������ ��������� ������ �� ������ ��������
// (���� � ���������� ��� ���������).
// ���� ��������� ����� �� ���� ���������, �� ��������� ����� ���� ��������� -
// ������ ������ ��� ������� ����� ��������� ��� ������ EEPROM. ��� �� �����
// ������ � EEPROM ��� ���� �� �������.
// � ��������� ��������� ����� ������������� ����������� ������� �� ���������
// � EEPROM � ����������� ������, ���� ����� ��������� (�� ������� ��).

void Read_Settings (void)
{
//  unsigned int CheckSum;

  Read_Config_From_EEPROM (EEPROM_MAIN_SETTINGS_ADDR); // ������ �������� ������
  // ����� ��������� ������������ � SRAM, ����� ��������� �� ����� ����� (��� ����� ��������� ���� ���� ��)
//  CheckSum = CRC16((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings)-2 );

  if (CRC16((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings) ) == 0) // ���� �� ������������ ����� � ��� ��������� �� �������, ��
    {
//      Write_Config_To_EEPROM (EEPROM_RESERVED_SETTINGS_ADDR); // ���������� ����� ������ �� ������ ������ �� ������ �����
      BSI_ResultEx.ArchiveStatus=0; // ������=0 - OK (�������� � �������� ������ �������)
      return;
    }

// ���� �������� ������ ���������, ��
  Read_Config_From_EEPROM (EEPROM_RESERVED_SETTINGS_ADDR); // ������ ������ ������
//  CheckSum = CRC16((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings)-2 ); // ������� �� ������������

  if (CRC16((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings) ) == 0)
    {
      Write_Config_To_EEPROM (EEPROM_MAIN_SETTINGS_ADDR); // ���������� ����� ������ � ������ �����
      BSI_ResultEx.ArchiveStatus=1; // ������=1 (�������� � ��������� ������ �������)
      return;
    }

  // ���� ��������� ������ ���������, �� � ��������� ������ �������� ������ �� ��������� �� ������ ��������
  Read_Config_From_PGM (PGM_SETTINGS_ADDR); // ������ ������ �� ������ ��������. �������� ��������� InitSettings()
  CRC16_And_Store((unsigned char *)(unsigned int)&BSI_Settings, (sizeof(BSI_Settings))-2 ); // ������� �� � ����������� � �����


  // ����� �������������� � �������� ��, �������� ������������ �������� � ��������� �������

  Write_Config_To_EEPROM (EEPROM_MAIN_SETTINGS_ADDR); // ���������� ������ � ������ �����  
  Write_Config_To_EEPROM (EEPROM_RESERVED_SETTINGS_ADDR); // ���������� ������ � ��������� �����

  BSI_ResultEx.ArchiveStatus=3; // ������=3 (��������� ����� ������� - �������� � ������ ������� �� ���������)

}



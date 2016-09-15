// #include "ds1621.h"

void Start_DS1621 (void)
{
  I2C_Start();
  if (Error_I2C==0)
    {
      I2C_Byte_Write(I2C_ADDR_DS1621);
      if (Error_I2C==0)
        {
          I2C_Byte_Write(DS_Command_Start);
        }
      I2C_Stop();
    }
}

// ===================== Init DS1621

void Init_DS1621 (void)
{
  I2C_Start();
  if (Error_I2C==0)
    {
      I2C_Byte_Write(I2C_ADDR_DS1621);
      if (Error_I2C==0)
        {
          I2C_Byte_Write(DS_Command_Init);
        }
      if (Error_I2C==0)
        {
          I2C_Byte_Write(0x00);
        }  
      I2C_Stop();
    }
}

// ======== ������ DS1621 ==============
// � ����� � ���������� ���������� ������ ����������� ���������� (���������/�������� �������),
// ������ ��������� �� ����� ������ � ���������

/*
void Pusk_DS1621 (void)
{
  Init_DS1621();
  if (Error_I2C==0)
    {
      Start_DS1621();
    }
}
*/

// ================= ������ �� DS1621 ================================
// ������ ������ ����������� �� DS1621 � �������� � ������ TERH & TERL

void Read_DS1621  (void)
{
  unsigned char tmp;
  I2C_Start();
  if (Error_I2C==0)
    {
      I2C_Byte_Write(I2C_ADDR_DS1621);
      if (Error_I2C==0)
        {
          I2C_Byte_Write(DS_Command_Read);
        }
      if (Error_I2C==0)
        {
          I2C_Start();
          if (Error_I2C==0)
            {
              I2C_Byte_Write(I2C_ADDR_DS1621+1); // ��� ������ ����� �� 1 ������
            }
          if (Error_I2C==0)
            {
              tmp=I2C_Byte_Read();
              TerIn_HI=tmp;
              I2C_ACK();

              tmp=I2C_Byte_Read();
              TerIn_LO=tmp;
              I2C_NACK();

              Error_I2C=0x00; // ����� ���������� ������
            }
        }
      I2C_Stop();
    }
}

// =====================================================================
// ���������� �����, ������� ����������� ����������� DS1621 � UART
/*
void Send_Result_DS1621 (void)
{
 signed int temperature;

 if (Error_I2C!=0)
  {
     char message[]="Error_Reading_DS1621_";
     sprintf (UART_Buf,message);
     Transmit(sizeof(message));
  }
 else
  {
    if (TerIn_LO==0)
       {
         temperature=TerIn_HI*100;
       }
    else
       {
         temperature=(TerIn_HI*100)+50;
       }
    sprintf (UART_Buf+1,"%d %d",temperature,TerIn_2dT);

    UART_Buf[0]=' ';
    Transmit(8); 
   }
}
*/

//====================================================================
// ������ �� ����������� ���������� � ��������� �� ������,
// ���������� ����� ���������� � ����� �����������.
// ��������� |2*dT| � ���� ����� �����������

void Read_Internal_T (void)
{
  Read_DS1621();
  if ( (Error_I2C) || (TerIn_LO&0x7F) ) // ���� ���� ������ I2C ��� ������ ����
    // ����������� ����� �� 0x80 ��� 0x00, �� ������
    {
      BSI_ResultEx.BSI_Results.Status.Flag.ReadyTintern=0; // ����� ���� ���������� ��������� DS1621
      return; // � ����� �����
    }
  
  unsigned char double_half;
  signed char deltaT;
  deltaT = (TerIn_HI - REPER_POINT); // ������ � ����������� �� ��������� �������� �����
  
  if (TerIn_LO==0x80)
    {
      double_half=1; // ���� ���� 0.5 ������� �� � ��������� ������ ��������� 1
    }
  else
    {
      double_half=0; // ���� �� ���� 0.5 ������� �� � ��������� ������ ��������� 0
    }

  if (deltaT<0)
    {
      BSI_ResultEx.BSI_Results.Status.Flag.signTintern=1; // ���� ���� ����� �� ������������� ����
    }
  else
    {
      BSI_ResultEx.BSI_Results.Status.Flag.signTintern=0;
    }

      // ������ �� ��� ������ � ���� ��������� ����������
  TerIn_2dT=abs((deltaT*2) + double_half); // ���������� ���������� � ������ ���������� "����������" � ������
  
  BSI_ResultEx.BSI_Results.Status.Flag.ReadyTintern=1; // ���� ���������� �����������
  BSI_ResultEx.BSI_Results.Tin=(TerIn_LO<<8|TerIn_HI); // ������������ � ������������ ���������
}



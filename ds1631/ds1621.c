// #include "ds1621.h"

void start_DS1621 (void)
{
  i2c_start();
  if (!error_i2c)
    {
      i2c_byte_write(I2C_ADDR_DS1621);
      if (!error_i2c)
        {
          i2c_byte_write(DS_Command_Start);
        }
      i2c_stop();
    }
}

// ===================== Init DS1621

void init_DS1621 (void)
{
  i2c_start();
  if (!error_i2c)
    {
      i2c_byte_write(I2C_ADDR_DS1621);
      if (!error_i2c)
        {
          i2c_byte_write(DS_Command_Init);
        }
      if (!error_i2c)
        {
          i2c_byte_write(0x00);
        }  
      i2c_stop();
    }
}

// ======== ������ DS1621 ==============
// � ����� � ���������� ���������� ������ ����������� ���������� (���������/�������� �������),
// ������ ��������� �� ����� ������ � ���������


void pusk_DS1621 (void)
{
  init_DS1621();
  if (!error_i2c)
    {
      start_DS1621();
    }
}


// ================= ������ �� DS1621 ================================
// ������ ������ ����������� �� DS1621 � �������� � ������ TERH & TERL

void read_DS1621  (void)
{
  unsigned char tmp;
  i2c_start();
  if (!error_i2c)
    {
      i2c_byte_write(I2C_ADDR_DS1621);
      if (!error_i2c)
        {
          i2c_byte_write(DS_Command_Read);
        }
      if (!error_i2c)
        {
          i2c_start();
          if (!error_i2c)
            {
              i2c_byte_write(I2C_ADDR_DS1621+1); // ��� ������ ����� �� 1 ������
            }
          if (!error_i2c)
            {
              tmp=i2c_byte_read();
              TerIn_HI=tmp;
              i2c_ack();

              tmp=i2c_byte_read();
              TerIn_LO=tmp;
              i2c_nack();

              error_i2c=0x00; // ����� ���������� ������
            }
        }
      i2c_stop();
    }
}

// =====================================================================
// ���������� �����, ������� ����������� ����������� DS1621

void send_result_DS1621 (void)
{
 float temperature;

 if (error_i2c)
  {
     sprintf (LIN1,"Error_I2C");
//     sed15b1_update_console();
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
    sprintf (LIN3,"T=%04.01f",temperature/100);
    sprintf (LIN4,"��� ������ �=%d",TerIn_2dT);
//    sed15b1_update_console();
   }
}


//====================================================================
// ������ �� ����������� ���������� � ��������� �� ������,
// ���������� ����� ���������� � ����� �����������.
// ��������� |2*dT| � ���� ����� �����������

void read_internal_T (void)
{
  read_DS1621();
//  if ( (Error_I2C) || (TerIn_LO&0x7F) ) // ���� ���� ������ I2C ��� ������ ����
    // ����������� ����� �� 0x80 ��� 0x00, �� ������
//    {
//      BSI_ResultEx.BSI_Results.Status.Flag.ReadyTintern=0; // ����� ���� ���������� ��������� DS1621
//      return; // � ����� �����
//    }
  
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
      signTintern=1; // ���� ���� ����� �� ������������� ����
    }
  else
    {
      signTintern=0;
    }

      // ������ �� ��� ������ � ���� ��������� ����������
  TerIn_2dT=abs((deltaT*2) + double_half); // ���������� ���������� � ������ ���������� "����������" � ������
  
//  Tin=(TerIn_LO<<8|TerIn_HI); // ������������ � ������������ ���������
}



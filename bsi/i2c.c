#include "i2c.h"

void I2C_ACK  (void)
{
  I2C_SDA_Out();  // SDA for OUT (SDA=0) �������� �����
  delay_us(4);
  I2C_SCL_In();   // SCL=1
  delay_us(4);
  I2C_SCL_Out();  // SCL=0
  delay_us(4);
}

void I2C_NACK  (void)
{
  I2C_SDA_In();   // SDA=1 �������� ��������
  delay_us(4);
  I2C_SCL_In();   // SCL=1
  delay_us(4);
  I2C_SCL_Out();  // SCL=0
  delay_us(4);
}

// I2C Start =====================================
// ���������� ���� ������: 0 - ��, 0xFF - ������

void I2C_Start  (void)
{
  Error_I2C=0;
  I2C_SDA_In();     // SDA for In (SDA=1)
  delay_us(1);
  I2C_SCL_In();     // SCL for In (SCL=1)
  delay_us(1);
  if (I2C_PIN&(1<<SDA_PIN) && I2C_PIN&(1<<SCL_PIN)) // ���� ��� ���� � �������, �� ��
    {
      I2C_SDA_Out();
      delay_us(4);
      I2C_SCL_Out();
      delay_us(4);  
    }
  else
    {
      Error_I2C=0xFF;
    }
}

//=======================================
// I2C Stop

void I2C_Stop (void)
{
  I2C_SDA_Out();
  delay_us(1);
  I2C_SCL_In();
  delay_us(1);
  I2C_SDA_In();
  delay_us(4);  
}

//=========================================
// i2c Byte Write

void I2C_Byte_Write (unsigned char byte)
{
//  __watchdog_reset();
  unsigned char counter;
  for (counter=0; counter<8; counter++)
    {
      if (byte&0x80)
        {
          I2C_SDA_In(); // SDA=1
        }
      else
        {
          I2C_SDA_Out(); // ����� SDA=0
        }
      delay_us(4);
  
      I2C_SCL_In();  // SCL=1 ������ ���� ��������
      delay_us(4);
  
      I2C_SCL_Out(); // SCL=0
      delay_us(4);
  
      byte<<=1;  // ������� ������
    }

  I2C_SDA_In(); // SDA �� ���� (SDA=1)
  delay_us(4);

  I2C_SCL_In(); // SCL=1
  delay_us(4);

  Error_I2C=0xFF;
  counter=0xFF; // ������� ��� ����� ������ ���������� ������
  do
  {
    if (!(I2C_PIN&(1<<SDA_PIN))) // ���� ACK �� �������
                                 // ���� �����=0 �� ��
    {
      Error_I2C=0; break;
    }
  }
  while (counter--);

  I2C_SCL_Out();    // SCL=0
}

// ============================================
// i2c Byte Read

unsigned char I2C_Byte_Read (void)
{
// __watchdog_reset();
 unsigned char counter;
 unsigned char result = 0;
 unsigned char mask = 0x80;
 I2C_SDA_In(); // ����� ������ �� ���� (SDA=1)
 
 for (counter=0; counter<8; counter++)
   {
      delay_us(4);
  
      I2C_SCL_In();   // SCL for In (SCL=1)
      delay_us(4);
  
      if (I2C_PIN&(1<<SDA_PIN)) // ����� �������� ���� SDA
        {
          result |= mask; // ���� ���� ������� �� ������ OR ���������� � �����
        }
      mask>>=1;   // ������� �����
      I2C_SCL_Out();  // ����� SCL � �������� ��������� ���� (SCL=0)
   }

 return result;
}


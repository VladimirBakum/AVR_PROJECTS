// I2C Special For VAmeter

void i2c_init (void)
{
  SCL_PORT=0; // Set pins to gnd
  SDA_PORT=0;
  SCL_DDR=0; // Set pullup
  SDA_DDR=0;
}

void i2c_ack  (void)
{
  I2C_SDA_0;  // SDA for OUT (SDA=0) �������� �����
  delay_us(8);
  I2C_SCL_1;   // SCL=1
  delay_us(8);
  I2C_SCL_0;  // SCL=0
  delay_us(8);
}

void i2c_nack  (void)
{
  I2C_SDA_1;   // SDA=1 �������� ��������
  delay_us(8);
  I2C_SCL_1;   // SCL=1
  delay_us(8);
  I2C_SCL_0;  // SCL=0
  delay_us(8);
}

// I2C Start =====================================
// ���������� ���� ������: 0 - ��, 0xFF - ������

void i2c_start  (void)
{
  error_i2c=0;
  I2C_SDA_1;     // SDA for In (SDA=1)
  delay_us(2);
  I2C_SCL_1;     // SCL for In (SCL=1)
  delay_us(2);
  if (SDA_PIN && SCL_PIN) // ���� ��� ���� � �������, �� ��
    {
      I2C_SDA_0;
      delay_us(8);
      I2C_SCL_0;
      delay_us(8);  
    }
  else
    {
      error_i2c=0xFF;
    }
}

//=======================================
// I2C Stop

void i2c_stop (void)
{
  I2C_SDA_0;
  delay_us(2);
  I2C_SCL_1;
  delay_us(2);
  I2C_SDA_1;
  delay_us(8);  
}

//=========================================
// i2c Byte Write

void i2c_byte_write (unsigned char byte)
{

  unsigned char counter;
  for (counter=0; counter<8; counter++)
    {
      if (byte&0x80)
        {
          I2C_SDA_1; // SDA=1
        }
      else
        {
          I2C_SDA_0; // ����� SDA=0
        }
      delay_us(8);
  
      I2C_SCL_1;  // SCL=1 ������ ���� ��������
      delay_us(8);
  
      I2C_SCL_0; // SCL=0
      delay_us(8);
  
      byte<<=1;  // ������� ������
    }

  I2C_SDA_1; // SDA �� ���� (SDA=1)
  delay_us(8);

  I2C_SCL_1; // SCL=1
  delay_us(8);

  error_i2c=0xFF;
  counter=0xFF; // ������� ��� ����� ������ ���������� ������
  do
  {
    if (!SDA_PIN) // ���� ACK �� �������
                                 // ���� �����=0 �� ��
    {
      error_i2c=0; break;
    }
  }
  while (counter--);

  I2C_SCL_0;    // SCL=0
}

// ============================================
// i2c Byte Read

unsigned char i2c_byte_read (void)
{

 unsigned char counter;
 unsigned char result = 0;
 unsigned char mask = 0x80;
 I2C_SDA_1; // SDA=1
 
 for (counter=0; counter<8; counter++)
   {
      delay_us(8);
  
      I2C_SCL_1;   // SCL for In (SCL=1)
      delay_us(8);
  
      if (SDA_PIN) // ����� �������� ���� SDA
        {
          result |= mask; // ���� ���� ������� �� ������ OR ���������� � �����
        }
      mask>>=1;   // ������� �����
      I2C_SCL_0;  // ����� SCL � �������� ��������� ���� (SCL=0)
   }

 return result;
}


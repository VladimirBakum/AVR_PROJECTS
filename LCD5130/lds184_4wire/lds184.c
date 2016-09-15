//=============================================================
// ������ ����� � ���
// ������� MSB (������ ���� ������� ���)

void lds184_write (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) // ������������� ����� ������� ��� � �����
      {
        SDATA_1; // � ������������ SI � ������������
      }
    else  
      {
        SDATA_0;
      }
    SCLK_0; // ������ �� ����� SCLK
    SCLK_1; // ���������� SCLK ������� � �������� ���������
    ch<<=1; // �������� ���� �����
  }
}

//===========================================
// �������� ������� � ���

void lds184_cmd(unsigned char cmd)
{
  A0_0; // ������� ������� A0=0
  lds184_write(cmd);
}

//=============================================
// �������� ������ � ���

void lds184_data(unsigned char dat)
{
  A0_1; // ������� ������ A0=1
  lds184_write(dat);
}


void display_graphic(unsigned char *graphic)
{
   unsigned int i;
   lds184_cmd(LDS184_COLUMN_ADDRESS_SET); // 0x2A Col add
   lds184_data(0x00);
   lds184_data(0x62);

   lds184_cmd(LDS184_MEMORY_WRITE); // 0x2C

    for(i=0;i<13860;i++)
      {
           lds184_data(*graphic++);
      }
}

void test_patten(unsigned char cl,unsigned char ch)
{
    unsigned int i,j;
   lds184_cmd(LDS184_COLUMN_ADDRESS_SET); // 0x2A Col add
   lds184_data(0x00);
   lds184_data(0x61);

   // address_set(0x10,0x00,0x72,0x60);
   lds184_cmd(LDS184_MEMORY_WRITE); // 0x2C

    for(i=0;i<LDS184_PIXELY;i++)
     {
       for(j=0;j<LDS184_PIXELX;j++)
         {
          lds184_data(cl);
          lds184_data(ch);
         }
     }
}



//=============================================================
// ������������� LCD

void lds184_init (void)
{
  PIN_A0=0; // ������������� ���� ������������ ��� � ����
  PIN_SCLK=0;
  PIN_SDATA=0;
  PIN_RESET=0;
  SCLK_1;
  CS_0; // CS ������ ������

  RESET_0;  // Reset LCD
  delay_us(500);
  RESET_1;  // Return Reset OFF
  delay_ms(20);
  
  lds184_cmd(LDS184_SOFTWARE_RESET);  // 0x01 Soft Reset
  delay_ms(20);  
  
  lds184_cmd (LDS184_WRITE_CONTRAST);	// 0x25
  lds184_data(0x39);

  lds184_cmd(LDS184_SLEEP_OUT);	// 0x11
  lds184_cmd(LDS184_DISPLAY_NORMAL_MODE); // 0x13

  lds184_cmd(LDS184_INVERSION_OFF);	// 0x20

  lds184_cmd(LDS184_IDLE_MODE_OFF);	// 0x38

  lds184_cmd (LDS184_MEMORY_ACCESS_CTRL);	// 0x36
  lds184_data(0x80);


  lds184_cmd(LDS184_DISPLAY_ON); // 0x29

//  lds184_cls(); // ������ �����-��� � ������������� ����� ������ � ������  
}

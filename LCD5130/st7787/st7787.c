// ST7787 Routine
// ********************************************************************
// Writen by (R)soft 13.5.2013
// Version 1.0
// Tested on Mega2561
// Testing on JCTTQ2437PS01-T01 LCD Module. Resolution 320x240 pixels. 
// Built-in ST7787 controller with parallel 16-bit interface.
// Reference manual for controller - ST7787.pdf
// ********************************************************************


//=============================================================
// ������ ������� � ���

void st7787_write_command (unsigned char byte)
{
  DCX_0; // ����� �������
  CSX_0; // �������� ��������

  DDRD_LO=~(byte); // ����������� � ���������� ���� �� ����

  WRX_0; // WRX=0 - Write mode
  delay_us(5); // �������� �����
  WRX_1; // ���� ���������� � ����������
  CSX_1; // ��������� ��������
}


//=============================================================
// ������ ����� ������ � ���

void st7787_write_data (unsigned int word)
{
  DCX_1; // ����� ������ ������
  CSX_0; // �������� ��������
  
  // ����������� �� ���� ������ 16-������� �����
  DDRD_HI=~((unsigned char)((word>>8)&0xFF)); // �������� ������� ���� � �����������  
  DDRD_LO=~((unsigned char)((word)&0xFF)); // �������� ������� ���� � �����������

  WRX_0; // WRX=0 - Write mode
  delay_us(5); // ��������
  WRX_1; // ���� ���������� � ����������
  CSX_1; // ��������� ��������
}


//=============================================================
// ������ ����� �� ���
// DCX ��� ������ ������ 1
// ��� ������� �������� ������������� ��� ������� ���� �
// ����������� ������� ST7787 - ������ ���������.

unsigned int st7787_read (void)
{
  DCX_1;
  CSX_0; // �������� ��������
  DDRD_HI=0; // ����� ������� ���� ������ ��������� � Z State
  DDRD_LO=0;
  RDX_0;  // RDX=0 - Read mode  ����� ������ - ������ �����������
  
  delay_us(5); // �������� �����������! ����� �� �������� ��������������� ������ �� ����
  
  // ������ � ���� ������ ������� ����
  unsigned char lo=PIN_LO;
  unsigned char hi=PIN_HI; // ������� ����
  
  RDX_1;  // ������� RDX � �������� ���������
  CSX_1; // ��������� ��������
  return ((unsigned int)(hi<<8|lo));
}


void st7787_reset_xy()   
{    
  st7787_write_command(ST7787_CASET);   /* column address set */   
  st7787_write_data(0x0000);       /*XS15-XS8*/   
  st7787_write_data(0x0000);   /*XS7-XS0    (X start address)*/   
  st7787_write_data(0x0000);  /* XE15-XE8*/   
  st7787_write_data(0x00EF);   /*XE7-XE0    (X end address)*/   
          
  st7787_write_command(ST7787_RASET);   /*row address set (RASET)*/   
  st7787_write_data(0x0000);      /*YS15-YS8*/   
  st7787_write_data(0x0000);    /*YS7-YS0    (Y start address)*/   
  st7787_write_data(0x0001);    /*YE15-YE8*/   
  st7787_write_data(0x003F);     
}   

//=============================================================
// ������������� LCD � ������� ����� ������/����������

void st7787_init ()   
{        
  PIN_RESX=0; // ������������� ���� ������������ ��� � ����
  PIN_CSX=0;
  PIN_RDX=0;
  PIN_WRX=0;
  PD_LO=0;
  PD_HI=0;
  
  CSX_1; // ����� ��������� ������� - �������� ��������
  RDX_1;  // �������� ��������� Read=1
  WRX_1;  // �������� ��������� Write=1
  DCX_0;
  
  RESX_0;  // Reset LCD
  delay_ms(10);
  RESX_1;  // Return Reset OFF
  delay_ms(50);

//  st7787_write_command(ST7787_SWRESET); // Soft Reset
//  delay_ms(50);

/*
  st7787_write_command(ST7787_SLPOUT);
  
  st7787_write_command(ST7787_PWCTR1);   //power control
  st7787_write_data(0x0000);   
           
  st7787_write_command(ST7787_PWCTR2);      
  st7787_write_data(0x00D0);  //d9   
  st7787_write_data(0x0001);   

  st7787_write_command(ST7787_PWCTR3);      
  st7787_write_data(0x0001);   
  st7787_write_data(0x00D4);   
  st7787_write_data(0x0085);   
  st7787_write_data(0x0000);   
  st7787_write_data(0x0000);   
   
  st7787_write_command(ST7787_PWCTR4);      
  st7787_write_data(0x0001);   
  st7787_write_data(0x0022);   
  st7787_write_data(0x0000);   
  st7787_write_data(0x0000);   
  st7787_write_data(0x0001);   
        
  st7787_write_command(0xF4); //?      
  st7787_write_data(0x00FF);   
  st7787_write_data(0x003F);   
          
  st7787_write_command(0xF5); //?  
  st7787_write_data(0x0010);   
          
  st7787_write_command(ST7787_VCOM_MMODE);      
  st7787_write_data(0x007F);   
           
  st7787_write_command(ST7787_VMCTR1);      
  st7787_write_data(0x00B6); //be   
  st7787_write_data(0x001A);   
             
  st7787_write_command(ST7787_VMCTR2);      
  st7787_write_data(0x0028);    
  st7787_write_data(0x0000);     
          
  st7787_write_command(ST7787_IDMOFF);    //IDLE mode off
   
  st7787_write_command(ST7787_NORON);    //Normal display mode on   
       
  st7787_write_command(ST7787_INVOFF);    // display inversion off
  st7787_write_command(ST7787_FRMCTR1);    // Full Colors
  st7787_write_data(0x0030);  //36   
  st7787_write_data(0x0002);   
  st7787_write_data(0x0002);   
  st7787_write_command(ST7787_VSYNCOUT);  // External VSYNC Disable
   
  st7787_write_command(ST7787_CASET);   // column address set
  st7787_write_data(0x0000);       // XS15-XS8
  st7787_write_data(0x0000);   // XS7-XS0    (X start address)
  st7787_write_data(0x0000);  // XE15-XE8
  st7787_write_data(0x00EF);   // XE7-XE0    (X end address)
          
  st7787_write_command(ST7787_RASET);   // row address set (RASET)
  st7787_write_data(0x0000);      // YS15-YS8
  st7787_write_data(0x0000);    // YS7-YS0    (Y start address)
  st7787_write_data(0x0001);    // YE15-YE8
  st7787_write_data(0x003F);   // YE7-YE0    (Y end address)
   
  st7787_write_command(ST7787_TEON);    // Tearinf Effect On  
  st7787_write_data(0x0000);   
  st7787_write_command(ST7787_COLMOD);  // color mode Interface pixel format  (COLMOD
  st7787_write_data(0x0055);   
  st7787_write_command(ST7787_MADCTL);  // memory data access control  (MADCTR)
  st7787_write_data(0x0080);     // MY=0; MX=0; MV=0; ML=0; RGB=0  mh=0
  st7787_write_command(ST7787_DISPON);   

*/

}   




//----------------------------------------------
void colours ()         
{  
   unsigned char a,b;  
   st7787_reset_xy();   
   st7787_write_command(ST7787_RAMWR);  
  
 for(a=0;a<160;a++)  
   {   
      for(b=0;b<240;b++)  
      {  
        st7787_write_data(0xFFFF);  
        st7787_write_data(0x0000);  
       }  
    }  
}

// ���������� ����� ������
void st7787_fill (unsigned int word)
{    
   
 unsigned int a,b;   
// st7787_reset_xy();    
// st7787_write_command(ST7787_RAMWR);  
  st7787_write_command(0x22);    
 for(a=0;a<320;a++)   
   {    
      for(b=0;b<240;b++)   
         {   
           st7787_write_data(word);   
         }   
   }   
}   


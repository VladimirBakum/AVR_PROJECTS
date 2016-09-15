#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf
#include <string.h> // for memset
#include <stdlib.h> // for abs

#define delay_us(V) __delay_cycles(V*14L)

#define KEY1  PINE_Bit4
#define KEY2  PINE_Bit5
#define KEY3  PINE_Bit6
#define KEY4  PINE_Bit7

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

#include "font\font5x7.h"
#include "sed15b1_3WIRE\sed15b1.h"
#include "sed15b1_3WIRE\sed15b1.c"

#include "ks0717\ks0717.h"
#include "ks0717\ks0717.c"

#include "..\ds1631\i2c.h"
#include "..\ds1631\i2c.c"
#include "..\ds1631\ds1621.h"
#include "..\ds1631\ds1621.c"

#include "st7558\st7558.h"
#include "st7558\st7558.c"

//����������� 1Wire ����������
#include "..\OWI\OWISWBitFunctions.c"
#include "..\OWI\OWIHighLevelFunctions.c"

//����� ������, � �������� ��������� ������
#define BUS   OWI_PIN_7

//������� ������� DS18B20
#define DS18B20_CONVERT_T                0x44
#define DS18B20_READ_SCRATCHPAD          0xbe

unsigned char scratchpad[9];

float a, b, c;

void main( void )
{
  unsigned int contrast=0x23;
  unsigned int tmp = 0;
  unsigned char piktogramm=0x00;
  unsigned char temperature;
  unsigned char counter=0;
  
  a=54.321;
  b=3.14;
  c=a/b;

  i2c_init();
  pusk_DS1621();
  
  ks0717_init();
  ks0717_cls();
  ks0717_clear_console(0x20); // ������ ����� ������� ���������

  ks0717_piktogramm(2,0xFF);
  ks0717_piktogramm(6,0xFF);
  ks0717_piktogramm(10,0xFF);
  ks0717_piktogramm(14,0xFF);  
  ks0717_piktogramm(18,0xFF);
  ks0717_piktogramm(22,0xFF);
  ks0717_piktogramm(30,0xFF);
  ks0717_piktogramm(66,0xFF);
  ks0717_piktogramm(70,0xFF);
  ks0717_piktogramm(72,0xFF);

  
  ks0717_piktogramm(74,0xFF); // ����� ����������� �������
  ks0717_piktogramm(78,0xFF);
  ks0717_piktogramm(82,0xFF);
  ks0717_piktogramm(86,0xFF);
  ks0717_piktogramm(42,0xFF); // �����������
  ks0717_piktogramm(54,0xFF); // �����

  //===================================
  st7558_init();
  sprintf(LN1,"������� ST7558");
  sprintf(LN2,"LCD 96X65 pixels");
  sprintf(LN3,"For MotorolaC115");
  sprintf(LN4,"I2C mode");
  sprintf(LN5,"���� %g/%g=%g",a,b,c);
  sprintf(LN7,"Research & Code");
  sprintf(LN8,"By (R)soft 2013");
  st7558_update_console();
  //====================================
  
  sed15b1_init();
  sed15b1_clear_console(0x20);
  OWI_Init(BUS);
  
  sprintf(LIN1,"�����������");
  sprintf(LIN2,"DS1631");
  sprintf(LIN8,"By (R)soft 2013");
  
  sed15b1_update_console();
  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input
  
  while (1)
  {

    //-----------------------------
    read_internal_T();
    send_result_DS1621(); // ����� ����������� �� �������
    
    //--------------------------------------------------------
    OWI_DetectPresence(BUS);     // ������� ������ ����
    OWI_SkipRom(BUS);                     // ��������� ���� ��������� �� ����
    OWI_SendByte(DS18B20_CONVERT_T ,BUS); // ����c� ��������������

    while (!OWI_ReadBit(BUS)); // �������� ����� ��������������

    OWI_DetectPresence(BUS);      // ����� ����
    OWI_SkipRom(BUS);             //    OWI_ReadRom(scratchpad,BUS);
    OWI_SendByte(DS18B20_READ_SCRATCHPAD, BUS); // ������ ������ ������� � ������
    
    scratchpad[0] = OWI_ReceiveByte(BUS); // ������ ����������� ��������
    scratchpad[1] = OWI_ReceiveByte(BUS);
    scratchpad[2] = OWI_ReceiveByte(BUS); // TH
    scratchpad[3] = OWI_ReceiveByte(BUS); // TL
    scratchpad[4] = OWI_ReceiveByte(BUS);
    scratchpad[5] = OWI_ReceiveByte(BUS);
    scratchpad[6] = OWI_ReceiveByte(BUS);
    scratchpad[7] = OWI_ReceiveByte(BUS);
    scratchpad[8] = OWI_ReceiveByte(BUS);

//    sprintf(LINEN1,"DALLAS DS18B20");
//    sprintf(LINEN2,"Debug:%02X",scratchpad[0]);
//    sprintf(LINEN2+8,"%02X",scratchpad[1]);
//    sprintf(LINEN2+10,"%02X",scratchpad[2]);
//    sprintf(LINEN2+12,"%02X",scratchpad[3]);
//    sprintf(LINEN2+14,"%02X",scratchpad[4]);    
//    sprintf(LINEN3+6,"%02X",scratchpad[5]);
//    sprintf(LINEN3+8,"%02X",scratchpad[6]);
//    sprintf(LINEN3+10,"%02X",scratchpad[7]);
//    sprintf(LINEN3+12,"%02X",scratchpad[8]);
//    sprintf(LINEN4,"�����������");
//    sprintf(LINEN6,"MOTOROLA V50 LCD");
//    sprintf(LINEN7,"-=-=-=-=-=-=-=-");    
    // ������� ���� � ����������� �����, ���� ��� �������������
    if ((scratchpad[1]&128) == 0)
          {
           sprintf(LINEN4+11,"+");
          }
    else
          {
            sprintf(LINEN4+11,"-");
            tmp = ((unsigned int)scratchpad[1]<<8)|scratchpad[0];
            tmp = ~tmp + 1;
            scratchpad[0] = tmp;
            scratchpad[1] = tmp>>8;  
          }
      
    //������� �������� ����� ����. �����������
    temperature = (scratchpad[0]>>4)|((scratchpad[1]&7)<<4);
    sprintf(LINEN4+12,"%02d",temperature);
      
    //������� ������� ����� ����. �����������
    temperature = (scratchpad[0]&15);
    temperature = (temperature<<1) + (temperature<<3);
    temperature = (temperature>>4);
    sprintf(LINEN4+14,".");
    sprintf(LINEN4+15,"%d",temperature);
    sprintf(LINEN5,"Counter:%X",counter);
    counter++;

    sed15b1_update_console(); // �������� ��� ������ 
    ks0717_update_console(); 
    
    if (!KEY1)
      {
        contrast--;
        contrast&=0x3F;

        sprintf(LINEN6,"Contrast:%02X",contrast);
        ks0717_update_console();
        ks0717_write(0,KS0717_REF_VOLT_MODE); // Two Bytes Instruction
        ks0717_write(0,contrast); // 00...3F

        delay_ms(50);
      }

    if (!KEY2)
      {
        contrast++;
        contrast&=0x3F;
          
        sprintf(LINEN6,"Contrast:%02X",contrast);
        ks0717_update_console();
        ks0717_write(0,KS0717_REF_VOLT_MODE); // Two Bytes Instruction
        ks0717_write(0,contrast); // 00...3F

        delay_ms(50);
      }

    if (!KEY3)
      {
        piktogramm++;
        piktogramm&=0x7F;
        ks0717_piktogramm(piktogramm,0xFF);
        delay_ms(1000);
        ks0717_piktogramm(piktogramm,0x00);
        sprintf(LINEN6,"Piktogramm:%d",piktogramm);
        ks0717_update_console();
      }



  }
}

#include <iom162.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf

#define delay_us(V) __delay_cycles(V*8L)

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

#include "font5x7.h"
#include "hd66753.h"
#include "hd66753.c"


float a, b, c;

void main( void )
{
// ============ ������ � ������ ���
  
  hd66753_init(); // init �������� � ���� ������� ������� ������ ������

//  unsigned int wrd = 0xFFF3;
//  DDRDATA=~((unsigned char)((wrd)&0xFF)); // �������� ������� ���� � �����������


  a=12.345;
  b=8.16;
  c=a/b;
  hd66753_clear_console(0x20); // ������ ����� ������� ���������

  sprintf(LINEN1,"������� hd66752");
  sprintf(LINEN2,"LCD 150x100 pixs");
  sprintf(LINEN3,"For Alcatel");
  sprintf(LINEN4,"715 LCD.");
  sprintf(LINEN5,"���� %g/%g=%g",a,b,c);
  sprintf(LINEN7,"The Best LCD!");
  sprintf(LINEN8,"By (R)soft 2011");
  
//  hd66753_update_console(); // ����� �� ������ ������� �� �����



  while (1)
  {
    
  }
}

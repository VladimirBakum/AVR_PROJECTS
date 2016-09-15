/* 
��������� ��� ����������� MEGA128 � ����������� ���� by (R)soft 27.5.20011
Version 1.1
�������� ���������� ������ ������:
  => 8-������� ������ ����� 
  => ������� ������������� 8 ��� 
  => ������� ����� �� 4 ��� 
  => ������������ ����� ������ 8 �����

������� ���� ������� � ������ �������������. ������ ������ � ����� 1. ���� ����� ��������,
�� ��� ���������� ������������ � �������� ������. ������ DataFlash ����� ����� 1 � ����� 2 �
������� ������.

MCU - ATmega128 (������������ ��� �����������)
������: 1 - PLAY
        2 - ERASE
        3 - RECORD
DataFlash - AT45DB321 (32 Mbit, 8192 ������� �� 528 ����)
FOSC - 8 MHz (���������), �������� 14.318 ���
PORTB - SPI DataFlash Interface (3 Lines)+ Control DataFlash Interface (2 Lines)
PORTB:
  PB0 - nc
  PB1 - SCK
  PB2 - MOSI
  PB3 - MISO (input pin)
  PB4 - Ready_Busy (input pin)
  PB5 - Chip Select
  PB6 - PWM Output (����� �����)
  PB7 - nc
PORTF - ADC Input (1 Line)
  PF0 - ADC input
PORTE - Keys for Select Modes (3 Keys)
  PE4 - PLAY Button
  PE5 - ERASE Button
  PE6 - RECORD Button
PORTD - Indication
  PD6 - LED Status
Samlerate of ADC ~ 15686 Hz
*/

#include "iom128.h"
#include <ina90.h>
#include "stdlib.h"
#include "dataflash.h"

// ���������
void setup (void);
void erasing (void);
void recording (void);
void write_to_flash (unsigned char ad_data);
void playback (void);
void next_page_to_next_buffer (unsigned char active_buffer, unsigned int page_counter);
void active_buffer_to_speaker (unsigned char active_buffer);

// ���������� ����������
volatile unsigned char wait = 0;

#define Button_Play   0x10 // PE 4
#define Button_Erase  0x20 // PE 5
#define Button_Record 0x40 // PE 6
#define Data_LED      0x40 // PD 6

void setup(void)
{
  DDRB = 0xE7;	// 1110 0111 ������������� ����� SPI
                // nc,  PWM, CS,  RDY, MISO, MOSI, SCK, nc
                // PB7, PB6, PB5, PB4, PB3, PB2 , PB1, PB0
                //  O    O    O    I    I    O     O    O
                //  1    1    1    0    0    1     1    1
  PORTB = 0xFF;	// ��� ������ � ������� ���������, �� ������ ����������� ���������
  DDRD =  0xFF; // ���� ����������� �� �����
  PORTD = 0x00; // �������� ����������
  DDRE =  0x00; // Port E ������������ ��� ���� (For Buttons)
  PORTE = 0xFF; // �������� ������ ��������
  DDRF =  0x00; // Port F ������������ ��� ���� (For ADC)
  PORTF = 0x00; // ��� ���������� ��������

  __enable_interrupt();
}

void erasing(void)
{
  unsigned int block_counter = 0;
  unsigned char temp = 0x80;
  ACSR |= 0x02; 	// ��������� �����, �������������, ��� ���������  
                        //   ������ ������ ���� ������ ������

  // ���������� ���������, ���� SPI �������, 
  //  �������� �����, ������ MSB, 3 ����� SPI, Fcl/4
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA); // ��������� SPI

  while (block_counter < 512)
  {
    PORTB &= ~DF_CHIP_SELECT; // ��������� DataFlash

    SPDR = BLOCK_ERASE;
    while (!(SPSR & temp));   // �������� ���������� ��������

    SPDR = (char)(block_counter>>3);
    while (!(SPSR & temp));   // �������� ���������� ��������

    SPDR = (char)(block_counter<<5);
    while (!(SPSR & temp));   // �������� ���������� ��������

    SPDR = 0x00; 	      // �� �����
    while (!(SPSR & temp));   // �������� ���������� ��������

    PORTB |= DF_CHIP_SELECT;  // ���������� DataFlash

    block_counter++;
    while(!(PINB & DF_RDY_BUSY)); 	// �������� ������� �����
  }

  SPCR = 0x00; 			//���������� SPI
}


void recording(void)
{
  // ���������� ���������, ���� SPI �������, �������� �����, ������ MSB, 3 ����� SPI, Fcl/4
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA);  // 0x5C=92 0101 1100
                // SPI Control Register SPCR (SPIE=0, SPE=1, DORD=0, MSTR=1, CPOL=1, CPHA=1, SPR1=0, SPR0=0)
                // SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
  ADMUX = (1<<REFS1)|(1<<REFS0)|(1<<ADLAR); // ���� ADLAR=1 �� ����� ��������� �� ADCH, ���� ADLAR=0 �� �� ADCL
                // ����� �������� ������ ��� = 0, Internal VREF=2.56 V (REFS0=1 & REFS1=1)
                // (REFS1=1, REFS0=1, ADLAR=?, MUX4=0, MUX3=0, MUX2=0, MUX1=0, MUX0=0)
  ADCSR = (1<<ADEN)|(1<<ADSC)|(1<<ADIF)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS0); // 0xDD=221 1101 1101
                // ��������� �� ��������������, fCK/32, ����� ��������������
                // ADC Control and Status Register ADCSR
                // (ADEN=1, ADSC=1, ADFR=0, ADIF=1, ADIE=1, ADPS2=1, ADPS1=0, ADPS0=1)
                // ADEN=1 (Enable ADC), ADSC=1 (Single Conv), ADFR=0 (No Free Run), ADIF=1 (Interrupt Flag)
                // ADIE=1 (Interrupt Enable), ADPS2=1, ADPS1=0, ADPS0=1 (Division Factor = 32)
  while (!(PINE & Button_Record)); // ���� ������������ ���� ������ ������ ������ (������ 3)

  ADCSR = 0x00; // ���������� ���
  SPCR = 0x00;  // ���������� SPI
}

#pragma vector=ADC_vect
__interrupt void sample_ready(void)
{
  unsigned char count = 0;

  while (count < 6) count++; // �������� � ������� ���������� ������
  ADCSR|=(1<<ADSC);            // 0x40 ����� ������ �� ��������������
  write_to_flash(ADCH); // (ADLAR=1) ������ ������, ���������� �� ����
}


void write_to_flash(unsigned char flash_data)
{
  static unsigned int buffer_counter;
  static unsigned int page_counter;
  unsigned char temp = 0x80;

  if((ACSR & 0x02)) // ���� ���� ����������, �� ����� ������ ������ ���� �����������
    {
      buffer_counter = 0;
      page_counter = 0;   // ����� �������� ���� ������ ���� �������� ����� ������ 
      ACSR &= 0xFD; 	// ������� ����� �������
     }

  while(!(PINB & DF_RDY_BUSY)); 	// �������� ��������� �����

  PORTB &= ~DF_CHIP_SELECT; 	// ��������� DataFlash

  SPDR = BUFFER_1_WRITE;
  while (!(SPSR & temp)); 	// �������� ���������� ��������
  SPDR = 0x00; 			// �� �����
  while (!(SPSR & temp)); 	// �������� ���������� ��������

  SPDR = (char)(buffer_counter>>8); // �� ����� + ������ ��� ���� ������ ������
  while (!(SPSR & temp)); 		 // �������� ���������� ��������

  SPDR = (char)buffer_counter; 	 // ����� ������ (����. 2^8 = 256 �������)
  while (!(SPSR & temp)); 		 // �������� ���������� ��������
  SPDR = flash_data; 		// ������ ������ � ������� ������ SPI
  while (!(SPSR & temp)); 	// �������� ���������� ��������

  PORTB |= DF_CHIP_SELECT; 	// ���������� DataFlash

  buffer_counter++;

  if (buffer_counter > bytes_per_page) 	// ���� ����� ��������, �� ��� 
                                //���������� ������������ � �������� ������ 
    {
      buffer_counter = 0;
      if (page_counter < dataflash_pages) // ���� ������ �� ���������
      {
        PORTB &= ~DF_CHIP_SELECT; 	// �������� DataFlash

        SPDR = B1_TO_MM_PAGE_PROG_WITHOUT_ERASE; // �������� ������ �� ������ 1 � ��������
        while (!(SPSR & temp)); 	// �������� ���������� ��������
        SPDR = (char)(page_counter>>6);
        while (!(SPSR & temp)); 	// �������� ���������� ��������
        SPDR = (char)(page_counter<<2);
        while (!(SPSR & temp)); 	// �������� ���������� ��������
        SPDR = 0x00; 			// �� �����
        while (!(SPSR & temp)); 	// �������� ���������� ��������

        PORTB |= DF_CHIP_SELECT; 	// ���������� DataFlash

        page_counter++;
      }
      else
      {
        PORTD &= ~Data_LED; 	 // �������� LED 
        while (!(PINE & Button_Record)); // ����� ���� ������ ������ �� �������� (������ 3)
      }
    }
}


void playback(void)
{
  unsigned int page_counter = 0;
  unsigned char active_buffer = 1; // �������� ����� = ����� 1

  TCCR1A = (1<<COM1B1)|(1<<WGM10); 	// 0x21 0010 0001 - 8 ��� ���, ������������ COM1B
  TCNT1 = 0x00; 	// ��������� �������� 1
  TIFR = (1<<TOV1); 		// 0x04 0000 0100 - ����� ����� ������������  �������� 1 
  TIMSK = (1<<TOIE1); 	// 0x04 0000 0100 - ���������� ���������� ������������ �������� 1 
  TCCR1B = (1<<CS10); 	// 0x01 0000 0001 - ����. ��������� �������� 1 = 1
  OCR1B = 0x00;		// ��������� ��������� �������� ��������� B

    // ���������� ���������, ���� SPI �������, �������� �����, ������ MSB, 3 ����� SPI, Fcl/4
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA);  // 0x5C=92 0101 1100

  next_page_to_next_buffer (active_buffer, page_counter); // ������ �������� 0 � ����� 1
	
  while (!(PINB & DF_RDY_BUSY)); // �������� ���������� �������� ������ �� �������� 0 � ����� 1
  while ((page_counter < dataflash_pages-1)&(!(PINE & Button_Play))) // ���� ������ ��������������� (������ 1) ������
    {
      page_counter++; // ������ ���� ��������� ��������

      next_page_to_next_buffer (active_buffer, page_counter);
      active_buffer_to_speaker (active_buffer);

      if (active_buffer == 1) // ���� ����� 1 �������� �������� �������
        {
          active_buffer++; 	// �� ������������� ����� 2 � �������� ���������
        }
      else 			// �����
        {
          active_buffer--; 	// ������������� ����� 1 � �������� ���������
        }
      }
  TIMSK = 0x00; 	// ��������� ��� ����������
  TCCR1B = 0x00; 	// ������������� ������� 1
  SPCR = 0x00; 		// ��������� SPI
}


void next_page_to_next_buffer (unsigned char active_buffer, unsigned int page_counter)
{
  unsigned char temp = 0x80;
  while(!(PINB & DF_RDY_BUSY)); 	// ���, ���� ���� �� �����������

  PORTB &= ~DF_CHIP_SELECT;     // �������� DataFlash

  if (active_buffer == 1) 	// ���� ����� 1 ��������
    {
      SPDR = MM_PAGE_TO_B2_XFER; // �� ������� ��������� �������� � ����� 2
    }
  else	 				// �����
    {
      SPDR = MM_PAGE_TO_B1_XFER; // ������� ��������� �������� � ����� 1
    }
  while (!(SPSR & temp)); // ������� ���������� ��������
  SPDR = (char)(page_counter >> 6);
  while (!(SPSR & temp)); // ������� ���������� ��������
  SPDR = (char)(page_counter << 2);
  while (!(SPSR & temp)); // ������� ���������� ��������
  SPDR = 0x00; // ���������� �� ������� �������� ����
  while (!(SPSR & temp)); // ������� ���������� ��������
  PORTB |= DF_CHIP_SELECT; // ��������� DataFlash � �������� ��������
}

#pragma vector=TIMER1_OVF_vect
__interrupt void out_now(void)
{
  wait = 0; // ������������� ����������
}

void active_buffer_to_speaker (unsigned char active_buffer)
{
  // ���� �������� ����� �� ��������� ������������� ��� ���������� �� ��������
  unsigned int buffer_counter = 0;
  unsigned char temp = 0x80;

  PORTB &= ~DF_CHIP_SELECT; // ��������� DataFlash

  if (active_buffer == 1)   // ���� ����� 1 �������� �����
    {
      SPDR = BUFFER_1_READ; // �� ������ �� ������ 1
    }
  else 	// �����
    {
      SPDR = BUFFER_2_READ; // ������ �� ������ 2
    }

  while (!(SPSR & temp)); // ������� ���������� �������� 
  SPDR = 0x00;            // ������ �� �������� �������� �����
  while (!(SPSR & temp)); // ������� ���������� ��������
  SPDR = 0x00;            // ������ �� �������� �������� �����
  while (!(SPSR & temp)); // ������� ���������� ��������
  SPDR = 0x00;            // ������ � ������ 0 ������
  while (!(SPSR & temp)); // ������� ���������� ��������
  SPDR = 0x00;            // ������ �� �������� �������� �����
  while (!(SPSR & temp)); // ������� ���������� ��������

  while (buffer_counter < bytes_per_page)
    {
      SPDR = 0xFF; // ���������� ��������� �������� � ������ ���������� ��������
      while (!(SPSR & temp)); // ������� ���������� ��������
      while(wait); // ������� ���������� ������������ ������� 1
      OCR1B = SPDR; // ������������� ������ �� ���������� ��������
      wait = 1; // ����� ����� �������
  
      buffer_counter++;
    }

  PORTB |= DF_CHIP_SELECT; // ���������� DataFlash
}

// ==========================================================================
// ������ 1 - PLAY
// ������ 2 - ERASE
// ������ 3 - RECORD
// ===========================================================================

// static __flash char Copyright[] = {"By (R)soft 2011"};

void main(void)
{
  setup();
//  SPSR = (1<<SPI2X); // ��������� �������� SPI
  
  for(;;)
   {
      if (!(PINE & Button_Record)) // ���� ������ ������ ������ (������ 3)
        {
          PORTD |= Data_LED; // �������� LED
          recording();
        }
      if (!(PINE & Button_Erase)) // ���� ������ ������ ������� (������ 2)
        {
          PORTD |= Data_LED; // �������� LED
          erasing();
          while (!(PINE & Button_Erase)); // ��� ���� ������ ������� �� �������� (������ 2)
        }
      if (!(PINE & Button_Play)) //���� ������ ������ ���������������(������ 1)
        {
          PORTD |= Data_LED; // �������� LED
          playback();
          while (!(PINE & Button_Play)); // ��� ���� ������ ��������������� �� �������� (������ 1)
        }
      PORTD &= ~Data_LED; // ����� LED �� ����� ��������� ������
//      PORTA=Copyright[0];
   }
}

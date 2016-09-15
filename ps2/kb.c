// ����� ���� ������� 11-������� ���� �������� ~ 0,7 ��;
// ������ ������ ����� ~ 66 ��� (15 ���) (0,066*11=0,726 ��);
// ������� � ������ ����������� ��������� ������� �������� ����� ����� ����� - 33 ���;
// � ���� ������ ��������� ��� ��������� ���-�� ���������.

#include "keyboard.h"
#include "scancodes.h"

//===============================================================

void init_kb(void)
{
    inpt =  kb_buffer; // ������������� ������
    outpt = kb_buffer;
    buffcnt = 0;

    MCUCR = (0<<ISC01)|(1<<ISC00);  // ��������� ���������� INT0 �� ��������� ���. ���������
//    edge_flag = 0;       // ���� ������: 0 = ��������� ����� 1 = ����������� �����
    bitcount = 11;  // ���������=0, 8 ��� ������, �����������, ��������=1
//    kbd_data_byte=0;
}

//===========================================================
// ������ ��������� ���� ������� � ����� ����������

void put_kbbuff(unsigned char c)
{
    if (buffcnt<KBD_BUFF_SIZE)    // If buffer not full
    {
        *inpt = c;                // ��������� ��� ������� � ������
        inpt++;                   // Increment pointer

        buffcnt++;    // ������� �������� ����� ������ �����������
    
        if (inpt >= kb_buffer + KBD_BUFF_SIZE)        // Pointer wrapping
          {
            inpt = kb_buffer;
          }
    }
}

//===========================================================
// ��������� �� ����� �������� �������� ���� � ����������


void decode(unsigned char sc)
{
    unsigned char i;

    // ���� is_up=0
    if (!is_up)       // ��������� �������� ������������� ��� ��������������� ���������� ������
     {
        switch (sc)
        {
          case 0xF0 :        // ������������� ���������� ������
            {
              is_up = 1;
              break;
            }
          case 0x12 :        // ����� SHIFT
            {
              shift = 1;
              break;
            }
          case 0x59 :        // ������ SHIFT
            {
              shift = 1;
              break;
            }
          case 0x05 :        // F1
            {
              if(mode == 0)
                {
                  mode = 1;    // ���� � ����� ����-����
                }
              if(mode == 2)
                {
                  mode = 3;    // ����� �� ������ ����-����
                }
              break;
            }

          default:
            {
              if(mode == 0 || mode == 3)        // ���� ����� ASCII
               {
                  if(!shift)                    // ���� SHIFT �� ������
                    {                            // ����� ���� ����������� �������
                      for(i = 0; unshifted[i][0]!=sc && unshifted[i][0]; i++)
                        {
                          if (unshifted[i][0] == sc) 
                            {
                              put_kbbuff(unshifted[i][1]);
                            }
                        }
                     }
                  else 
                    {                    // ���� SHIFT ������
                      for(i = 0; shifted[i][0]!=sc && shifted[i][0]; i++)
                        {
                          if (shifted[i][0] == sc) 
                            {
                              put_kbbuff(shifted[i][1]);
                            }
                        }
                    }
               }
              else
               {                            // ����� ����-����
//                  print_hexbyte(sc);            // ����� ����-����
                  put_kbbuff(' ');
                  put_kbbuff(' ');
               }
              break;
            }
        }
      } 
    else 
     {
        is_up = 0;                            // ��� ���� 0xF0 � ������ ����������
        switch (sc)
        {
          case 0x12 :                        // ����� SHIFT
            {
              shift = 0;
              break;
            }
          case 0x59 :                        // ������ SHIFT
            {
              shift = 0;
              break;
            }
          case 0x05 :                        // F1
            {
              if(mode == 1)
                {
                  mode = 2;
                }
              if(mode == 3)
                {
                  mode = 0;
                }
              break;
            }
          case 0x06 :                        // F2
            {
//              clr();
              break;
            }
         } 
      }   
} 

//=============================================================================
// ���������� ������� �� ��� �����, ������ - ��������� ���������� ������ CLK
// ������ - ��������� ������������ ������ CLK.

#pragma vector=INT0_vect
__interrupt void INT0_interrupt(void)
{

    if (TimeOut_KBD==0 && bitcount==11) // �������� �� ������ ���������
      {
        kbd_data_byte=0; // �������� ������ ��� ���������� �����
        kbd_data_mask=0x01;
        TimeOut_KBD=TIMEOUT_KEYBOARD; // �������������� �������� �� ���� �� ������� � ��� ��� �������!
      }
  
    if (KBD_PIN & (1<<KBD_CLK))      // ����� ����� � ����������� �� ���� ����� ������� CLK
      {                            //----------------------- ���. ������� CLK = 1 - �����
        LED_PORT|=(1<<LED_KBD);  // ����� LED

        bitcount--; // �� ������ ��������� ������� �����
        TimeOut_KBD=TIMEOUT_KEYBOARD; // ����� ������ ���������� ���� �� ���� ��������� ��������
        
        if (bitcount == 0)                    // ��� ���� ��������?
          {
            put_kbbuff(kbd_data_byte); // ���������� �������� ���� � ��������� �������������
//            bitcount = 11;  // �������������� ������� ��� ��� ���������� ������ �����
          }
        
       } 
    else 
     {                                // --------------------- ���. ������� CLK = 0 - ���� ������ ����
        LED_PORT&=~(1<<LED_KBD); // �������� LED - ������
         if(bitcount > 2 && bitcount < 11)    // ���� � 3 �� 10 � ���� ������. Parity bit,
            {                                    // �����- � ����- ���� ������������
              if(KBD_PIN & (1<<KBD_DATA))  // ����� �������� ���� DATA
                {
                  kbd_data_byte |= kbd_data_mask;  // ���� ���� ������� �� ������ OR ���������� � �����
                }
              kbd_data_mask<<=1; // ������� ����� �����
            }
      }
}

//--------------------------------------------------------

unsigned char getchar_kbd(void)
{
    unsigned char byte;
    while(buffcnt == 0); // �������� ������ - ������ � ����� ���� �������=0

      byte = *outpt;                                // Get byte
      outpt++;                                    // Increment pointer

      if (outpt >= kb_buffer + KBD_BUFF_SIZE)            // �������� ���������
        {
          outpt = kb_buffer;
        }
    
      buffcnt--;                                    // Decrement buffer count

    return byte;
}


#include "modbus.h"

//======================================================================
// The function returns the CRC16 as a unsigned short type
//======================================================================
// Name :  CRC16
// Width : 16
// Poly :  A001
// Init :  FFFF
// ������� ��������� ��� ���������: 
//	unsigned int *pBuf; ��������� �� �����
//	unsigned char DataLen; ���������� ���� � ������
//	������� ���������� CRC ��� ��� unsigned int. 

unsigned short CRC16(unsigned char *pBuf, unsigned char DataLen)
{
  unsigned char CRCHi = 0xFF;
  unsigned char CRCLo = 0xFF; 
  unsigned char Index;
  while (DataLen--)
    {
      Index = CRCLo ^ *pBuf++; // calculate the CRC
      CRCLo = CRCHi ^ Table_CRC_H[Index];
      CRCHi = Table_CRC_L[Index];
    };
  return ((CRCHi<<8) | CRCLo); 
} 

//===============================================================
// �� �� ����� ��� ���������� ���������, ������ ����� ���������
// ��������� CRC16 � ������, �� �������� �������� CRC16

void CRC16_And_Store(unsigned char *pBuf, unsigned char DataLength)
{
  unsigned char CRCHi = 0xFF;
  unsigned char CRCLo = 0xFF; 
  unsigned char Index;
  while (DataLength--)
    {
      Index = CRCLo ^ *pBuf++;
      CRCLo = CRCHi ^ Table_CRC_H[Index];
      CRCHi = Table_CRC_L[Index];
    };
  *pBuf++=CRCLo;
  *pBuf++=CRCHi;
}

//======================================================================
// ������� 3 ��������� Modbus (0x03):
// ������ �������� �� ���������� ��������� ����� (Read Input Registers).
// �� ����� unsigned char length_rx - ���������� �������� ���� � ������.
// ������:
//  0       1    2        3    4    5    6       7
// SLV_ADR FUNC HI_ADR LO_ADR HI_N LO_N  CRCLo CRCHi
// �����:
//  0       1    2                  
// SLV_ADR FUNC NBYTES DAT1...DATN CRCLo CRCHi (256-5)/2=125,5
// ������������ �����:
// 02 03 01 80 00 32 C4 38
// 02 03 01 40 00 1E C5 D9
// ----------------------------
// 02 03 00 0A 00 01 A4 3B -> 02 03 02 00 00 FC 44
// 02 03 00 02 00 01 25 F9 -> 02 03 02 00 00 FC 44

void Modbus_Function_3 (void)
{

  if (HowMany_RX!=8)  // ���� ����� ������ <> 8, �� ������.
    {
      return; // ����� ������� �� ���������
    }

  if (UART_Buf[4]!=0) // ���� ������� ���� ���������� ���� ������� <>0,
                      // �� ������ (�������� � ���� ����� ������� ������ >512 ����).
    {
      return;
    }

//  unsigned char LenPacketLOW; // ���������� ��� �������� ����� ����� ������ ��� ������
//  LenPacketLOW=UART_Buf[5];
  if ( (UART_Buf[5]==0) || (UART_Buf[5]>125) )  // ���� ������� ���� ���������� ���� ������� > 125,
                     // ��� ����� ���� �� ������ (������ ������ ��� 125 ���� ��������� ������������).
                      // ������� �������� (256-5)/2=125 ���� + 1 ����
    {
      return;
    }

  unsigned int ChSum; // ���������� ��� �������� ����������� �����
  ChSum=CRC16((unsigned char *)UART_Buf,8); // ������� CRC16 ����� ������
  if (ChSum!=0) // ���� ����������� ����� ����� ������ �� ����� ����, �� ������
    {
      return;
    }

  unsigned int AddrFromCopy;
  AddrFromCopy=( ((UART_Buf[2]<<8)|UART_Buf[3]) + OFFSET ); // ������ �������� (����������� ����� + OFFSET)
  unsigned char Bytes;
  Bytes=(UART_Buf[5] * 2); // ����� � ������ ���������� � �����
  UART_Buf[2]=Bytes;      // ���������� ������ - ����� ������ � ������

  unsigned char *p;
  p=(unsigned char *)AddrFromCopy; // ������ ��������
  unsigned char *d=UART_Buf+3;  // �����, ���� �������� (����� UART)

  while (Bytes--)
    {
      *d++=*p++;
    };

//  Bytes = (UART_Buf[2] + 3); // ����� ������ � ������ ���� ��� ������������
  CRC16_And_Store((unsigned char *)UART_Buf, (UART_Buf[2] + 3)); // ��������� CRC16 � ����������� ����� � ����� ������. ����� ������ � ������ ���� ��� ������������
  Transmit(UART_Buf[2] + 5); // ��������� ����� + 3 ������������ + 2 ����� CRC16
}

//=====================================================================
// ������� 6 ��������� Modbus (0x06):
// ������ �������� � ���� ������� �������� (Preset Single Register).
// �� ����� unsigned char length_rx - ���������� �������� ���� � ������
// ������:
//  0       1    2      3      4      5     6 7
// SLV_ADR FUNC HI_ADR LO_ADR HI_DAT LO_DAT CRC
// ����� - ����� �������:
// SLV_ADR FUNC HI_ADR LO_ADR HI_DAT LO_DAT CRC

/*
void Modbus_Function_6 (void)
{

  if (HowMany_RX!=8)  // ���� ����� ������ <> 8, �� ������.
      {
        return;
      }
  
  unsigned int ChSum; // ���������� ��� �������� ����������� �����
  ChSum=CRC16((unsigned char *)UART_Buf, 8); // ������� CRC16 ����� ������
  if (ChSum!=0) // ���� ����������� ����� ����� ������ �� ����� ����, �� ������
      {
        return;
      }
  
  unsigned int AddrToCopy;
  AddrToCopy=( ((UART_Buf[2]<<8)|UART_Buf[3]) + OFFSET); // ���� �������� (����������� ����� + OFFSET)
  unsigned char *p;
  p=(unsigned char *)AddrToCopy;
  *p++=UART_Buf[4]; // ������ � ������� ������ ������� �����
  *p=UART_Buf[5];   // ������ ������� �����
  Transmit(8);  // ��� ����� �������������
}
*/


//============================================================
// ������� 8 Modbus (0x08)
// �� ����� unsigned char length_rx - ���������� �������� ���� � ������
// 02 08 80 00 00 00 C9 F8

void Modbus_Function_8 (void)
{

  if (HowMany_RX!=8)  // ���� ����� ������ <> 8, �� ������.
      {
        return;
      }

  unsigned int ChSum; // ���������� ��� �������� ����������� �����
  ChSum=CRC16((unsigned char *)UART_Buf, 8); // ������� CRC16 ����� ������
  if (ChSum!=0) // ���� ����������� ����� ����� ������ �� ����� ����, �� ������
    {
      return;
    }

 // �������� ������  
  if ( (UART_Buf[2]==0) && (UART_Buf[3]==0) ) // ���� ADDR=0
    {
      Transmit(8);  // �� ������ ��������
      return;   // � ������������
    }

  if ( (UART_Buf[2]==0x80) && (UART_Buf[3]==0) )  // ���� ADDR=0x8000 ��� ������� ���������� ������� � EEPROM
    {
      CRC16_And_Store((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings)-2 ); // ������� ��

      // Write_Config_To_EEPROM
      Write_Config_To_EEPROM (EEPROM_MAIN_SETTINGS_ADDR); // ���������� ������ � ������ �����  
      Write_Config_To_EEPROM (EEPROM_RESERVED_SETTINGS_ADDR); // ���������� ������ � ��������� �����

      Restart();
      Transmit(8);
      return;
    }

  if (UART_Buf[3]==2) // ���� ����� = 0002
    {
      UART_Buf[4]=BSI_ResultEx.BSI_Results.Status.word; // ������ ����� ������
      CRC16_And_Store((unsigned char *)UART_Buf,6); // ��������� CRC16 � ����������� ����� � ����� ������
      Transmit(8); // ���� �����
    }
}


//============================================================
// ������� 16 Modbus (0x10)
// ������ �������� � ��������� ��������� �������� (Preset Multiple Registers)
// �� ����� unsigned char length_rx - ���������� �������� ���� � ������
// ������:
//  0       1    2      3       4       5      6     7    X X  XX
// SLV_ADR FUNC HI_ADR LO_ADR N_RG_HI N_RG_LO NBYTE DAT1 DATN CRC (256-9)/2=123,5
// �����:
//  0       1    2      3      4        5     6 7
// SLV_ADR FUNC HI_ADR LO_ADR N_RG_HI N_RG_LO CRC
// 02 10 01 80 00 32 64 xx xx
// NBYTE ������ ���� ��������� ���������� ���� (N_RG_LO)
// -----------------------------------------------
// ������� 1E
//  02 10 40 00 00 01 02 00 1E 73 6A -> 02 10 40 00 00 01 14 3A
//  Slv

void Modbus_Function_16 (void)
{

  //  unsigned char LenPacketLOW;
  if (UART_Buf[4]!=0) // ���� ������� ���� ������� �� ����� ���� (������ ����� �������)
      {
        return;
      }

//  LenPacketLOW=UART_Buf[5];
  
  if ( (UART_Buf[5]==0) || (UART_Buf[5]>123) ) // ���� ������� ���� ���������� ���� ������� > 123,
                     // ��� ����� ���� �� ������ (������ ������ ��� 123 ����� ��������� ������������).
                    // ������� ������� (256-9)/2=123,5 (���� ���-�� ���� � ������)
      {
        return;
      }

  unsigned char Bytes=(UART_Buf[5] * 2); //
  if (UART_Buf[6]!=Bytes) // �������� ����� ������� � ������ � ������
      {
        return;
      }

  if ( (Bytes+9)!=HowMany_RX )  // ���� ���������� �������� ���� � ������
                                // �� ����� ���������� ���� � ������ ���� ������������ � CRC16
      {
        return;
      }

  unsigned int ChSum; // ���������� ��� �������� ����������� �����
  ChSum=CRC16((unsigned char *)UART_Buf, Bytes+9); // ������� CRC16 ����� ������
  if (ChSum!=0) // ���� ����������� ����� ����� ������ �� ����� ����, �� ������
      {
        return;
      }
  
  unsigned int AddrToCopy;
  AddrToCopy=( ((UART_Buf[2]<<8)|UART_Buf[3]) + OFFSET ); // ���� �������� (����������� ����� + OFFSET)

  // �������� ������
  // ����� ����������� ���������� AddrToCopy, ���� �������, �� ����������. ����� ������������.




  
  Bytes=UART_Buf[6]; // ������� ���� ����������

  unsigned char *p;
  p=(unsigned char *)AddrToCopy;

  unsigned char *d=UART_Buf+7; // ������ ��������� ������
  while (Bytes--)
    {
      *p++=*d++;
    };

  CRC16_And_Store((unsigned char *)UART_Buf, 6); // ��������� CRC16 � ����������� ����� � ����� ������
  Transmit(8);  // ��� ����� �������������

}


//=====================================================================
// ��������� ������������ CRC16 ��������� ������, ����� ������������
// ������� Modbus.

void Calc_Modbus (void)
{
//  UCSR0B&=~(1<<RXEN0); // ��������� ��������
  ReceiveOK=0; // ����� ����� �������� ����������

  // 1 - ��������� �� ��������� ������.
  // � HowMany_RX ���������� ���-�� �������� ���� ������, �� ���� � ������...
  // ���� ��������� �� ����� ������, ������� �� ������, �� ��������� ������ ����
  // ����� ����. �� ����� ��������� ���� ��������� �������� ��������, �����
  // �� ������� ������ ��� ����-�� ������� �� ���������� ���������...

//  CheckSum=CRC16((unsigned char*)UART_Buf,HowMany_RX);
  if (CRC16((unsigned char*)UART_Buf,HowMany_RX)==0)   // ���� CRC16 �������, ��
    {
      switch (UART_Buf[1])  // ����� ����� �������
        {
          case READ_HOLDING_REGISTERS:

                Modbus_Function_3(); // ��������� ������� 3
                break;

          case DIAGNOSTIC:

                Modbus_Function_8();
                break;

          case PRESET_MULTIPLE_REGISTER:

                Modbus_Function_16();
                break;

          default: // ��� ������ ������ - ������

                HowMany_RX=0;
                HowMany_TX=0;
                UCSR0B&=~(1<<TXEN0);
                UCSR0B|=(1<<RXEN0);

        }
    }

// ���� CRC16 �� ������� ��� ����� ������� �� ������ (������ ��������!),
// �� �������� ����������, ��������� ���������� � �������� �������� ������...

//  sprintf(UART_Buf,"CRC16:%04X Len:%X",sum,HowMany_RX);
//  ECHO (16);
//  sprintf(UART_Buf,"CRC16:%04X CRC in Packet: %04X Len:%X",sum,((unsigned int *)(((unsigned char*)UART_Buf)+HowMany_RX-2))[0],HowMany_RX-2);
//  ECHO(37);
  else
    {
      HowMany_RX=0;
      HowMany_TX=0;
      UCSR0B&=~(1<<TXEN0);  // ���������� �����������
      UCSR0B|=(1<<RXEN0);   // ��������� ����� �����
    }
}








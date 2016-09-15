// DataFlash.h

// By (R)soft 26.5.2011
// DataFlash AT45DB321 (32 Mbit, 528 Bytes per 8192 Pages)

//#include "ina90.h"
//#pragma language=extended

// ��������� DataFlash
#define bytes_per_page 528
#define dataflash_pages 8192

// DataFlash ����� ��������� ����� �����/����� (PB 4)
#define DF_RDY_BUSY 0x10

// DataFlash ����� ����� ������ ���������� (PB 5)
#define DF_CHIP_SELECT 0x20

// ����� 1
#define BUFFER_1 0x00

// ����� 2
#define BUFFER_2 0x01

// ����������� ���� ����� ��������

// ������ ������ 1
#define BUFFER_1_WRITE 0x84

// ������ ������ 2
#define BUFFER_2_WRITE 0x87

// ������ ������ 1
#define BUFFER_1_READ 0x54

// ������ ������ 2
#define BUFFER_2_READ 0x56

// ����� 1 � �������� �������� ������ ��������� � ���������� ���������
#define B1_TO_MM_PAGE_PROG_WITH_ERASE 0x83

// ����� 2 � �������� �������� ������ ��������� � ���������� ���������
#define B2_TO_MM_PAGE_PROG_WITH_ERASE 0x86

// ����� 1 � �������� �������� ������ ��������� ��� ����������� ��������
#define B1_TO_MM_PAGE_PROG_WITHOUT_ERASE 0x88

// ����� 2 � �������� �������� ������ ��������� ��� ����������� ��������
#define B2_TO_MM_PAGE_PROG_WITHOUT_ERASE 0x89

// �������� �������� ������ ��������� ������ ����� 1
#define MM_PAGE_PROG_THROUGH_B1 0x82

// �������� �������� ������ ��������� ������ ����� 2
#define MM_PAGE_PROG_THROUGH_B2 0x85

// �������������� ���������� �������� ����� ����� 1
#define AUTO_PAGE_REWRITE_THROUGH_B1 0x58

// �������������� ���������� �������� ����� ����� 2
#define AUTO_PAGE_REWRITE_THROUGH_B2 0x59

// ��������� �������� �������� ������ � ������� 1
#define MM_PAGE_TO_B1_COMP 0x60

// ��������� �������� �������� ������ � ������� 2
#define MM_PAGE_TO_B2_COMP 0x61

// �������� �������� �������� ������ � ����� 1
#define MM_PAGE_TO_B1_XFER 0x53

// �������� �������� �������� ������ � ����� 2
#define MM_PAGE_TO_B2_XFER 0x55

// ������� ��������� DataFlash ��� ������ ���������, ��������� ���������,
// � ��������� �����/�����
#define STATUS_REGISTER 0x57

// ������ �������� �������� ������
#define MAIN_MEMORY_PAGE_READ 0x52

// ������� 528 ���� ��������
#define PAGE_ERASE 0x81

// ������� 512 �������
#define BLOCK_ERASE 0x50

#define TRUE  0xFF
#define FALSE 0x00
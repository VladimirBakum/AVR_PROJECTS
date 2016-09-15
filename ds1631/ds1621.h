#define REPER_POINT       (20)  // �������� ����� 20 ��������

// Definitions for DS1621
#define I2C_ADDR_DS1621   0x90
#define DS_Command_Init   0xAC // Access Config Register
#define DS_Command_Start  0xEE // Start Convert T
#define DS_Command_Read   0xAA // Read Temperature
// 0xA1 Access TH
// 0xA2 Access TL
// 0xA8 Read Counter
// 0xA9 Read Slope
// 0x22 Stop Convert T


signed char TerIn_HI; // ������ ����������� ����������, ����������� �� ������
unsigned char TerIn_LO; // ���� ����������
unsigned char TerIn_2dT; // ������ �� ��� ������ �
unsigned char signTintern; // ����
//unsigned int Tin; // ����� �����������
#define KBD_BUFF_SIZE (32)
#define TIMEOUT_KEYBOARD (2) // ����������� �������� 2 ��

unsigned char bitcount;
unsigned char kb_buffer[KBD_BUFF_SIZE];
unsigned char *input_pointer;
unsigned char *output_pointer;
unsigned char buffcnt;
unsigned char kbd_data_byte; // ���� ��� ������ ��������. ������ � ����������
unsigned char kbd_data_mask;
unsigned char is_up; // ���� ���������� ������
unsigned char shift;  // ���� �������� �����
unsigned char mode; // ���� ������
unsigned char TimeOut_KBD;

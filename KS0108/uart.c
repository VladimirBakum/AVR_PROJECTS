// ���������� UART0 ��� ������ � �������� ����������
void InitUART (void)
#define FOSC      (8000000)
#define Baudrate  (1200)
#define UBRR_Value (FOSC/(16*Baudrate))-1
{  
  UBRR0H=UBRR_Value>>8; // ������� �������� UART0
  UBRR0L=UBRR_Value&0xFF;
  UCSR0A=(0<<U2X0); // ������� ������������ ������ � Double Speed
  UCSR0B=(1<<RXEN0)+(1<<TXEN0); //  ��������� ������ � ��������
}

// �������� ����� � ���������������� ����
void _pch(char c)
{
  while(!UCSR0A_UDRE0); // ����, ���� ������� �������� ����� ����
  UDR0=c; // ����� � ������� �������� ������
}

void _ps_P(char __flash *p)
{
  char c;
  while ((c=*p++)) _pch(c);
}

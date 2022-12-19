#ifndef uartH
#define uartH

void UartInit(void);
//void UartSendString( char* );
void UartSendByte( char );
void UartSendWord( short );
char UartGetByte( char* );
//char UartGetWord( short* );

#endif

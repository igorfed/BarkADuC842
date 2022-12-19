#include <ADI\ADUC842.H>
#include <stdio.h>
#include <ctype.h>

void UartInit(void)
{
/* 
//115200
	T3CON = 0x82; 	
	T3FD = 0x09; 	
	SCON = 0x52;
//*/
///*  
//57600
	T3CON = 0x83; 	
	T3FD = 0x09; 	
	SCON = 0x52;	
//*/
/*  
//38400
	T3CON = 0x83; 	
	T3FD = 0x2D; 	
	SCON = 0x52;
//*/
/* 
//19200
	T3CON = 0x84; 	
	T3FD = 0x2D; 	
	SCON = 0x52;
//*/
/*
//9600
	T3CON = 0x85; 	
	T3FD = 0x2D; 	
	SCON = 0x52;
//*/
}
/*
void UartSendString( char s[] )
{
	unsigned char i=0;
	while( s[i] != 0 )
	{
		putchar( s[i++] );
	}
}
*/
void UartSendTetr( char tmp )
{
	if( tmp < 10 )
	{
		putchar( tmp + '0' );
	}
	else putchar( tmp + '7' );
}

void UartSendByte( char byte )
{
	UartSendTetr( (byte >> 4) & 0x0f );
	UartSendTetr( byte & 0x0f );
}

void UartSendWord( short word )
{
	UartSendTetr( (*((char*)&word) >> 4) & 0x0f );
	UartSendTetr( *((char*)&word) & 0x0f );
	UartSendTetr( (*(((char*)&word)+1) >> 4) & 0x0f );
	UartSendTetr( *(((char*)&word)+1) & 0x0f );
}

char UartGetByte( char *in_byte )
{
	char ch;
	ch = getchar();
	ch = toupper( ch );
	if( ( ch < '0' ) || ( ch > 'F' ) )
	{
		return 0 ;
	}
	if( ch <= '9' )
	{
		*in_byte = (ch - '0')<<4;
	}
	else
    if( ch >= 'A' )
	{
		*in_byte = (ch - '7')<<4;
	}
	else return 0;
	ch = getchar();
	ch = toupper( ch );
	if( ( ch < '0' ) || ( ch > 'F' ) )
	{
		return 0;
	}
	if( ch <= '9' )
	{
		*in_byte = *in_byte | (ch - '0');
	}
	else
    if( ch >= 'A' )
	{
		*in_byte = *in_byte | (ch - '7' );
	}
	else return 0;
	return 1;
}
/*
char UartGetWord( short *in_word )
{
	char ch, *in_byte = (char *)in_word;
	if( UartGetByte( &ch ) )
	{
		*in_byte = ch;
		in_byte++;
		if( UartGetByte( &ch ) )
		{
			*in_byte = ch;
		}
		else return 0;
	}
	else return 0;
	return 1;
}
*/

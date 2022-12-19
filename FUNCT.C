;_________________________________________________________________________________________________*/
#include <ADI\ADUC824.H>
#include <stdio.h>
#include <ctype.h>
#include <absacc.h>

#include "uart.h"
#include "bark.h"

//extern char code s_sum[];
//extern struct st_thres data thres;
extern unsigned char data average;
extern unsigned char data thres_right;
extern char num_ch;
extern unsigned char MemDefine;
unsigned short sum = 0;
unsigned long lsum = 0L;
unsigned long l_sum = 0L;
//unsigned long lhistsum = 0L;

void fTimer(unsigned char , unsigned char );

void DelayStopMotor(unsigned long );

void CalcHistSum( void )
{
	unsigned char i;
	char byte[2];

	sum = 0;
	lsum = 0L;
	l_sum = 0L;
	EX0 = 0; // disable interrupt from RS-485
	XBYTE[MEM_DEFINE] = MEM_HIST;	
	MemDefine = MEM_HIST;			/* save the current value of MEM_DEFINE */
	XBYTE[MEM_ADDR] = 255;			
	thres_right = 0;
	i = 0;
	do
	{
		byte[0] = XBYTE[MEM_DATA_H];
		byte[1] = XBYTE[MEM_DATA_L];
		sum = sum + *((unsigned short*)byte);
		if( CY )	  //???
		{
			i--;
			break;
		}
		i--;
	} while( (i != 0) && (sum < QUANT_R) );
		thres_right = i;
		i = 0;
		if( sum == 0xFFFF )
		{
			do
			{
				byte[0] = XBYTE[MEM_DATA_H];
				byte[1] = XBYTE[MEM_DATA_L];
				lsum = lsum + (unsigned long)(*((unsigned short*)byte));
			}
			while( --i != 0 );
		}
		else
		{
			lsum = (unsigned long)sum;
		}
	EX0 = 1; // enable interrupt from RS-485
}

void CalcAverage( void )
{

	average = XBYTE[AVERAGE]; //reading 0x80FF address
}

bit SelfExamine( )
{
	char tmp, i;
	bit stat = 1;

	tmp = XBYTE[ FILTER ];
    putchar( '\n' );
    putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( 's' );putchar( ' ' );putchar( 'S' );putchar( 't' );putchar( 'a' );putchar( 't' );putchar( 'e' );putchar( 's' );putchar( '=' );
    UartSendByte( tmp );
	
	if( tmp & 0x08  )
	   {putchar( '\n' );
      putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '6' );putchar( ' ' );putchar( '(' );putchar( 'N' );putchar( 'F' );putchar( ')' );putchar( ' ' );putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( ' ' );putchar( 'o' );putchar( 'p' );putchar( 'e' );putchar( 'n' );
	   }	
	if( tmp & 0x20  )
	   {putchar( '\n' );
      putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '5' );putchar( ' ' );putchar( '(' );putchar( 'M' );putchar( 'F' );putchar( ')' );putchar( ' ' );putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( ' ' );putchar( 'o' );putchar( 'p' );putchar( 'e' );putchar( 'n' );
	   }	   
	if( tmp & 0x02  )
	   {putchar( '\n' );
     putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '4' );putchar( ' ' );putchar( '(' );putchar( 'W' );putchar( 'F' );putchar( ')' );putchar( ' ' );putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( ' ' );putchar( 'o' );putchar( 'p' );putchar( 'e' );putchar( 'n' );	
	   }

	for( i = 0; i < 3; i++ )
	{
		XBYTE[ NUM_CHANNEL ] = i;
		XBYTE[ FILTER ] = F_IN;
	}

	TIMECON = 0; // Time Clock disable
	HTHSEC = 0;
	SEC = 0;
	TIMECON = 0x09; // Time Clock enable
	while( SEC < 2 );
	tmp = XBYTE[ FILTER ];
    putchar( '\n' );
    putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 'e' );putchar( 'r' );putchar( 's' );putchar( ' ' );putchar( 'S' );putchar( 't' );putchar( 'a' );putchar( 't' );putchar( 'e' );putchar( 's' );putchar( '=' );
    UartSendByte( tmp );
	if( tmp != 0x2A )
	{
		stat =  0;
	}
	
	if( tmp & 0x08  )
	   {putchar( '\n' );
      putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '6' );putchar( ' ' );putchar( '(' );putchar( 'N' );putchar( 'F' );putchar( ')' );putchar( ' ' );putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( ' ' );putchar( 'o' );putchar( 'p' );putchar( 'e' );putchar( 'n' );
	   }	
	if( tmp & 0x20  )
	   {putchar( '\n' );
      putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '5' );putchar( ' ' );putchar( '(' );putchar( 'M' );putchar( 'F' );putchar( ')' );putchar( ' ' );putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( ' ' );putchar( 'o' );putchar( 'p' );putchar( 'e' );putchar( 'n' );
	   }	   
	if( tmp & 0x02  )
	   {putchar( '\n' );
     putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '4' );putchar( ' ' );putchar( '(' );putchar( 'W' );putchar( 'F' );putchar( ')' );putchar( ' ' );putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( ' ' );putchar( 'o' );putchar( 'p' );putchar( 'e' );putchar( 'n' );	
	   }		

	for( i = 0; i < 3; i++ )
	{
		XBYTE[ NUM_CHANNEL ] = i;
		XBYTE[ FILTER ] = F_OUT;
	}
	TIMECON = 0; // Time Clock disable
	HTHSEC = 0;
	SEC = 0;
	TIMECON = 0x09; // Time Clock enable
	while( SEC < 2 );
	tmp = XBYTE[ FILTER ];

    putchar( '\n' );
    putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 'e' );putchar( 'r' );putchar( 's' );putchar( ' ' );putchar( 'S' );putchar( 't' );putchar( 'a' );putchar( 't' );putchar( 'e' );putchar( 's' );putchar( '=' );
    UartSendByte( tmp );

	if( tmp & 0x04  )
	   {putchar( '\n' );
      putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '6' );putchar( ' ' );putchar( '(' );putchar( 'N' );putchar( 'F' );putchar( ')' );putchar( ' ' );putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( ' ' );putchar( 'c' );putchar( 'l' );putchar( 'o' );putchar( 's' );putchar( 'e' );
	   }	
	if( tmp & 0x10  )
	   {putchar( '\n' );
      putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '5' );putchar( ' ' );putchar( '(' );putchar( 'M' );putchar( 'F' );putchar( ')' );putchar( ' ' );putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( ' ' );putchar( 'c' );putchar( 'l' );putchar( 'o' );putchar( 's' );putchar( 'e' );	   
	   }	
	if( tmp & 0x01  )
	   {putchar( '\n' );
     putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '4' );putchar( ' ' );putchar( '(' );putchar( 'W' );putchar( 'F' );putchar( ')' );putchar( ' ' );putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( ' ' );putchar( 'c' );putchar( 'l' );putchar( 'o' );putchar( 's' );putchar( 'e' );	
	   }

	if( tmp != 0x15 )
	{
		stat =  0;
	}
	   
	if( stat )
	   {
         putchar( '\n' );
		 putchar( '\n' ); putchar( 'C' );putchar( 'a' );putchar( 'm' );putchar( 'e' );putchar( 'r' );putchar( 'a' );putchar( ' ' );putchar( 'r' );putchar( 'e' );putchar( 'a' );putchar( 'd' );putchar( 'y' );putchar( '.' );
         putchar( '\n' );
	   }	

	return stat;
}


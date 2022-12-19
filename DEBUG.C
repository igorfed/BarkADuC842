#include <ADI\ADUC842.H>
#include <stdio.h>
#include <ctype.h>
#include <absacc.h>
#include <intrins.h>

#include "uart.h"
#include "bark.h"
#include "funct.h"
#include "adc.h"
#include "focus.h"

//#define RS485_DEBUG_BUILD

extern char code s_enter_debug[];
extern char code s_help[];
extern char code s_help1[];
extern char code s_help2[];
extern char code s_help_parameter[];
extern char code s_help_settings[];
extern char code s_debug_error[];
//extern char code s_debug_prompt[];
extern char code s_hist_error[];
extern char code s_ram_error[];

extern char code s_exit_adc_debug[];
extern char code s_adc_channel3[];
extern char code s_adc_channel4[];
extern char code s_adc_channel5[];	
extern char code s_single_ADC_mode[];
extern char code s_cont_ADC_mode[];

extern short idata shutter[];

extern short idata gain[];
extern unsigned char idata frame[];
extern short idata index[];

extern unsigned char data average;
struct exp code exp_table[];

bit bStep = 0;
extern char status;
extern bit bHistRdy;
extern bit bRAMRdy;
extern char num_ch;
extern unsigned char VideoMux;

extern unsigned char idata ucAnalogError;
extern unsigned char idata ucInfinity_1_5;
extern unsigned char idata ucFocusMin_1_5;
extern unsigned char idata ucFocusMax_1_5;
extern unsigned char idata ucFocusMin_6;
extern unsigned char idata ucFocusMax_6;
extern unsigned char idata ucInfinity_6;
extern unsigned char data adc_h[4];
extern unsigned char data adc_l[4];

extern bit bFilterDis;

extern short nnn;
extern unsigned char temp_nord;
extern unsigned char corr_delay;
extern unsigned char ManFocus[4];
extern unsigned char num_adc_ch;
extern unsigned char focus[3];
extern bit bADCdebug;
extern bit bAdcAutoDebug;
extern bit bDebug;
extern char idata bNEAR[4];
extern char idata bFAR[4];
extern char idata bQ_NEAR[4];
extern char idata bQ_FAR[4];
unsigned char idata cDeltaFoc[4] = {0x00, 0x00, 0x00, 0x00};
extern unsigned char idata ucRcapL;
extern unsigned char idata ucRcapH;


extern unsigned char data frame_limit;
extern unsigned short exp_delta;
extern unsigned short shutter_limit;
extern unsigned short exp_delta_fine;

extern unsigned char ex_corr;

extern bit bDis_Auto;

	
void CalcExpDelta();
void CalcExpDeltaFine();
void fAutoFocus();

void PrintAndSetParam()
{

	unsigned short array_index;
			EA = 0;//EADC	= 0;
				array_index = index[ num_ch ];
	if( !bDis_Auto )
	{
				shutter[ num_ch ] = exp_table[ array_index ].shutter;
	}
				if( shutter[ num_ch ] > shutter_limit )
				{
					shutter[ num_ch ] = shutter_limit;
					array_index += exp_delta_fine;
					if( array_index > INDEX_MAX )
					{
						array_index = INDEX_MAX;
					}
				}
	if( !bDis_Auto )
	{
				frame[ num_ch ] = exp_table[ array_index ].frame;
	}
				if( frame[ num_ch ] > frame_limit )
				{
					frame[ num_ch ] = frame_limit;
					array_index += exp_delta;
					if( array_index > INDEX_MAX )
					{
						array_index = INDEX_MAX;
					}
				}
				XBYTE[A_FRAME] = frame[ num_ch ];

				XBYTE[A_SHUTTER_H] = *((char*)&shutter[ num_ch ]);
				XBYTE[A_SHUTTER_L] = *(((char*)&shutter[ num_ch ])+1);
	if( !bDis_Auto )
	{
				gain[ num_ch ] = exp_table[ array_index ].gain;
	}
//				putchar( ' ' );putchar( 'g' );putchar( 'a' );putchar( 'i' );putchar( 'n' );putchar( ':' );putchar( '=' );putchar( ' ' );					  
//				UartSendWord( gain[ num_ch ] );
				XBYTE[A_GAIN_H] = *((char*)&gain[ num_ch ]);
				XBYTE[A_GAIN_L] = *(((char*)&gain[ num_ch ])+1);
				putchar( ' ' );putchar( ' ' );putchar( ' ' );putchar( ' ' );putchar( ' ' );
				putchar( 'f' );putchar( 'r' );putchar( 'a' );putchar( 'm' );putchar( 'e' );putchar( ':' );putchar( '=' );putchar( ' ' );					  
				UartSendWord( frame[ num_ch ] );
				putchar( ' ' );putchar( 's' );putchar( 'h' );putchar( 'u' );putchar( 't' );putchar( ':' );putchar( '=' );putchar( ' ' );					  
				UartSendWord( shutter[ num_ch ] );
				putchar( ' ' );putchar( 'g' );putchar( 'a' );putchar( 'i' );putchar( 'n' );putchar( ':' );putchar( '=' );putchar( ' ' );					  
				UartSendWord( gain[ num_ch ] );putchar( '\n' );
			//	puts("\n");

				EA = 1;//	EADC	= 1;
}

void PrintAndSetGain()
{
				UartSendWord( gain[ num_ch ] );
				XBYTE[A_GAIN_H] = *((char*)&gain[ num_ch ]);
				XBYTE[A_GAIN_L] = *(((char*)&gain[ num_ch ])+1);
}
void PrintAndSetShutter()
{
				UartSendWord( shutter[ num_ch ] );
				XBYTE[A_SHUTTER_H] = *((char*)&shutter[ num_ch ]);
				XBYTE[A_SHUTTER_L] = *(((char*)&shutter[ num_ch ])+1);
}

void DelayStopMotor(unsigned long ulDelay )
{
  while(ulDelay!=0x00000000)
			{
			ulDelay--;
			}	
}

void fTimer(unsigned char ucMode, unsigned char ucValue )
{
			TIMECON = 0; 
			HTHSEC = 0;
			SEC = 0;
			TIMECON = 0x09; 

	    if( ucMode == SEC ) while( SEC < ucValue );			
			else while( HTHSEC < ucValue );
}

void fFocusParamInit(void)
{
				EADRH = 0x00;
				EADRL = 0x00;
				ECON = 1;
				ECON = 4;
				ucInfinity_1_5 = EDATA1;
				ucFocusMin_1_5 = EDATA2;  
				ucFocusMax_1_5 = EDATA3;
				ucAnalogError  = EDATA4;
				
				EADRH = 0x00;
				EADRL = 0x01;
				ECON = 1;
				ECON = 4;
				ucInfinity_6 = EDATA1;
				ucFocusMin_6 = EDATA2;  
				ucFocusMax_6 = EDATA3;	
													
				EADRH = 0x00;
				EADRL = 0x03;
				ECON = 1;
				ECON = 4;
				ucRcapH = EDATA1; 				
				ucRcapL = EDATA2;
}

void GetAndPrintAdcOne(unsigned char ucModeCs)
{
	DelayStopMotor(0xFF);    		
	ADCCON1 = 0xBC;				                                                   	
	ADCCON2 = ucModeCs;	 
}

void Debug( void )
{	
	unsigned char *ptr_parametres, i, parameter_min, value, parameter_max,  expo_data;	

	unsigned char ch, addr_l, dummy_l = 0xff, addr_h, dummy_h = 0xff,num_bytes_tx,num_bytes_rx,cAdcModeCS = ADCS_SINGL_CONV_1_5;	

	short si;
	short *paddr = (short *)&addr_l;

	puts( s_enter_debug );
	fFocusParamInit();

	while( bDebug )
	{

		putchar('\n');
		putchar('-');
		ch = getchar();
		ch = toupper( ch );
		switch( ch )
		{

			case '?':
				EADRH = 0x00;
				EADRL = 0x02;
				ECON = 1;
				ECON = 4;
                putchar('\n');putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '8' );putchar( ' ' );putchar( '#' );
		        for( i = 1; i < 4; i++ )
		           {
			        switch( i )
				       {
			   		    case 1:
					        value = EDATA1;
					     	UartSendByte( value );
				            break;
			   		    case 2:
					     	value = EDATA2;
					     	UartSendByte( value );
				            break;
			   		    case 3:
					     	value = EDATA3;
					     	UartSendByte( value );
				            break;
					    default:
				            break;
                       }
				   }
                putchar( ' ' );
				EADRH = 0x00;
				EADRL = 0x04;
				ECON = 1;
				ECON = 4;	   
			    putchar( ' ' );  putchar( 'l' );  putchar( 'o' );  putchar( 'a' );  putchar( 'd' );  putchar( 'e' );  putchar( 'd' );  putchar( ' ' );
		    	//"loaded 21.06.2011"
				for( i = 1; i < 5; i++ )
				   {
			        switch( i )
				       {
			   		    case 1:
					        value = EDATA1;
					     	UartSendByte( value );
			                putchar( '.' );
				            break;
			   		    case 2:
					     	value = EDATA2;
					     	UartSendByte( value );
			                putchar( '.' );
				            break;
			   		    case 3:
					     	value = EDATA3;
					     	UartSendByte( value );
				            break;
			   		    case 4:
					     	value = EDATA4;
					     	UartSendByte( value );
				            break;
					      default:
				            break;
			         }
				   }
				puts( s_help );
				break;
				
			case '1':
				bDebug = 0;
				bStep = 1;
				break;
				
			case 'M':
				bDebug = 0;
				bStep = 0;
				break;
				
	    case 'F':	
			ch = getchar();
			ch = toupper( ch );
			switch( ch )
			   {		
			    case '?':
				   EADRH = 0;
				   if( UartGetByte( &expo_data ) )
    		           {
	                   for( addr_l = 0; addr_l < expo_data; addr_l++ )
	                      {
						   EADRL = addr_l;
						   ECON = 1;
	                       ECON = 4;
	                       for( i = 1; i < 5; i++ )
	                          {
	         	               switch( i )
	                               {
		                            case 1:
		     	                        value = EDATA1;
		     		                    UartSendByte( value );
		     	                        putchar(' ');
	                                    break;
		                            case 2:
	     	                     	    value = EDATA2;
	     		                        UartSendByte( value );
		     	                        putchar(' ');
	                                    break;
		                            case 3:
		     	                        value = EDATA3;
		     		                    UartSendByte( value );
		     	                        putchar(' ');
	                                    break;
		                            case 4:
		     	                        value = EDATA4;
		     		                    UartSendByte( value );
		     	                        putchar(' ');
	                                    break;
		                            default:
	                                    break;
	                               }
							  }
		     	           putchar('\n');
					      }
	     	           }
	     	       else puts( s_debug_error );
                   break;
					   	
			    case 'E':
			        ch = getchar();
			        ch = toupper( ch );
			        switch( ch )
			            {
			   	         case 'A':
						     ECON = 6;
						   	 EADRH = 0;
							 EADRL = 0;
	                         ECON = 1;
	                         ECON = 4;
	                         break;

			   		     case 'P':
					         if( UartGetByte( &addr_h ) )
					             {
						          if( UartGetByte( &addr_l ) )
							         {
                                      EADRH = addr_h;
								      EADRL = addr_l;
								      ECON = 5;
                                      ECON = 4;
							          }
				     	          else puts( s_debug_error );
				     	         }
					         else puts( s_debug_error );
                             break;
				            
						 default:
						     puts( s_debug_error );
				             break;
                        }
                   break;

		       case 'W':
			       ch = getchar();
			       ch = toupper( ch );
			       switch( ch )
				           {
			   		        case 'B':
					              if( UartGetByte( &addr_h ) )
					                 {
						                if( UartGetByte( &addr_l ) )
						     	             {
				     		                if( UartGetByte( &i ) )
								                   {
				     		                    if( UartGetByte( &value ) )
									                     {
                                        
									      	              EADRH = addr_h;
									      	              EADRL = addr_l;
									      		            
								      			            ECON = 1;
			             	                    switch( i )
				                                   {
			   		                                case 1:
                                                EDATA1 = value;
				                                        break;
			   		                                case 2:
                                                EDATA2 = value;
				                                        break;
			   		                                case 3:
                                                EDATA3 = value;
				                                        break;
			   		                                case 4:
                                                EDATA4 = value;
				                                        break;
					                                  default:
				                                        break;
                                           }
											                  ECON = 5;
											                  ECON = 2;
                                        ECON = 4;
											                 }
				     	             	        else puts( s_debug_error );
										               }
				     	                  else puts( s_debug_error );
								               }
				        	          else puts( s_debug_error );
				     	             }
					              else puts( s_debug_error );
                        break;
			   		        case 'P':
					              if( UartGetByte( &addr_h ) )
					                 {
						                if( UartGetByte( &addr_l ) )
						     	             {
                                
									              EADRH = addr_h;
									              EADRL = addr_l;
		                            for( i = 1; i < 5; i++ )
		                               {
				     		                    if( UartGetByte( &value ) )
									                     {
			             	                    switch( i )
				                                   {
			   		                                case 1:
                                                EDATA1 = value;
				                                        break;
			   		                                case 2:
                                                EDATA2 = value;
				                                        break;
			   		                                case 3:
                                                EDATA3 = value;
				                                        break;
			   		                                case 4:
                                                EDATA4 = value;
				                                        break;
					                                  default:
				                                        break;
                                           }
     										               }
				     	             	        else puts( s_debug_error );
											   }
										   ECON = 5;
										   ECON = 2;
                                           ECON = 4;
								               }
				        	          else puts( s_debug_error );
				     	             }
					              else puts( s_debug_error );
                           break;
					          default:
						            puts( s_debug_error );
				                break;
                   }
                break;

				    case 'R':
			          ch = getchar();
			          ch = toupper( ch );
			          switch( ch )
				           {
			   		        case 'B':
					              if( UartGetByte( &addr_h ) )
					                 {
						                if( UartGetByte( &addr_l ) )
						     	             {
				     		                if( UartGetByte( &i ) )
									                 {
                                    
										                EADRH = addr_h;
										                EADRL = addr_l;
											              
											              ECON = 1;
                                    ECON = 4;
			             	                switch( i )
				                               {
			   		                            case 1:
					     	                     	      value = EDATA1;
					     	                            putchar(' ');
					     		                          UartSendByte( value );
				                                    break;
			   		                            case 2:
					     	                     	      value = EDATA2;
					     	                            putchar(' ');
					     		                          UartSendByte( value );
				                                    break;
			   		                            case 3:
					     	                     	      value = EDATA3;
					     	                            putchar(' ');
					     		                          UartSendByte( value );
				                                    break;
			   		                            case 4:
					     	                     	      value = EDATA4;
					     	                            putchar(' ');
					     		                          UartSendByte( value );
				                                    break;
					                              default:
				                                    break;
                                       }
											             }
				     	             	    else puts( s_debug_error );
										           }
				     	              else puts( s_debug_error );
								           }
					              else puts( s_debug_error );
                        break;

			   		        case 'P':
					              if( UartGetByte( &addr_h ) )
					                 {
						              if( UartGetByte( &addr_l ) )
						     	          {
										   EADRH = addr_h;
										   EADRL = addr_l;
										   ECON = 1;
                                           ECON = 4;
		                                   for( i = 1; i < 5; i++ )
		                                    {
			             	                 switch( i )
				                               {
			   		                            case 1:
					     	                     	      value = EDATA1;
					     		                          UartSendByte( value );
					     	                            putchar(' ');
				                                    break;
			   		                            case 2:
					     	                     	      value = EDATA2;
					     		                          UartSendByte( value );
					     	                            putchar(' ');
				                                    break;
			   		                            case 3:
					     	                     	      value = EDATA3;
					     		                          UartSendByte( value );
					     	                            putchar(' ');
				                                    break;
			   		                            case 4:
					     	                     	      value = EDATA4;
					     		                          UartSendByte( value );
					     	                            putchar(' ');
				                                    break;
					                            default:
				                                    break;
                                               }
										  }
								         }
				     	             else puts( s_debug_error );
								     }
					              else puts( s_debug_error );
                                  break;
  		                    default:
						        puts( s_debug_error );
				                break;
				           }
				        break;

					default:
						puts( s_debug_error );
				        break;
				   }
			  break;
			  

//#ifdef RS485_DEBUG_BUILD

			case '4':	// write command to rs485 
	                            //XBYTE[ RS485_CTRL ] = RS_PRESET;//0x40ff//0x0a
	    	    ch = getchar();
	    	    ch = toupper( ch );				           
	    	    switch( ch )
				       {
				        case 'R'://
							if( UartGetByte( &num_bytes_rx ) )
							   {
								putchar('\n');
	 		                    XBYTE[ 0x46FF ]   = 0x02;
				                XBYTE[MEM_ADDR]   = 0x00;
				                XBYTE[MEM_ADDR_H] = 0x00;
	                            putchar('R');putchar('X');putchar('-');putchar('>');
				                for( i = 0; i < num_bytes_rx; i++ )
				                    {
						            value = XBYTE[RS485_CTRL];
									value = XBYTE[0x12FF];
									UartSendByte( value );
	                                putchar(' ');
				                    }
							   }
							else puts( s_debug_error );

				         	break;
	
				        case 'T'://
							if( UartGetByte( &num_bytes_tx ) )
								{
								putchar('\n');
	 		                    XBYTE[ 0x46FF ]   = 0x01;
				                XBYTE[MEM_ADDR]   = 0x00;
				                XBYTE[MEM_ADDR_H] = 0x00;
	                            putchar('T');putchar('X');putchar('-');putchar('>');
				                for( i = 0; i < num_bytes_tx; i++ )
				                    {
						            value = XBYTE[RS485_CTRL];
									value = XBYTE[0x12FF];
									UartSendByte( value );
	                                putchar(' ');
				                    }
								}
							else puts( s_debug_error );

				         	break;
						            
				        case 'D'://rs485 DEBUG            					
				            puts( "\nenter number of bytes for load to receiver\n");
						    putchar('R');putchar('S');putchar(' ');putchar('R');putchar('C');putchar('-');putchar('>');
							if( UartGetByte( &num_bytes_rx ) )
							   {
								XBYTE[ RS485_NRCV ] = num_bytes_rx;
							   }
							else puts( s_debug_error );				
				            puts( "\nenter number of bytes for load to transfer\n");
						    putchar('R');putchar('S');putchar(' ');putchar('T');putchar('R');putchar('-');putchar('>');
							if( UartGetByte( &num_bytes_tx ) )
								{
								 XBYTE[ RS485_NSND ] = num_bytes_tx;
								}
							else puts( s_debug_error );				
				            puts( "\nenter number of bytes for load to error counter\n");
						    putchar('E');putchar('R');putchar('R');putchar(' ');putchar('-');putchar('>');                        
							if( UartGetByte( &value ) )
								{
								 XBYTE[ RS485_ERR ] = value;
							    }
							else puts( s_debug_error );
	                        puts( "\nloading RAM tx...\n");
		                    XBYTE[ RS485_CTRL ] = 0x2b;
			                XBYTE[MEM_ADDR] = 0x00;
			                XBYTE[MEM_DEFINE] = MEM_RS485;
	                        parameter_min = 0;
			                for( i = 0; i < num_bytes_tx; i++ )
			                    {
	                             puts( "\nbyte");//printf( "\nbyte");//puts
			                     UartSendByte( i );	
			                     putchar( '=' );
								 if( UartGetByte( &value ) )
								    {
				                     XBYTE[MEM_DATA_L] = value;
								    }
								 else puts( s_debug_error );
	                             parameter_min = parameter_min ^ value;
				                 putchar( '\n' );
			                    }
				            XBYTE[MEM_DATA_L] = parameter_min;
	                        puts( "\nxor byte=");
			                UartSendByte( parameter_min );
				            putchar( '\n' );
	
	
	                        puts( "\nreading RAM tx...\n");
			                XBYTE[MEM_DEFINE] = 0x00;
			                XBYTE[MEM_ADDR] = 0x00;
		                    //XBYTE[ RS485_CTRL ] = 0x2a;//was loaded
			                XBYTE[MEM_DEFINE] = MEM_RS485;
			                for( i = 0; i <= num_bytes_tx; i++ )
			                    {
	                             puts( "\nbyte");//printf( "\nbyte");
			                     UartSendByte( i );	
			                     putchar( '=' );
				                 value = XBYTE[MEM_DATA_L];
			                     UartSendByte( value );
				                 putchar( '\n' );
			                    }
	
	                        puts( "\nloading RAM rx...\n");
			                XBYTE[MEM_DEFINE] = 0x00;
			                XBYTE[MEM_ADDR] = 0x00;//recovered bigining address
		                    XBYTE[ RS485_CTRL ] = 0x6b;
			                XBYTE[MEM_DEFINE] = MEM_RS485;
			                XBYTE[START_TEST_RS485] = 0x00;


				         	break;
				       default:
		            		bADCdebug = 0;
		         			puts( s_exit_adc_debug );
		         			break;				     		       						         					
				       }//END SWITCH 
			  	break;
//#endif
			    
			case 'A':
				bADCdebug = 1;				
				puts( "\nadc debug...\n" );												
				while( bADCdebug )
	            {
	         	putchar( 'A' );putchar( '-' );putchar( '>' );         
	    	    ch = getchar();
	    	    ch = toupper( ch );				           
	    	    switch( ch )
				       {
						            
				        case 'S':	//auto focusing stop
				        	puts( "\nADC auto stop...\n" );
		                    ADCCON1 = 0xBC;
			                ADCCON2 = ADCS_EXT_T_SENSE;                        
				         	break;						             
						             
				        case 'R':	//auto focusing run
			                puts( "\nADC auto run...\n" );
			                bADCdebug = 0;
			                ADCCON1 = ADC_POWER_UP;	
		                    ADCCON2 = ADCS_EXT_T_SENSE; 
				         	break;		
				         			   						           			             
				        case '1':	//one step auto focusing
			                putchar('\n');
			                expo_data = ADCCON1;		                
			                ch        = ADCCON2;
			                
				            num_adc_ch = NORD300;  		                
		                    GetAndPrintAdcOne(ADCS_SINGL_CONV_1_5);
		                    fAutoFocus();
	                    
				            num_adc_ch = NORD75;                     
		                    GetAndPrintAdcOne(ADCS_SINGL_CONV_6);
		                    fAutoFocus();
                    
				            num_adc_ch = NORD_TEMP;                     
		                    GetAndPrintAdcOne(ADCS_SINGL_CONV_EXT_T_SENSE);
		                    fAutoFocus(); 
	                    
		                    ADCCON1 = expo_data;                      
		                    ADCCON2 = ch;                                          
				         	break;	
				         			   	
			   	      case 'N'://PARAM_FOCUS_NARROW  	300	 1.5     01        	
		                  putchar('\n');
				          num_adc_ch = NORD300;  
				          XBYTE[NUM_CHANNEL] = NORD300;   
				          XBYTE[VIDEO_MUX]   = NORD300;			                
		                  expo_data = ADCCON1;		                
		                  ch        = ADCCON2;
		                  GetAndPrintAdcOne(ADCS_SINGL_CONV_1_5);
		                  fAutoFocus();
		                  ADCCON1 = expo_data;                                          
		                  ADCCON2 = ch;  
				          break;	
	 			
			   	      case 'M'://PARAM_FOCUS_MIDDLE		75   6       02 
		                  putchar('\n');
				          num_adc_ch = NORD75;  
				          XBYTE[NUM_CHANNEL] = NORD75;   
				          XBYTE[VIDEO_MUX]   = NORD75;			                
		                  expo_data = ADCCON1;		                
		                  ch        = ADCCON2;
		                  GetAndPrintAdcOne(ADCS_SINGL_CONV_6);
		                  fAutoFocus();
		                  ADCCON1 = expo_data;                                          
		                  ADCCON2 = ch;  
				          break;
					                      					                               			   						             
				        case 'E':	//exit adc debug
				            		 bADCdebug = 0;
				         			   puts( s_exit_adc_debug );
				         			   break;			
				         			   	           	
				        case '4':	//
									       puts( s_adc_channel4 );
				            		 num_adc_ch = NORD300; 										       
				         			   GetAndPrintAdcOne(ADCS_SINGL_CONV_1_5);	                         			            	
				         			   break;
				         			   
				        case '3':	//
	     		         		   puts( s_adc_channel3 );
				            		 num_adc_ch = NORD75; 	     		         		   
				         			   GetAndPrintAdcOne(ADCS_SINGL_CONV_6);	                         			            	
				         	       break;
				         	       
				            case '5':	//
				         			   puts( s_adc_channel5 );
				            		 num_adc_ch = NORD_TEMP; 				         			   
				         			   GetAndPrintAdcOne(ADCS_SINGL_CONV_EXT_T_SENSE);			            		         								    
				         	       break;	
				         	       
				         	  default:
				            		 bADCdebug = 0;
				         			   puts( s_exit_adc_debug );
				         			   break;				     		       						         					
				           }//end select mode ADC	
	    			 }//exit debug adc			
				break;
														
			case 'W':
						if( UartGetByte( &addr_l ) )
						{
							if( UartGetByte( &value ) )
							{
								XBYTE[*paddr] = value;
							}
							else puts( s_debug_error );
						}
						else puts( s_debug_error );
						break;

			case 'R':	/* read command */
						if( UartGetByte( &addr_l ) )
						{
							value = XBYTE[*paddr];
							putchar('\n');
							UartSendByte( value );
						}
						else puts( s_debug_error );
						break;
						
		  case 'P'://     
				    ch = getchar();
				    ch = toupper( ch );
				    switch( ch )
				          {
			   	         case '?':      	
                       puts(s_help_parameter);	
					             break;
					                      				          	
			   	         case 'N'://PARAM_FOCUS_NARROW  	300	 1.5     01        	
                                num_adc_ch = NORD300;           
							                  XBYTE[NUM_CHANNEL] = NORD300;   
				                  			XBYTE[VIDEO_MUX]   = NORD300;							                                                                    
					                      addr_l = A_FOCUS_NARROW;
					                      cAdcModeCS = ADCS_SINGL_CONV_1_5;	
					                      break;

			   	         case 'M'://PARAM_FOCUS_MIDDLE		75   6     02       	
                                num_adc_ch = NORD75;  
							                  XBYTE[NUM_CHANNEL] = NORD75;   
				                  			XBYTE[VIDEO_MUX]   = NORD75;	                                                              
					                      addr_l = A_FOCUS_MIDDLE;
					                      cAdcModeCS = ADCS_SINGL_CONV_6;					                      
					                      break;       

			   	         case 'T'://PARAM_TEMPERATURE		               03       	
                                num_adc_ch = NORD_TEMP;  	                                                              
					                      cAdcModeCS = ADCS_SINGL_CONV_EXT_T_SENSE;					                      
					                      break;

			             case 'F'://FRAME
					                      ptr_parametres = &frame[ num_ch ];
                                parameter_min = FRAME_MIN;
                                parameter_max = FRAME_MAX;
					                      addr_l = 0x28;//
					                      break;
					                      					                      					                                       					                     				                      
			             case 'G'://GAIN
				           case 'S'://SHUTTER
				           case 'E'://EXPOSURE					                      
                        break;
                                
					         default:
            		                puts( s_debug_error );
                                break;            		                
            		  }//end  F  
				    switch( ch )
				       {
			   	      case 'F'://FRAME
			   	          ch = getchar();
			   	          ch = toupper( ch );
			   	          while(( ch == '-')||( ch == '+'))
			   	         	   {
			   	         	    switch( ch )
			   	         	       {
				                    case '-':
						                    if( *ptr_parametres > parameter_min )
						                       {
						                                      if( *ptr_parametres <= parameter_max )
						                                         *ptr_parametres =*ptr_parametres - 1;
						                                      else
						                                         *ptr_parametres = parameter_max;
						                       }
						                    else *ptr_parametres = parameter_min;
						                    UartSendByte( *((char*)ptr_parametres) );
						                    XBYTE[*paddr] = *((char*)ptr_parametres);
						                    ch = getchar();
						                    ch = toupper( ch );
						                    putchar('\n');						                                  
						                    break;//END CASE '-'
				                    case '+':	
						                    if( *ptr_parametres < parameter_max ) 
						                       {
						                        if( *ptr_parametres >= parameter_min )
						                            *ptr_parametres =*ptr_parametres + 1;
						                        else
						                            *ptr_parametres = parameter_min;
						                       }
						                    else//
						                       {
						                        *ptr_parametres = parameter_max; 
						                       }
						                    UartSendByte( *((char*)ptr_parametres) );
						                    XBYTE[*paddr] = *((char*)ptr_parametres);
						                    ch = getchar();
						                    ch = toupper( ch );
						                    putchar('\n');						                                  
						                    break;//END CASE '+'
						                default:
						                    puts( s_debug_error );
						                }//end switch +/-
			   	             }//end while
			   	          break;//F	
			   	         	         			          	
			   	         case 'T':  
			   	         	         bADCdebug = 1;			   	         	
			   	         	         while( ch == 'T')
			   	         	             {
			   	         	              GetAndPrintAdcOne(cAdcModeCS);		
			   	         	              ch = getchar();
			   	         	              ch = toupper( ch );			   	         	              		   	         	             	
			   	         	             }			   	         	
			   	         	         break;
			   	         	         
			   	         case 'M'://FOCUS
			   	         case 'N'://FOCUS	
			   	         	         bADCdebug = 1;		   	         	
				                     ch = getchar();
				                     ch = toupper( ch );
                             while(
                                   ( ch == '-')||
                                   ( ch == '+')||
                                   ( ch == '/')||
                                   ( ch == '*')||
                                   ( ch == '=')||
                                   ( ch == 'F')||
                                   ( ch == 'N')                                   
                                   )
                                  {
				                           switch( ch )
				                              {			                                                   
				                                          case '-':
					                                                 XBYTE[*paddr] = FOC_FAR;	
				                                                   putchar('F');putchar('a');putchar('r');putchar(' ');					                                                 
				                                                   GetAndPrintAdcOne(cAdcModeCS);
				                                                   DelayStopMotor(SPEED_FOCUS_1);				                                                   
				                                                   XBYTE[*paddr] = FOC_STOP;					                                                   					                                                   				                                                   				                                                   	                                                   	                                                   
				                                                   ch = getchar();
				                                                   ch = toupper( ch );
				                                                   break;//END CASE '-'

				                                          case '+':	
				                                                   XBYTE[*paddr] = FOC_NEAR;		
				                                                   putchar('N');putchar('e');putchar('a');putchar('r');putchar(' ');				                                                   		                                                   
				                                                   GetAndPrintAdcOne(cAdcModeCS);	
				                                                   DelayStopMotor(SPEED_FOCUS_1);				                                                   
				                                                   XBYTE[*paddr] = FOC_STOP;					                                                   						                                                   				                                                   				                                                   	                                                   	                                                   
				                                                   ch = getchar();
				                                                   ch = toupper( ch );
				                                                   break;//END CASE '+'
				                                                   
				                                          case '/':
					                                                 XBYTE[*paddr] = FOC_Q_FAR;	
				                                                   putchar('F');putchar('a');putchar('r');putchar('Q');putchar(' ');						                                                 
				                                                   GetAndPrintAdcOne(cAdcModeCS);
				                                                   DelayStopMotor(SPEED_FOCUS_2);				                                                   
				                                                   XBYTE[*paddr] = FOC_STOP;					                                                   					                                                   				                                                   				                                                   	                                                   	                                                   
				                                                   ch = getchar();
				                                                   ch = toupper( ch );
				                                                   break;//END CASE '/'
				                                                   
				                                          case '*':	
				                                                   XBYTE[*paddr] = FOC_Q_NEAR;	
				                                                   putchar('N');putchar('e');putchar('a');putchar('r');putchar('Q');putchar(' ');				                                                   			                                                   
				                                                   GetAndPrintAdcOne(cAdcModeCS);	
				                                                   DelayStopMotor(SPEED_FOCUS_2);				                                                   
				                                                   XBYTE[*paddr] = FOC_STOP;					                                                   						                                                   				                                                   				                                                   	                                                   	                                                   
				                                                   ch = getchar();
				                                                   ch = toupper( ch );
				                                                   break;//END CASE '+'
				                                                   
				                                          case '=':	
				                                                   XBYTE[*paddr] = FOC_Q_STOP;	
				                                                   putchar('R');putchar('e');putchar('q');putchar('u');putchar('e');putchar('s');putchar('t');putchar(' ');			                                                   
				                                                   GetAndPrintAdcOne(cAdcModeCS);	
				                                                   ch = getchar();
				                                                   ch = toupper( ch );
				                                                   break;//END CASE '+'	
				                                                   
				                                          case 'F':
					                                                 XBYTE[*paddr] = FOC_Q_FAR;	
				                                                   putchar('F');putchar('a');putchar('r');putchar('C');putchar('o');putchar('n');putchar('t');putchar(' ');						                                                 
				                                                   GetAndPrintAdcOne(cAdcModeCS);				                                                   				                                                   					                                                   				                                                   				                                                   	                                                   	                                                   
				                                                   ch = getchar();
				                                                   ch = toupper( ch );
				                                                   break;//END CASE '-'

				                                          case 'N':	
				                                                   XBYTE[*paddr] = FOC_Q_NEAR;	
				                                                   putchar('N');putchar('e');putchar('a');putchar('r');putchar('C');putchar('o');putchar('n');putchar('t');putchar(' ');				                                                   			                                                   
				                                                   GetAndPrintAdcOne(cAdcModeCS);						                                                   						                                                   				                                                   				                                                   	                                                   	                                                   
				                                                   ch = getchar();
				                                                   ch = toupper( ch );
				                                                   break;//END CASE '+'	
				                                                   			                                                   			                                                   
						                                   default:
						                                           puts( s_debug_error );
				                                               break;//				                                           
				                               }//end switch +/-
                                   }//end while
			                       break;//F 

			   	         case 'S'://SHUTTER
				                     ch = getchar();
				                     ch = toupper( ch );
				                     switch( ch )
				                           {
			   	                          case 'R'://PARAM_SHUT_ROUGH
				                                 ch = getchar();
				                                 ch = toupper( ch );
                                         while(( ch == '-')||( ch == '+'))
                                             {
				                                      switch( ch )
				                                           {
				                                            case '-':
			                                                  if( shutter[ num_ch ] > 0 ) shutter[ num_ch ]--;
				                                                PrintAndSetShutter();
				                                                ch = getchar();
				                                                ch = toupper( ch );
	                                                      putchar('\n');
				                                                break;//END CASE '-'

				                                            case '+':	
				                                                if( shutter[ num_ch ] < SHUTTER_MAX ) shutter[ num_ch ]++;
				                                                PrintAndSetShutter();
				                                                ch = getchar();
				                                                ch = toupper( ch );
	                                                      putchar('\n');
				                                                break;//END CASE '+'
						                                       default:
						                                            puts( s_debug_error );
				                                                break;						                                            
				                                           }//end switch +/-
                                             }//end while
					                               break;//PARAM_SHUT_ROUGH

						                        default:
						                             puts( s_debug_error );
					                               break;						                             
				                           }
				                     break;//S
				                     
			   	             case 'G'://GAIN
				                     ch = getchar();
				                     ch = toupper( ch );
                             while(( ch == '-')||( ch == '+'))
                                   {
				                            switch( ch )
				                                  {
				                                   case '-':
				                                       if( gain[ num_ch ] > 0 ) gain[ num_ch ]--;
				                                       PrintAndSetGain();
				                                       ch = getchar();
				                                       ch = toupper( ch );
	                                             putchar('\n');
				                                       break;//END CASE '-'

				                                   case '+':	
			                                         if( gain[ num_ch ] < GAIN_MAX ) gain[ num_ch ]++;
				                                       PrintAndSetGain();
				                                       ch = getchar();
				                                       ch = toupper( ch );
	                                             putchar('\n');
				                                       break;//END CASE '+'
				                                       
						                               default:
						                                   puts( s_debug_error );
				                                       break;//END CASE '+'						                                           
				                                  }//end switch +/-
                                   }//end while
				                     break;//G
				                     
			   	             case 'E'://exposure
				                     ch = getchar();
				                     ch = toupper( ch );
                             while(( ch == '-')||( ch == '+'))
                                   {
				                           switch( ch )
				                                 {
				                                          case '-':
			                                                     if( index[ num_ch ] > 0 ) index[ num_ch ]--;
				                                                   PrintAndSetParam();
				                                                   ch = getchar();
				                                                   ch = toupper( ch );
	                                                               putchar('\n');
				                                                   break;//END CASE '-'

				                                          case '+':	
				                                                   if( index[ num_ch ] < INDEX_MAX ) index[ num_ch ]++;
				                                                   PrintAndSetParam();
				                                                   ch = getchar();
				                                                   ch = toupper( ch );
	                                                               putchar('\n');
				                                                   break;//END CASE '+'
						                                   default:
						                                           puts( s_debug_error );
				                                               break;//				                                           
				                                 }//end switch +/-
                                   }//end while
				                     break;//E

			                 default :
                             puts( s_debug_error );
				                     break;//                             
                  }
						break;//parameter
						    						
			case 'C':
				ch = getchar();
				ch = toupper( ch );
				switch( ch )
				{
					case 'H':	/* write command */
						if( UartGetByte( &value ) )
						{
							XBYTE[NUM_CHANNEL] = value;
							if( value < 3 ) num_ch = value;
						}
						else puts( s_debug_error );
						break;

					case 'V':	/* write command */
						if( UartGetByte( &value ) )
						{
							XBYTE[VIDEO_MUX] = value;
						}
						else puts( s_debug_error );
						break;

					case 'T':	/* write command */
						  if( UartGetByte( &value ) )
						      {
							    //                   0000 0011
							    //                   1010 0000
		           				switch( value )
		               				{
		               				case 0:	/* write command */
		             		     	    XBYTE[VIDEO_MUX] =  VideoMux;
		               						break;
		               						
		              				case 1:	/* write command */
				     	                XBYTE[VIDEO_MUX] =  VideoMux | 0x80; 
		               						break;
		               						
		               				case 2:	/* write command */
		             		     	    XBYTE[VIDEO_MUX] =  VideoMux | 0x20;
		               						break;
		               						
		              				case 3:	/* write command */
				     	                XBYTE[VIDEO_MUX] =  VideoMux | 0xA0; 
		               						break;
		               						               						
		               				default:
		               						puts( s_debug_error );               						
		    						      }
		                              }
						  else puts( s_debug_error );
						  break;

					default:
						puts( s_debug_error );
			           }
				break;

			case 'S':// set
				ch = getchar();
				ch = toupper( ch );
				switch( ch )
				{
					case '?':	// set  
                        puts(s_help_settings);	
					    break;	
					    			    	
					case 'D':	// set default focuses const 
			              EADRH = 0;
			              EADRL = 0;
			              ECON = 1;
			              EDATA1 = INFINITY_15;
			              EDATA2 = FOCUS_MIN_15;
			              EDATA3 = FOCUS_MAX_15;
			              EDATA4 = ANALOG_ERROR;                                          
			              ECON = 5;
			              ECON = 2;
			              ECON = 4;
			              
			              EADRH = 0;
			              EADRL = 1;
			              ECON = 1;
			              EDATA1 = INFINITY_6;
			              EDATA2 = FOCUS_MIN_6;
			              EDATA3 = FOCUS_MAX_6;
			              EDATA4 = DUMMY_LOCAL;                                          
			              ECON = 5;
			              ECON = 2;
			              ECON = 4;
			              
			              EADRH = 0;
			              EADRL = 2;
			              ECON = 1;
			              EDATA1 = 0x10;
			              EDATA2 = 0x76;
			              EDATA3 = 0x05;
			              EDATA4 = DUMMY_LOCAL;                                          
			              ECON = 5;
			              ECON = 2;
			              ECON = 4;
			              ucInfinity_1_5 = INFINITY_15;
			              ucInfinity_6   = INFINITY_6;
			              ucAnalogError  = ANALOG_ERROR;  
			              
			              EADRH = 0;
			              EADRL = 3;
			              ECON = 1;
			              EDATA1 = 0x80;
			              EDATA2 = 0x00;
			              EDATA3 = 0x00;
			              EDATA4 = 0x00;                                          
			              ECON = 5;
			              ECON = 2;
			              ECON = 4;
							      ucRcapH = EDATA1; 				
							      ucRcapL = EDATA2;  
			
			              EADRH = 0;
			              EADRL = 4;
			              ECON = 1;
			              EDATA1 = 0x03;
			              EDATA2 = 0x08;
			              EDATA3 = 0x20;
			              EDATA4 = 0x12;                                          
			              ECON = 5;
			              ECON = 2;
			              ECON = 4;                                    	
					    break;	
					    
					case 'C':	// set  channel
						
						  if( UartGetByte( &value ) )
						      {
						     	if( value < 3 )
						    	    {
						    	   	 XBYTE[NUM_CHANNEL] = value;
						    	     XBYTE[VIDEO_MUX]   = value;
							         num_ch = value;
						          }
						      else puts( "\nThere are three channals: #00, #01, #02" );						         
						      }
						  else puts( s_debug_error );
					    break;
					    										
					case 'A':	// set All 
						  ch = getchar();
				      ch = toupper( ch );
				      switch( ch )
				        {
				         case 'O':	// set All Open Filter  
                         XBYTE[VIDEO_MUX]     = NORD19;	
	                       XBYTE[ NUM_CHANNEL ] = NORD19;                         
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_OUT;
  	                     fTimer( SEC, 0x04 );
  	                     
                         XBYTE[VIDEO_MUX] = NORD300;	
	                       XBYTE[ NUM_CHANNEL ] = NORD300;                          
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_OUT;
  	                     fTimer( SEC, 0x04 );
  	                     
                         XBYTE[VIDEO_MUX]     = NORD75;	
	                       XBYTE[ NUM_CHANNEL ] = NORD75;                          
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_OUT;
  	                     fTimer( SEC, 0x04 );
  	                     
                         ch = XBYTE[ FILTER ];
                         putchar( '\n' );
                         putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( 's' ); putchar( ':' );
                         UartSendByte( ch );				         	
						         break;
						         
				         case 'C':	// set Middle Close Filter  
                         XBYTE[VIDEO_MUX]     = NORD19;	
	                       XBYTE[ NUM_CHANNEL ] = NORD19;                         
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_IN;
  	                     fTimer( SEC, 0x04 );
  	                     
                         XBYTE[VIDEO_MUX] = NORD300;	
	                       XBYTE[ NUM_CHANNEL ] = NORD300;                          
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_IN;
  	                     fTimer( SEC, 0x04 );
  	                     
                         XBYTE[VIDEO_MUX]     = NORD75;	
	                       XBYTE[ NUM_CHANNEL ] = NORD75;                          
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_IN;
  	                     fTimer( SEC, 0x04 );
  	                     
                         ch = XBYTE[ FILTER ];
                         putchar( '\n' );
                         putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( 's' ); putchar( ':' );
                         UartSendByte( ch );			         	
						         break;	
						         
				         default:
						         puts( s_debug_error );				        	
						         break;
						    }
						  break;
											
					case 'N':	// set Narrow 
						ch = getchar();
				    ch = toupper( ch );
				    switch( ch )
				        {
				         case 'O':	// set Narrow Open Filter  
                         XBYTE[VIDEO_MUX] = NORD300;	
	                       XBYTE[ NUM_CHANNEL ] = NORD300;                          
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_OUT;
  	                     fTimer( SEC, 0x04 );
                         ch = XBYTE[ FILTER ];
                         putchar( '\n' );
                         putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( 's' ); UartSendByte( NORD300 );putchar( ':' );
                         UartSendByte( ch );				         	
						         break;
						         
				         case 'C':	// set Narrow Close Filter  
                         XBYTE[VIDEO_MUX] = NORD300;	
	                       XBYTE[ NUM_CHANNEL ] = NORD300;                                                   
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_IN;
  	                     fTimer( SEC, 0x04 );
                         ch = XBYTE[ FILTER ];
                         putchar( '\n' );
                         putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( 's' ); UartSendByte( NORD300 );putchar( ':' );
                         UartSendByte( ch );				         	
						         break;	
						         				        	
				         case 'I':	// set Narrow Infinity  
						         if( UartGetByte( &value ) )
						            {
                         EADRH = 0;
                         EADRL = 0;
                         ECON = 1;
                         EDATA1 = value;
                         ECON = 5;
                         ECON = 2;
                         ECON = 4;
						             }
						         else puts( s_debug_error );				         	
						         break;
						         
				         case 'E':	// set Narrow Error
						         if( UartGetByte( &value ) )
						            {
                         EADRH = 0;
                         EADRL = 0;
                         ECON = 1;
                         EDATA4 = value;
                         ECON = 5;
                         ECON = 2;
                         ECON = 4;
                         ucAnalogError = value;
						             }
						         else puts( s_debug_error );				         	
						         break;
						         
				         default:
						         puts( s_debug_error );				        	
						         break;
						    }
						break;

			         case '#':	// set Number ME-108
		                EADRH = 0x00;
		                EADRL = 0x02;
                        for( i = 1; i < 4; i++ )
                           {
 		                    if( UartGetByte( &value ) )
				              {
     	                       switch( i )
	                               {
	                                case 1:
			                            EDATA1 = value;
			                            break;
			                        case 2:
			                            EDATA2 = value;
			                            break;
			                        case 3:
			                            EDATA3 = value;
			                            break;
	                                default:
	                                    break;
	                               }
							    }
 	             	        else puts( s_debug_error );
					       }
						ECON = 5;
						ECON = 2;
                        ECON = 4;				         	
				        break;

			         case 'L':	// set loaded date
		                EADRH = 0x00;
		                EADRL = 0x04;
                        for( i = 1; i < 5; i++ )
                           {
 		                    if( UartGetByte( &value ) )
				              {
     	                       switch( i )
	                               {
	                                case 1:
			                            EDATA1 = value;
			                            break;
			                        case 2:
			                            EDATA2 = value;
			                            break;
			                        case 3:
			                            EDATA3 = value;
			                            break;
			                        case 4:
			                            EDATA4 = value;
			                            break;
	                                default:
	                                    break;
	                               }
							    }
 	             	        else puts( s_debug_error );
					       }
						ECON = 5;
						ECON = 2;
                        ECON = 4;				         	
				        break;
				        
			    case 'T':	// set speed convertion fFocusParamInit
		          EADRH = 0x00;
		          EADRL = 0x03;
              for( i = 1; i < 3; i++ )
                  {
 		               if( UartGetByte( &value ) )
				               {
     	                  switch( i )
	                         {
	                          case 1:
			                          EDATA1 = value;
			                          break;
			                                
			                      case 2:
			                          EDATA2 = value;
			                          break;

	                          default:
	                              break;
	                         }
						           }
 	             	   else puts( s_debug_error );
					        }
						  ECON = 5;
						  ECON = 2;
              ECON = 4;				         	
				      break;
					
					case 'M':	// set Middle 
						ch = getchar();
				    ch = toupper( ch );						    
				    switch( ch )
				        {
				         case 'O':	// set Middle Open Filter  
                         XBYTE[VIDEO_MUX] = NORD75;	
	                       XBYTE[ NUM_CHANNEL ] = NORD75;                          
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_OUT;
  	                     fTimer( SEC, 0x04 );
                         ch = XBYTE[ FILTER ];
                         putchar( '\n' );
                         putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( 's' ); UartSendByte( NORD75 );putchar( ':' );
                         UartSendByte( ch );				         	
						         break;
						         
				         case 'C':	// set Middle Close Filter  
                         XBYTE[VIDEO_MUX] = NORD75;	
	                       XBYTE[ NUM_CHANNEL ] = NORD75;                          
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_IN;
  	                     fTimer( SEC, 0x04 );
                         ch = XBYTE[ FILTER ];
                         putchar( '\n' );
                         putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( 's' ); UartSendByte( NORD75 );putchar( ':' );
                         UartSendByte( ch );				         	
						         break;		
						         				         				        	
				         case 'I':	// set Middle Infinity  
						         if( UartGetByte( &value ) )
						            {
                         EADRH = 0;
                         EADRL = 1;
                         ECON = 1;
                         EDATA1 = value;
                         ECON = 5;
                         ECON = 2;
                         ECON = 4;
						             }
						         else puts( s_debug_error );				         	
						         break;
						         
				         case 'E':	// set Middle Error
						         if( UartGetByte( &value ) )
						            {
                         EADRH = 0;
                         EADRL = 1;
                         ECON = 1;
                         EDATA4 = value;
                         ECON = 5;
                         ECON = 2;
                         ECON = 4;
                         ucAnalogError = value;                         
						             }
						         else puts( s_debug_error );				         	
						         break;
						         
				         default:
						         puts( s_debug_error );				        	
						         break;
						    }
						break;
						
					case 'W':	// set Wide 
						ch = getchar();
				    ch = toupper( ch );
				    switch( ch )
				        {
				         case 'O':	// set Wide Open Filter  
                         XBYTE[VIDEO_MUX] = NORD19;	
	                       XBYTE[ NUM_CHANNEL ] = NORD19;                         
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_OUT;
  	                     fTimer( SEC, 0x04 );
                         ch = XBYTE[ FILTER ];
                         putchar( '\n' );
                         putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( 's' ); UartSendByte( NORD19 );putchar( ':' );
                         UartSendByte( ch );				         	
						         break;
						         
				         case 'C':	// set Middle Close Filter  
                         XBYTE[VIDEO_MUX] = NORD19;	
	                       XBYTE[ NUM_CHANNEL ] = NORD19;                          
  	                     fTimer( SEC, 0x04 );                         			         	 		
                         XBYTE[ FILTER ]      = F_IN;
  	                     fTimer( SEC, 0x04 );
                         ch = XBYTE[ FILTER ];
                         putchar( '\n' );
                         putchar( 'F' );putchar( 'i' );putchar( 'l' );putchar( 't' );putchar( 'e' );putchar( 'r' );putchar( 's' ); UartSendByte( NORD19 );putchar( ':' );
                         UartSendByte( ch );				         	
						         break;	
						         
				         default:
						         puts( s_debug_error );				        	
						         break;
						    }
						  break;
												
					case 'F':	// set frame limit
						if( UartGetByte( &frame_limit ) )
						    {
							CalcExpDelta();
						    }
						else
						    {
							   puts( s_debug_error );
						    }
						break;
						
					case 'G':	// set gain
						ch = getchar();
				    ch = toupper( ch );
				    switch( ch )
				        {
					       case 'V':	// set gain value
    				         if( UartGetByte( (char*)&gain[ num_ch ] ) )
					  				            {
					  				             if( UartGetByte( ((char*)&gain[ num_ch ])+1 ) )
					  				                {
					  				                 if( gain[ num_ch ] >= GAIN_DEFAULT )
					  				                 	  {
					  				                 	   if( gain[ num_ch ] <= GAIN_MAX ) 
					  				                 	   	   {
								  				                   PrintAndSetGain();
					  				                 	   	   	putchar( '\n' );		
					  				                 	   	   }							  				                   
					  				                     else
					  				                         {
                                              gain[ num_ch ] = GAIN_MAX;
                                             	puts( "\ngain range: 432 to 1023!" );
								  				                    PrintAndSetGain();
					  				                 	   	   	putchar( '\n' );									  				                    					  				                    	
					  				                         }		
					  				                    }
					  				                 else
					  				                    {
					  				                     gain[ num_ch ] = 0;
                                         puts( "\ngain range: 432 to 1023!" );					  				                     
								  				               PrintAndSetGain();		
					  				                 	   putchar( '\n' );									  				               			  				                      
					  				                    }					  				                     			  				                    	
					  				                }
					  				             else
					  				                {
					  				                 puts( s_debug_error );
					  				                }
					  				            }						  				                
					  				         else
					  				            {
					  				           	 puts( s_debug_error );
					  				            }					  				                
				             break;	
					       default:
					           puts( s_debug_error );
					           break;						              			                 
					      }					  				     	   			        							
					    break;						              			                 

					case 'E':	// set exposure integral
						ch = getchar();
				    ch = toupper( ch );
				    switch( ch )
				        {
					       case 'V':	// set exposure value
    				         if( UartGetByte( (char*)&index[ num_ch ] ) )
					  				            {
					  				             if( UartGetByte( ((char*)&index[ num_ch ])+1 ) )
					  				                {
					  				                 if( index[ num_ch ] >= 0 )
					  				                 	  {
					  				                 	   if( index[ num_ch ] <= INDEX_MAX )
					  				                 	   	   { 
								  				                   PrintAndSetParam();
					  				                 	   	   putchar( '\n' );			
					  				                 	   	   }						  				                   
					  				                     else
					  				                         {
                                              index[ num_ch ] = INDEX_MAX;
                                             	puts( "\nexposure range: 0 to 511!" );
								  				                    PrintAndSetParam();	
					  				                 	   	   	putchar( '\n' );									  				                    				  				                    	
					  				                         }		
					  				                    }
					  				                 else
					  				                    {
					  				                     index[ num_ch ] = 0;
                                         puts( "\nexposure range: 0 to 511!" );					  				                     
								  				               PrintAndSetParam();
					  				                 	   putchar( '\n' );									  				               					  				                      
					  				                    }					  				                     			  				                    	
					  				                }
					  				             else
					  				                {
					  				                 puts( s_debug_error );
					  				                }
					  				            }						  				                
					  				         else
					  				            {
					  				           	 puts( s_debug_error );
					  				            }					  				                
				             break;	
					       default:
					           puts( s_debug_error );
					           break;						              			                 
					      }					  				     	   			        							
					    break;
					    											
					case 'S':	// shutter  
						ch = getchar();
				    ch = toupper( ch );
				    switch( ch )
				        {
					       case 'R':	// set shutter rough
										ch = getchar();
										ch = toupper( ch );
										switch( ch )
										    {	
					               case 'V':	// set shutter rough value
					  				         if( UartGetByte( (char*)&shutter[ num_ch ] ) )
					  				            {
					  				             if( UartGetByte( ((char*)&shutter[ num_ch ])+1 ) )
					  				                {
					  				                 if( shutter[ num_ch ] >= 0 )
					  				                 	  {
					  				                 	   if( shutter[ num_ch ] <= SHUTTER_MAX ) 
					  				                 	   	   {
								  				                    PrintAndSetShutter();
					  				                 	   	   	putchar( '\n' );								  				                    
								  				                   }
					  				                     else
					  				                         {
                                              shutter[ num_ch ] = SHUTTER_MAX;
                                             	puts( "\nshutter range: 0 to 620!" );
								  				                    PrintAndSetShutter();
					  				                 	   	   	putchar( '\n' );									  				                    					  				                    	
					  				                         }		
					  				                    }
					  				                 else
					  				                    {
					  				                     shutter[ num_ch ] = 0;
                                         puts( "\nshutter range: 0 to 620!" );					  				                     
								  				               PrintAndSetShutter();		
					  				                 	   putchar( '\n' );									  				               			  				                      
					  				                    }					  				                     			  				                    	
					  				                }
					  				             else
					  				                {
					  				                 puts( s_debug_error );
					  				                }
					  				            }						  				                
					  				         else
					  				            {
					  				           	 puts( s_debug_error );
					  				            }					  				                
					                 	 break;	
					                 	 
					               case 'L':	
					  				         if( UartGetByte( (char*)&shutter_limit ) )
					  				            {
					  				             if( UartGetByte( ((char*)&shutter_limit)+1 ) )
					  				                {
					  				                 CalcExpDeltaFine();
					  				                 //UartSendWord( shutter_limit);
					  				                }
					  				             else
					  				                {
					  				                 shutter_limit = SHUTTER_MAX;
					  				                }
					  				            }
					  				         else
					  				          	{
					  				           	 puts( s_debug_error );
					  				          	}
					  								 break;						               	
					                 	 					                 	 			               										    		
										     default:
										         puts( s_debug_error );
										         break;
										    }										    				       	   
					  				break; 
					  				 
					       default:
					           puts( s_debug_error );
					  				 break; 
					      }					  				     	   			        							
						break;

					default:
						puts( s_debug_error );
					  break; 						
				}
				break;


			case 'H':	/* Histogram */
				ch = getchar();
				ch = toupper( ch );
				switch( ch )
				{
					case 'M':	/* make histogram command */
						XBYTE[HIST_CONTROL] = HIST_REQ;	/* ����� ���⮣ࠬ�� */
						bHistRdy = 0;
						si = 0;
						while( --si!=0 )
						{
							status = XBYTE[ HIST_CONTROL ];
							if( bHistRdy )
							{
								//bHistRdy = 1;
								break;
								}
							}
						if( !bHistRdy )	puts( s_hist_error );
						break;
					case 'D':	/* read command */
						XBYTE[MEM_DEFINE] = MEM_HIST;	/* ����� �ਥ���� - ���⮣ࠬ�� */
						XBYTE[MEM_ADDR] = 255;			/* ���㫨�� ���� ���⮣ࠬ�� */
						puts("\r\n");
						i = 0;
						do
						{
							value = XBYTE[MEM_DATA_H];
							UartSendByte( value );
							value = XBYTE[MEM_DATA_L];
							UartSendByte( value );
							putchar(' ');
						}
						while( --i!=0 );
						break;

					default:
						puts( s_debug_error );
				}
				break;
			default:
				puts( s_debug_error );
		}
	}
	//if( !bFilterDis ) EX1 = 1;		/* ������� ���뢠��� �� INT1 */
	XBYTE[ NUM_CHANNEL ] = 0;
	XBYTE[ HIST_CONTROL ] = HIST_REQ;	/* ����� ���⮣ࠬ�� */
	XBYTE[ NUM_CHANNEL ] = 1;
	XBYTE[ HIST_CONTROL ] = HIST_REQ;	/* ����� ���⮣ࠬ�� */
	XBYTE[ NUM_CHANNEL ] = 2;
	XBYTE[ HIST_CONTROL ] = HIST_REQ;	/* ����� ���⮣ࠬ�� */
}
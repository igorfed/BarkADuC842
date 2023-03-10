#include <ADI\ADUC842.H>   /*  Use 8052&ADuC842 predefined symbols     */

#include <stdio.h>	  /* prototype declarations for I/O functions */
#include <ctype.h>
#include <absacc.h>
#include <intrins.h>
#include <math.h>

#include "uart.h"
#include "bark.h"
#include "funct.h"
#include "protocol.h"
#include "adc.h"
#include "focus.h"

//#define DEBUG_BUILD
#define RS485_DEBUG_BUILD
#define BUILD_WITH_CROSS

void PrintAndSetParam();
//void GetParam();

extern char code s_exit_adc_debug[];
extern char code s_copyright[];
extern char code s_debug[];
extern char code s_hist_error[];
extern char code s_ram_error[];
extern char code s_int_rs485[];

//unsigned short array_index;
signed short index_tmp;
signed char new_change;//, cur_change, old_change[3] = {0};

extern short idata shutter[];
extern short idata gain[];
extern unsigned char idata frame[];
extern short idata index[];
unsigned char data frame_limit = FRAME_LIMIT_DEF;

extern struct exp code exp_table[];
extern signed char code change_table[];
extern signed char code change_table_av[];
//extern struct st_thres data thres;
extern unsigned char data average;
extern unsigned char data thres_right;
unsigned char thres_old1[3] = {128,128,128}, thres_old2[3] = {128,128,128};
//extern unsigned long lhistsum;
extern unsigned short sum;
extern unsigned long lsum;
//extern unsigned long l_sum;
//unsigned char DUMMY00;
unsigned char DUMMY01;
unsigned char DUMMY02;
//define SUM_OF (*(((unsigned char*)&lhistsum)+1))
#define SUM_OF (*((unsigned char*)&lsum))
//#define L_SUM (*((unsigned char*)&l_sum))
bit bAverage = 1;
bit bFilterDis    = 1;
bit bADCdebug     = 0;
bit bAdcAutoDebug = 0;
bit bDebug        = 0;
char num_ch = 0;

extern bit bStep;
char bdata status;
sbit bHistRdy = status ^ 0;
sbit bRAMRdy  = status ^ 4;
char bdata rs_state;
sbit bRS_OK   = rs_state ^ 0;
sbit bRS_ERR  = rs_state ^ 1;
sbit bRS_RDY  = rs_state ^ 7;

char bdata dsp_state1;
sbit bNARROW = 	dsp_state1 ^ 0;
sbit bMIDDLE = 	dsp_state1 ^ 1;
sbit bEXT_TV = 	dsp_state1 ^ 2;
sbit bFOCR = 	dsp_state1 ^ 3;
sbit bCROSS =   dsp_state1 ^ 4;//???????????
//sbit bTPV    = 	dsp_state1 ^ 5;
sbit bTPV_out1 = 	dsp_state1 ^ 5;//select out1
sbit bEXAM = 	dsp_state1 ^ 6;
//sbit bTHERM_EN =  dsp_state1 ^ 7;//?????????? ??????????????
sbit bTPV_out2 =  dsp_state1 ^ 7;//select out2


bit bDis_Auto = 0;


char bdata dsp_BVTK1;
sbit bDATA_REQ = dsp_BVTK1 ^ 0;

char bdata dsp_state2 = 0x28;
sbit bCOMM = 	dsp_state2 ^ 2;
sbit bREADY = 	dsp_state2 ^ 3;
sbit bHEAT = 	dsp_state2 ^ 4;
sbit bT_NORM = 	dsp_state2 ^ 5;


char bdata dsp_state3;
sbit bWatchDog = 	dsp_state3 ^ 0;
unsigned long idata ulWatchDogCnt  = 0x00000000;

bit bRS485_NEW = 0;

char bdata filter_status;
sbit bFILT19_OUT =  filter_status ^ 0;
sbit bFILT19_IN =   filter_status ^ 1;
sbit bFILT300_OUT = filter_status ^ 2;
sbit bFILT300_IN =  filter_status ^ 3;
sbit bFILT75_OUT =  filter_status ^ 4;
sbit bFILT75_IN =   filter_status ^ 5;

unsigned char temp_channel = 0;
extern short code thermo[10];

unsigned char idata ucCntAccADC =0;
unsigned long idata ulAccADC[4]= { 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
unsigned long idata ulAccTemp = 0;
unsigned char idata ucAnalogError  = ANALOG_ERROR;
unsigned char idata ucInfinity_1_5 = INFINITY_15;
unsigned char idata ucFocusMin_1_5 = FOCUS_MIN_15;
unsigned char idata ucFocusMax_1_5 = FOCUS_MAX_15;
unsigned char idata ucFocusMin_6 = FOCUS_MIN_6;
unsigned char idata ucFocusMax_6 = FOCUS_MAX_6;
unsigned char idata ucInfinity_6   = INFINITY_6;
unsigned char data adc_h[4] = { 0x11, 0x2F, 0x0D, 0x66 };
unsigned char data adc_l[4];
unsigned char idata ucRcapL = 0x00;
unsigned char idata ucRcapH = 0x80;
unsigned char rs_PSK3, rs_PSK2;

unsigned char MemDefine;
unsigned char VideoMux;
unsigned char ManFocus[4];
//unsigned char range_sum;
bit bFocusParam = 0;
unsigned char FocusRep = 0;
/*
bit bNEAR = 0;
bit bFAR = 0;
*/
char idata bNEAR[4] = {0};
char idata bFAR[4] = {0};
char idata bQ_NEAR[4]= {0};
char idata bQ_FAR[4]= {0};
extern unsigned char idata cDeltaFoc[4];
extern unsigned char focus[4];
unsigned short sFocAddr = FOCUS;
//bit bMFOCUS = 0;
bit bSelfExam = 0;

unsigned short exp_delta = DELTA_INDEX_DEF;
unsigned short index_max = INDEX_MAX - DELTA_INDEX_DEF;
unsigned short shutter_limit = SHUTTER_MAX;
unsigned short exp_delta_fine = 0;
unsigned char temp_nord;
//#define temp_nord focus[4]
unsigned char corr_delay;
//signed char ex_corr = 0x40; // 64
signed char ex_corr = 0xC0; // -64

unsigned char num_adc_ch = 0;
//short nnn = 0;

void Debug( void );

void fFocusParamInit(void);

void GetAndPrintAdcOne (unsigned char);

void DelayStopMotor(unsigned long );

void fTimer(unsigned char, unsigned char );

void fAutoFocus(void)
{
	  short sFocAddr;
    //putchar( '\n' );putchar( 'f' );putchar( 'A' );putchar( 'u' );putchar( 't' );putchar( 'o' );  putchar( 'F' );putchar( 'o' );putchar( 'c' );putchar( '\n' );
	  if(( num_adc_ch == 1 )	|| ( num_adc_ch == 2 ))	
	       {		
	         switch( num_adc_ch )
			       {                     	
			        	case 1: // focus 300 	
				            sFocAddr = FOCUS_300;
				            break;
				        case 2: // focus 75		  		          
				            sFocAddr = FOCUS_75;
				            break;
				        default:
				            break;			  	   	 			  
				     }
/*			 if( bFAR[ num_adc_ch ] || bNEAR[ num_adc_ch ] )
					{
						if( ( adc_h[ num_adc_ch ] -4 ) > focus[ num_adc_ch ] )
						{
							if( bNEAR[ num_adc_ch ] ) 
								{
								bNEAR[ num_adc_ch ] = 0;
								bFAR[ num_adc_ch ] = 0;
								}
							else
								{
								if( adc_h[ num_adc_ch ] > ( focus[ num_adc_ch ] + 8) )
										XBYTE[ sFocAddr ] = FOC_Q_FAR;
								else	XBYTE[ sFocAddr ] = FOC_FAR;
								bFAR[ num_adc_ch ] = 1;
								}
						}
						else
						   {
							if( (adc_h[ num_adc_ch ]-4 ) < focus[ num_adc_ch ] ) //-1//2
							   {
								if( bFAR[ num_adc_ch ] ) // if we did "far" and drove over
								   {
		
									XBYTE[ sFocAddr ] = FOC_Q_STOP;
									XBYTE[ sFocAddr ] = FOC_STOP;
									bNEAR[ num_adc_ch ] = 0;
									bFAR[ num_adc_ch ] = 0;
								   }
								else
								   {
									XBYTE[ sFocAddr ] = FOC_Q_NEAR;
									bNEAR[ num_adc_ch ] = 1;
							 	   }
							   }
							else
							   {
								XBYTE[ sFocAddr ] = FOC_Q_STOP; // if adc_h[ num_adc_ch ] == focus[ num_adc_ch ]
								XBYTE[ sFocAddr ] = FOC_STOP;
								bNEAR[ num_adc_ch ] = 0;
								bFAR[ num_adc_ch ] = 0;
							   }
						   }
					}// endif( bFAR || bNEAR )
					else
					{*/
						if( adc_h[ num_adc_ch ] - 1 > focus[ num_adc_ch ] )
						{
							XBYTE[ sFocAddr ] = FOC_Q_FAR;
							bFAR[ num_adc_ch ] = 1;
						}
						else
						{
							if( adc_h[ num_adc_ch ] < focus[ num_adc_ch ] )
							{
								XBYTE[ sFocAddr ] = FOC_Q_NEAR;
								bNEAR[ num_adc_ch ] = 1;
							}
							else
							{
								XBYTE[ sFocAddr ] = FOC_Q_STOP; 
								XBYTE[ sFocAddr ] = FOC_STOP;
							}
						}
				//	}				     

				 } 
    
    if( (++num_adc_ch) == 4 )
         {
          num_adc_ch = 1;
          //temp_nord = adc_h[3]- 73 + (char)ucAnalogError;
          //putchar( '\n' );putchar( 'T' );putchar( 'M' );putchar( 'P' );putchar( ' ' );putchar( 'N' );  putchar( 'O' );putchar( 'R' );putchar( 'D' );putchar( '\n' );
          temp_nord = XBYTE[ THERMO_NORD ];
          if( corr_delay == 0 ) FocusThermoCorrection( temp_nord ); 
          corr_delay++;				         
         } 
    if( num_adc_ch == 1)   ADCCON2 = ADCS_1_5;		    
    if( num_adc_ch == 2)   ADCCON2 = ADCS_6;		
    if( num_adc_ch == 3)   ADCCON2 = ADCS_EXT_T_SENSE;		
}

void CalcExpDelta()
{
	// ????????? ??????????? ???????????? ???????
	shutter_limit = SHUTTER_MAX;
	exp_delta_fine = 0;
	//-------------------------------------------
	switch( frame_limit )
	{
		case 0:
			exp_delta = DELTA_INDEX0;
			index_max = INDEX_MAX - DELTA_INDEX0;
			break;
		case 1:
			exp_delta = DELTA_INDEX1;
			index_max = INDEX_MAX - DELTA_INDEX1;
			break;
		default:
			if( frame_limit >= 250 )
			{
				exp_delta = DELTA_INDEX250;
				index_max = INDEX_MAX - DELTA_INDEX250;
			}
			else
			{
				exp_delta = 0;
				while( exp_table[ INDEX_GAIN - exp_delta ].frame > frame_limit )
				{
					exp_delta++;
				}
				index_max = INDEX_MAX - exp_delta;
			}
	}// end of switch
}

void CalcExpDeltaFine()
{
		if( shutter_limit >= SHUTTER_MAX )
		{
				exp_delta_fine = 0;
				//????????? ??????????? ??????????
				frame_limit = FRAME_LIMIT_DEF;
				exp_delta = DELTA_INDEX_DEF;
				index_max = INDEX_MAX - DELTA_INDEX_DEF;
		}
		else
		{
				//???????? ??????????? ?????????? ?? 1 ?????
				frame_limit = 0;
				exp_delta = DELTA_INDEX0;
				//---------------------------------------------
				exp_delta_fine = 0;
				while( exp_table[ INDEX_GAIN_FINE - exp_delta_fine ].shutter > shutter_limit )
				{
					exp_delta_fine++;
				}
				index_max = INDEX_MAX - DELTA_INDEX0 - exp_delta_fine;
		}
}

void int_RS485 (void) interrupt 0 /* using 3 */
{
	unsigned char n;
	unsigned char *ptr, filter_ch;
	unsigned short array_index;
	rs_state = XBYTE[ RS485_CTRL ]; 
	if( bRS_OK )
	{
		bRS485_NEW = 1;
		XBYTE[MEM_DEFINE] = MEM_RS485;
		XBYTE[MEM_ADDR] = 1;
	    for( n = 0; n < 10; n++ )
	     {
	     }
		dsp_state1 = XBYTE[MEM_DATA_L];
		XBYTE[VIDEO_MUX] = dsp_state1 & 0xA3;
		VideoMux = dsp_state1 & 0x03;

#ifdef BUILD_WITH_CROSS
		//???????????
		XBYTE[NUM_CHANNEL] = VideoMux;
		if( bCROSS)
		{
			XBYTE[0x3cFF] = 0x10;
		}
		else
		{
			XBYTE[0x3cFF] = 0x00;
		}
		XBYTE[NUM_CHANNEL]= num_ch;
		//end ???????????
#endif
//*******************************		
//*******************************		
		//XBYTE[VIDEO_MUX] = dsp_state1 & 0x23;	
		dsp_BVTK1 = XBYTE[MEM_DATA_L];	// read command if it exist
                                      //  ???5       ???6
		if( !bTPV_out1)//                   bTPV_out1  bTPV_out2
		    {
		     if( !bTPV_out2) //                0          0   
		     	   XBYTE[VIDEO_MUX] =   dsp_state1 & VIDEO_CHANNELS_MASK;
		     else            //                0          1
		     	   XBYTE[VIDEO_MUX] =  (dsp_state1 & 0x03)|0x40; 
		    }
		else                              //bTPV_out1  bTPV_out2
		    {
		     if( !bTPV_out2) //                1          0   
		     	   XBYTE[VIDEO_MUX] =  (dsp_state1 & 0x03)|0x20; 
		     else            //                1          1
		     	   XBYTE[VIDEO_MUX] =  (dsp_state1 & 0x03)|0x60; 
		    }		
//*******************************	
//*******************************
			switch( dsp_BVTK1 & PARAM_MASK )
			{
				case PARAM_EXP:
					ptr = (unsigned char*)&index[ VideoMux ];
					break;
				case PARAM_GAIN:
					ptr = (unsigned char*)&gain[ VideoMux ];
					break;
				case PARAM_SHUT:
					ptr = (unsigned char*)&shutter[ VideoMux ];
					break;
				case PARAM_FRAME:
					ptr = (unsigned char*)&frame[ VideoMux ];
					break;
				case PARAM_FILTER_300:
					filter_ch = NORD300;
					break;
				case PARAM_FILTER_75:
					filter_ch = NORD75;
					break;
				case PARAM_FILTER_19:
					filter_ch = NORD19;
					break;
				default:;
			}
			switch( dsp_BVTK1 & PARAM_MASK )
			{
				case PARAM_EXP:
				case PARAM_GAIN:
				case PARAM_SHUT:
					if( !bDATA_REQ )
					{
						*ptr = XBYTE[MEM_DATA_L]; //high byte of short (BVTK2)
					}
					rs_PSK2 = *ptr++; //high byte of short (PSK2)
				case PARAM_FRAME:
					if( !bDATA_REQ )
					{
						*ptr = XBYTE[MEM_DATA_L]; //low byte of short (BVTK3)
					}
					rs_PSK3 = *ptr; //low byte of short (PSK3)
					if( dsp_BVTK1 == PARAM_EXP )
					{
						array_index = index[ VideoMux ];
						shutter[ VideoMux ] = exp_table[ array_index ].shutter;
						gain[ VideoMux ] = exp_table[ array_index ].gain;
						frame[ VideoMux ] = exp_table[ array_index ].frame;
					}
					break;
				case PARAM_FILTER_300:
				case PARAM_FILTER_75:
				case PARAM_FILTER_19:
					if( !bDATA_REQ )
					{
						XBYTE[ NUM_CHANNEL ] = filter_ch;
						XBYTE[ FILTER ] = XBYTE[MEM_DATA_L];
						XBYTE[ NUM_CHANNEL ] = num_ch;
					}
					rs_PSK2 = 0;
					rs_PSK3 = XBYTE[ FILTER ];
					break;
				case PARAM_LIMIT:
					if( !bDATA_REQ )
					{
						frame_limit = XBYTE[MEM_DATA_L];
						CalcExpDelta();
					}
					rs_PSK3 = frame_limit;
					break;
					
				case PARAM_SHUTTER_LIMIT:
					if( !bDATA_REQ )
					{
						*((char*)&shutter_limit) = XBYTE[MEM_DATA_L]; // Hi byte
					    for( n = 0; n < 10; n++ )
					     {
					     }
						*((char*)&shutter_limit+1) = XBYTE[MEM_DATA_L]; // Lo byte
						CalcExpDeltaFine();
					}
					rs_PSK2 = *((char*)&shutter_limit);
					rs_PSK3 = *((char*)&shutter_limit+1);
					break;
					
				case PARAM_FOCUS:
					if( bFOCR )
					if( !bDATA_REQ )
					{
						ManFocus[ VideoMux ] = XBYTE[MEM_DATA_L]; // BVTK2;
						bFocusParam = 1;
					}
					rs_PSK2 = adc_h[ VideoMux ];
					rs_PSK3 = adc_l[ VideoMux ];
					break;
					
				case TEMP_ACK:
					rs_PSK2 = thermo[ temp_channel++ ];
					rs_PSK3 = XBYTE[ thermo[ temp_channel++ ] ];//????????? ? 5-? ????XBYTE
					if (temp_channel == 10)
					    {
			         temp_channel = 0; 
        			 rs_PSK3 = temp_nord;
					    }
					if( ( rs_PSK3 < 65 ) || ( rs_PSK3 > 216 ) )
					    {
					     bHEAT = 0;//0
					     bT_NORM = 1;//1
					    }
					else
					    {
					     bT_NORM = 0;//0
						   if( rs_PSK3 < 128  )
						       {
							      bHEAT = 1;//1
						       }
						   else
						       {
							      bHEAT = 0;//0
						       }
					    }
					break;
					
				case WINDOW_AV:
					rs_PSK2 = XBYTE[MEM_DATA_L]; //dummy
					if( !bDATA_REQ )
					{
					    for( n = 0; n < 10; n++ )
					     {
					     }
						XBYTE[ DSP_CTRL ] = XBYTE[MEM_DATA_L];
					}
				    for( n = 0; n < 10; n++ )
				     {
				     }
					rs_PSK3 = XBYTE[ DSP_CTRL ];
					break;
				default:;
			}
		
		if( bFOCR )
		{
			//_nop_();
			switch( dsp_BVTK1 ) //& PARAM_MASK )
			{
				case PARAM_LO:
					if( FocusRep < TYPEMATIC_DELAY )
					{
						FocusRep++;
						XBYTE[FOCUS] = FOC_NEAR;
					}
					else
					{
						XBYTE[FOCUS] = FOC_Q_NEAR;
					}
					rs_PSK2 = adc_h[ VideoMux ];
					rs_PSK3 = adc_l[ VideoMux ];
					break;
				case PARAM_MO:
					if( FocusRep < TYPEMATIC_DELAY )
					{
						FocusRep++;
						XBYTE[FOCUS] = FOC_FAR;
					}
					else
					{
						XBYTE[FOCUS] = FOC_Q_FAR;
					}
					rs_PSK2 = adc_h[ VideoMux ];
					rs_PSK3 = adc_l[ VideoMux ];
					break;
				default:;
					FocusRep = 0;
					XBYTE[FOCUS] = FOC_Q_STOP;
					XBYTE[FOCUS] = FOC_STOP;
			}
		}//endif( bFOCR )
		else
		{
				bFocusParam = 0;
		}
		XBYTE[MEM_ADDR] = 0;
		XBYTE[MEM_DATA_L] = ME_ADDR;
		XBYTE[MEM_DATA_L] = dsp_state1;//0x05;//dsp_state1;
		XBYTE[MEM_DATA_L] = dsp_state2;//0x00;//dsp_state2;
		XBYTE[MEM_DATA_L] = dsp_BVTK1; //0x05;//dsp_BVTK1;		//PSK1
		XBYTE[MEM_DATA_L] = rs_PSK2;   //0x00;//rs_adc;		//PSK2
		XBYTE[MEM_DATA_L] = rs_PSK3;   //0x05;//rs_temp;		//PSK3
		XBYTE[MEM_DEFINE] = MemDefine; //restore previous value of MEM_DEFINE
	}

}

void SaveADC( unsigned char adch, unsigned char adcl ) // thanks to the Analog Devices ( see ERRATA824_A1.pdf #824_01 )
{
	unsigned short usADC = 0;			
    char cData, ch1;
	*((unsigned char*)&usADC)   = adch & ADC_MASK; 
	*((unsigned char*)&usADC+1) = adcl;
	


	if(num_adc_ch != 3)		
		  {	
	     ulAccADC[ num_adc_ch ] = (unsigned long)usADC<<5;		  	
 	     adc_h[ num_adc_ch ] = *((unsigned char*)&ulAccADC[ num_adc_ch ]+2);  
 	     adc_l[ num_adc_ch ] = *((unsigned char*)&ulAccADC[ num_adc_ch ]+3);
	     ulAccADC[ num_adc_ch ] = 0;  	     
 	 	 	}
 	else
 		  {
		   adc_h[ num_adc_ch ] = XBYTE[ THERMO_NORD ]; 
/*
		   ulAccTemp += (unsigned long)usADC;
		   if(ucCntAccADC== 0x1F)		
		       {
    	    	ucCntAccADC = 0xFF;	
 	          adc_h[ num_adc_ch ] = *((unsigned char*)&ulAccTemp+2);  
 	          adc_l[ num_adc_ch ] = *((unsigned char*)&ulAccTemp+3);
    	    	ulAccTemp = 0;    	    		       	
		       }    
            ucCntAccADC ++;	
//*/ 	
 			}

	if(  bDebug == 1) 
		  {   	 
			if( RI )
			   {
	    	    ch1 = getchar();
	    	    ch1 = toupper( ch1 );	
	    	    switch( ch1 )
				    {
				     case 'E':	//auto focusing stop
	            		 bADCdebug = 0;
	            		 bDebug    = 0;
	         			 puts( s_exit_adc_debug );                        
				         break;
	
				     default:
		         		 break;
	                 }
	           }
	
		   if(num_adc_ch != 3)		
		       {
            putchar('\n');putchar( 'F' );putchar( 'o' );putchar( 'c' );putchar( 'u' );putchar( 's' );UartSendByte( num_adc_ch  );putchar( ' ' );
                 
		        if(num_adc_ch == 1)
		   	        {
                       putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '6' );putchar( ' ' );
		   	        }
		        if(num_adc_ch == 2)
		   	        {
		             putchar( 'M' );putchar( 'E' );putchar( '-' );putchar( '1' );putchar( '0' );putchar( '5' );putchar( ' ' );
		   	        }	
		   	    putchar( 'R' );putchar( '=' );putchar( '0' );putchar( 'x' ); 	 	
		        UartSendByte( adc_h[ num_adc_ch ] );putchar( ' ' );		 	
		        putchar( 'T' );putchar( 'e' );putchar( 'm' );putchar( 'p' );putchar( ' ' );putchar( 'N' );putchar( 'o' );putchar( 'r' );putchar( 'd' );putchar( ' ' );putchar( '0' );putchar( 'x' );
		        //cData = adc_h[3]- 73 + (char)ucAnalogError;
		        cData = XBYTE[ THERMO_NORD ]; 
		        UartSendByte( cData );
		       }
	     else
		       {
		        putchar('\n');
		        //putchar( 'A' );putchar( 'D' );putchar( 'C' );UartSendByte( adc_h[3] );putchar( ' ' );
		        putchar( 'T' );putchar( 'e' );putchar( 'm' );putchar( 'p' );putchar( ' ' );putchar( 'N' );putchar( 'o' );putchar( 'r' );putchar( 'd' );putchar( ' ' );putchar( '0' );putchar( 'x' );
		        //cData = adc_h[3]- 73 + (char)ucAnalogError;
		        cData = XBYTE[ THERMO_NORD ]; 
		        UartSendByte( cData );		  	
			     }
	     putchar( '\n' ); 	
		  }				 	
}


//???????????????
void int_ADC (void) interrupt 6
{
  //putchar( '\n' );putchar( 'i' );putchar( 'n' );putchar( 't' );putchar( ' ' );putchar( 'A' );  putchar( 'D' );putchar( 'C' );putchar( '\n' );

	T2CON &= 0x7F;	

  SaveADC( ADCDATAH, ADCDATAL );	 
	    
	if( !bADCdebug )  
///*
	   {   	
		   if( bFOCR )
		      {
		       if( bFocusParam )
		           {
		            if( ( num_adc_ch == VideoMux ) )
		                {
				             if( bFAR[ num_adc_ch ] || bNEAR[ num_adc_ch ] )
				                {
					               if( (adc_h[ num_adc_ch ] ) > ManFocus[ num_adc_ch ] )// if motor is on the adc value is larger than the real position -1
					                   {
						                  if( bNEAR[ num_adc_ch ] ) // if we did "near" and drove over
						                      {

							                     XBYTE[FOCUS] = FOC_Q_STOP;
							                     XBYTE[FOCUS] = FOC_STOP;
							                     bNEAR[ num_adc_ch ] = 0;
							                     bFAR[ num_adc_ch ] = 0;
						                      }
						                  else
						                      {

							                     XBYTE[FOCUS] = FOC_FAR;
							                     bFAR[ num_adc_ch ] = 1;
						                      }
					                   }
					               else
					                   {
						                  if( (adc_h[ num_adc_ch ] ) < ManFocus[ num_adc_ch ] ) // if motor is on the adc value is larger than the real position -1
						                      {
							                  if( bFAR[ num_adc_ch ] ) // if we did "far" and drove over
							                      {

							                    	XBYTE[FOCUS] = FOC_Q_STOP;
								                    XBYTE[FOCUS] = FOC_STOP;
								                    bNEAR[ num_adc_ch ] = 0;
								                    bFAR[ num_adc_ch ] = 0;
							                      }
							                  else
							                      {

								                     XBYTE[FOCUS] = FOC_NEAR;
								                     bNEAR[ num_adc_ch ] = 1;
							                      }
						                      }
						                  else
						                    	{

							                     XBYTE[FOCUS] = FOC_Q_STOP; // if adc_h[ num_adc_ch ] == ManFocus
							                     XBYTE[FOCUS] = FOC_STOP;
							                     bNEAR[ num_adc_ch ] = 0;
							                     bFAR[ num_adc_ch ] = 0;
						                      }
					                   }
				                }// endif( bFAR || bNEAR )
				             else
				                {
				               	 if( adc_h[ num_adc_ch ] > ManFocus[ num_adc_ch ] )
					                   {

						                  XBYTE[FOCUS] = FOC_FAR;
						                  bFAR[ num_adc_ch ] = 1;
					                   }
					               else
					                   {
					                  	if( adc_h[ num_adc_ch ] < ManFocus[ num_adc_ch ] )
					                      	{

							                     XBYTE[FOCUS] = FOC_NEAR;
							                     bNEAR[ num_adc_ch ] = 1;
						                      }
						                  else
						                      {

						                       XBYTE[FOCUS] = FOC_Q_STOP; // if adc_h[ num_adc_ch ] == ManFocus
							                     XBYTE[FOCUS] = FOC_STOP;
						                      }
					                   }
				                }
		                }// endif( num_adc_ch == VideoMux )
		           }// endif ( bFocusParam )
		      }//endif( bFOCR )
		   else// else if( bFOCR )
		      {
		       fAutoFocus();             			                     		                   	                       
	        }
	    }  
}
//???????????????

void CheckRamReady()
{
	unsigned char i, j;
			bRAMRdy = 0;
			i = 100; // 5000 * 10us = 50ms
			do
			{
			  j = 100;
			  do
			  {
				status = XBYTE[ HIST_CONTROL ];
				if( bRAMRdy )
				{
					goto skip1;
				}
			  } while( --j!=0 );
			} while( --i!=0 );
  skip1:
			if( !bRAMRdy ) puts( s_ram_error );
}
/*------------------------------------------------
The main C function.  Program execution starts
here after stack initialization.
------------------------------------------------*/
void main (void)
{
char ch, num_char = 0;
	unsigned char i, j, k;
	//unsigned short array_index;
	bit bFiltIn = 0;
	char n= 0, rs_data;

	PLLCON = 1;  	/* ?????????? PLL ?? 12.582912 ??? */

	UartInit();

    fFocusParamInit();
  	
	RCAP2L  = ucRcapL; 
	RCAP2H  = ucRcapH; 
	TL2	= 0x00; 	 
	TH2	= 0x00;		

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
		        j = EDATA1;
		     	UartSendByte( j );
	            break;
   		    case 2:
		     	j = EDATA2;
		     	UartSendByte( j );
	            break;
   		    case 3:
		     	j = EDATA3;
		     	UartSendByte( j );
	            break;
		      default:
	            break;
         }
	   }

	EADRH = 0x00;
	EADRL = 0x04;
	ECON = 1;
	ECON = 4;	   
    putchar( ' ' );  putchar( 'l' );  putchar( 'o' );  putchar( 'a' );  putchar( 'd' );  putchar( 'e' );  putchar( 'd' );  putchar( ' ' );
//"loaded 09.07.2012"
	for( i = 1; i < 5; i++ )
	   {
        switch( i )
	       {
   		    case 1:
		        j = EDATA1;
		     	UartSendByte( j );
                putchar( '.' );
	            break;
   		    case 2:
		     	j = EDATA2;
		     	UartSendByte( j );
                putchar( '.' );
	            break;
   		    case 3:
		     	j = EDATA3;
		     	UartSendByte( j );
	            break;
   		    case 4:
		     	j = EDATA4;
		     	UartSendByte( j );
	            break;
		    default:
	            break;
         }
	   }
	puts( s_copyright );
			  UartSendWord( INDEX_FILT_IN );
					  puts("\n ");
			  UartSendWord( INDEX_FILT_OUT );
					  puts("\n ");



	EX0 = 1;		// ????????? ?????????? ?? INT0 
	EX1 = 0;		// ????????? ?????????? ?? INT1 
	EA  = 1;			// ????????? ?????????? 		

	EADC	= 1;
	ADCCON1 = ADC_POWER_UP;	
    ADCCON2 = ADCS_INT_T_SENSE;
	//ADCCON2 = ADCS_EXT_T_SENSE;  
  
	T2CON  |= 0x04;

	dsp_state3 = XBYTE[ A_WD_STATE ];
//not watch dog reset
    if(!bWatchDog)
	    {	
		/* Init RS-485 */
		XBYTE[ RS485_CTRL ] = RS_PRESET;
		XBYTE[ RS485_NSND ] = NUM_BYTE_TX;
		XBYTE[ RS485_NRCV ] = NUM_BYTE_RX;
		XBYTE[ RS485_ERR  ] = 100;

	    XBYTE[ VIDEO_MUX  ] = NORD75;
		//????????? ??????? ?? ??????, ????? ???? ??????
		/*
	    EADRH = 0x00;
	    EADRL = 0x08;						         
		ECON = 1;
	    ECON = 4;
		XBYTE[A_WD_PRESET_0] = EDATA4;//00
		XBYTE[A_WD_PRESET_1] = EDATA3;//00
		XBYTE[A_WD_PRESET_2] = EDATA2;//00
		XBYTE[A_WD_PRESET_3] = EDATA1;//E0
	    //*/
        ///*
		XBYTE[A_WD_PRESET_0] = 0x00;//00
		XBYTE[A_WD_PRESET_1] = 0x00;//00
		XBYTE[A_WD_PRESET_2] = 0x00;//00
		XBYTE[A_WD_PRESET_3] = 0x00;//E0
		//*/
		XBYTE[A_WD_CTRL]     = 0x03; //????????  ?????? ???????? ?? ????
	
	    #ifdef DEBUG_BUILD
	          bDebug = 1;
		      XBYTE[A_WD_CTRL]  = 0x03; //???????? ?????? ? ??????? ?? ???? ???????? 
		      Debug();
	    #endif
	
		bREADY = SelfExamine();
	  	XBYTE[A_WD_CTRL]  = 0x01;//?????????? ?????
	
		k = 2;
		do
			{
				XBYTE[ NUM_CHANNEL ] = k;
					bRAMRdy = 0;
					i = 100; // 5000 * 10us = 50ms
					do
					{
					  j = 100;
					  do
					  {
						status = XBYTE[ HIST_CONTROL ];
						if( bRAMRdy )
						{
							goto skip;
						}
					  } while( --j!=0 );
					} while( --i!=0 );
		     skip:
					if( !bRAMRdy ) puts( s_ram_error );
					XBYTE[A_FRAME]      = FRAME_DEFAULT;
					XBYTE[A_SHUTTER_H]  = SHUTTER_DEFAULT_HI;//*((char*)&tmp_param);
					XBYTE[A_SHUTTER_L]  = SHUTTER_DEFAULT_LO;
					XBYTE[A_GAIN_H]     = GAIN_DEFAULT_HI;
					XBYTE[A_GAIN_L]     = GAIN_DEFAULT_LO;
					XBYTE[HIST_CONTROL] = HIST_REQ;
			} while( --k != 0 );
	   }//?????? ?? ????
  else//?????? ???????
  {
    putchar('\n');putchar( 'W' );putchar( '_' );putchar( 'D' );putchar( 'o' );putchar( 'g' );putchar( 'C' );putchar( 'n' );putchar( 't' );
 
	XBYTE[A_WD_CTRL]  = 0x03; //???????? ?????? ? ??????? ?? ???? ???????? 
	XBYTE[A_WD_STATE] = 0x00; //??????? ????,  
	XBYTE[A_WD_CTRL]  = 0x01; //?????????? ????? 
    //?????? ??????? ???????????? ?? W.D.
    EADRH = 0x00;
    EADRL = 0x07;						         
	ECON = 1;
    ECON = 4;
    for( i = 0; i < 4; i++ )
       {
        switch( i )
           {
            case 0:
     	        *((unsigned char*)&ulWatchDogCnt)   = EDATA1;
				putchar( ' ' );
                UartSendByte( EDATA1 );
                putchar(' ');
                break;
            case 1:
     	        *((unsigned char*)&ulWatchDogCnt+1) = EDATA2;
				putchar( ' ' );
                UartSendByte( EDATA2 );
                putchar(' ');
                break;
            case 2:
     	        *((unsigned char*)&ulWatchDogCnt+2) = EDATA3;
				putchar( ' ' );
                UartSendByte( EDATA3 );
                putchar(' ');
                break;
            case 3:
     	        *((unsigned char*)&ulWatchDogCnt+3) = EDATA4;
				putchar( ' ' );
                UartSendByte( EDATA4 );
                putchar(' ');
                break;
            default:
                break;
           }
	    }
    /*
    #ifdef BUILD_WITH_DEBUG_WD
		  XBYTE[A_WD_CTRL]  = 0x03; //???????? ?????? ? ??????? ?? ???? ???????? 
   	      ch = getchar();
     #endif
	 //*/
	XBYTE[A_WD_CTRL]  = 0x01; //?????????? ????? 
    ulWatchDogCnt  += 1;

	EADRH = 0;
	EADRL = 7;
    ECON = 1;
	EDATA4 = *((unsigned char*)&ulWatchDogCnt+3);
	EDATA3 = *((unsigned char*)&ulWatchDogCnt+2);
	EDATA2 = *((unsigned char*)&ulWatchDogCnt+1);
	EDATA1 = *((unsigned char*)&ulWatchDogCnt);                                          
	ECON = 5;
	ECON = 2;
    ECON = 4; 
  }
		// ?????? ???????? ?? ?????
  while (1)
  {
				filter_status = XBYTE[ FILTER ];
 	XBYTE[A_WD_CTRL]  = 0x03;
  	XBYTE[A_WD_CTRL]  = 0x01;
	 for( num_ch = 0; num_ch < 3; num_ch++ )
	{
	//	num_ch = 2;
	
		if( RI )
		{
			ch = getchar();
			ch = toupper( ch );

			if ( ch == 'M') 
				{
            	    //XBYTE[A_WD_CTRL]  = 0x03; //???????? ?????? ? ??????? ?? ???? ???????? 
					ADCCON1 = 0xBC;
					ADCCON2 = ADCS_EXT_T_SENSE; 
					bDebug = 1;
					Debug();			
				}
			if ( ch == 'S') 
				{
            	    XBYTE[A_WD_CTRL]  = 0x03; //???????? ?????? ? ??????? ?? ???? ???????? 
					ADCCON1 = 0xBC;
					ADCCON2 = ADCS_EXT_T_SENSE; 
					bDebug = 1;
					Debug();			
				}
			if ( ch == 'L') //only auto light
				{
                 	XBYTE[A_WD_CTRL]  = 0x03; //???????? ?????? ? ??????? ?? ???? ???????? 
					ADCCON1 = 0xBC;
					ADCCON2 = ADCS_EXT_T_SENSE; 		
				}	
			if ( ch == 'T') //only auto temp
				{
                	XBYTE[A_WD_CTRL]  = 0x03; //???????? ?????? ? ??????? ?? ???? ???????? 
					bDebug = 1;
					Debug();			
				}
			if (ch == 'Z') 
				{
					bAverage = 1;
				}
			if (ch == 'X') 							
				{
					bAverage = 0;					
				}
		}


  
		if( bRS485_NEW )
		{
		  bRS485_NEW = 0;
		  puts( s_int_rs485 );
		  putchar( '\n' );
		  XBYTE[MEM_DEFINE] = MEM_RS485;
		  XBYTE[MEM_ADDR] = 0;
		  for( j = 0; j < 6; j++ )
		     {
			  for( n = 0; n < 10; n++ )
			     {
			     }
			  rs_data = XBYTE[MEM_DATA_L];
		      UartSendByte( rs_data );
			  putchar( ' ' );
		     }
		  putchar( '\n' );
		  UartSendByte( ME_ADDR );
		  putchar( ' ' );
		  UartSendByte( dsp_state1 );
		  putchar( ' ' );
		  UartSendByte( dsp_state2 );
		  putchar( ' ' );
		  UartSendByte( dsp_BVTK1 );
		  putchar( ' ' );
		  UartSendByte( rs_PSK2 );
		  putchar( ' ' );
		  UartSendByte( rs_PSK3 );
		}

	if( bEXAM )
		if( !bSelfExam )
		{
			bREADY = SelfExamine();
			bSelfExam = 1;
		}
		else   
			bSelfExam = 0;

	
		XBYTE[ NUM_CHANNEL ] = num_ch;

		bHistRdy = 0;
    	status = XBYTE[ HIST_CONTROL ];
		if( bHistRdy )
	  	{
			if( !( bEXT_TV && (VideoMux == num_ch) ) )
 		    {
				bDis_Auto = 0;
				putchar( '\n' );
				UartSendByte( num_ch );
				putchar( ':' );
				if (bAverage) 
					{CalcHistSum();}
				else 
					{
						CalcAverage();
						thres_right = average;
					}

				// Filtering thres_right histogramm
				if(((signed char)( thres_right - (thres_old2[ num_ch ] >> 1 ) - (thres_old1[ num_ch ] >> 1 ))) >=8 )
				{
					thres_old2[ num_ch ] = thres_old1[ num_ch ];
					thres_old1[ num_ch ] = thres_right; // save current value thres_right
					thres_right = thres_old2[ num_ch ]; // change current value thres_right to previous value thres_hold
				}
				else
				{
					thres_old2[ num_ch ] = thres_old1[ num_ch ];
					thres_old1[ num_ch ] = thres_right; // save thres_hold value for next cycle process
				}
		 		SUM_OF = (unsigned char)( ( *((unsigned short*)((char*)&lsum + 1 )))  );
		  		index_tmp = index[ num_ch ]; // reading preaviose index exposition 
				printf("  index:= ");
				UartSendWord(index_tmp);
				printf("  thres:= ");
				UartSendWord(thres_right);
				printf(" SUM_OF:= ");
				UartSendWord(SUM_OF);
				printf("    sum:= ");
				UartSendWord(sum);
				printf("  QUANT:= ");
				UartSendWord(QUANT_R);
				puts("\n ");
				if ((thres_right == 255)&&(bAverage))
				{
					if (SUM_OF > 30) 
					
					new_change = -24;//Achtung 10
						else
							new_change = -12; //Achtung 10
				}
				else 
					if (bAverage)
					{
						if (num_ch == 2)
							{
								if ((thres_right !=255) && (sum == 0xFFFF))
								{
									//if (SUM_OF > 30) 
									//	XBYTE[ FILTER ] = F_IN;
									//else 
										new_change = -12; //Achtung 10
								}
								else
								{
									new_change = change_table[ thres_right ];
								}
							}
						else
							{
								if ((thres_right !=255) && (sum == 0xFFFF))
								{
									if (SUM_OF > 30) 
										new_change = -24; //Achtung 10
									else 
										new_change = -12; //Achtung 10
								}
								else
								{
									new_change = change_table[ thres_right ];
								}
							}
				   	}
					else 
					{
						new_change = change_table_av[ thres_right ];
					}

				index_tmp = index_tmp + new_change;
				printf("     index:= ");
				UartSendWord(index_tmp);
				printf(" change:= ");
				UartSendWord(new_change);
				puts("\n");
				index_tmp = index[ num_ch ] + (( index_tmp - index[ num_ch ]) >> 1 );
				switch (num_ch) 
				{
					case 0 : 
					{

							{
								if( index_tmp < 22 ) //change from INDEX_FILT_IN 39 +5
								{
									filter_status = XBYTE[ FILTER ];
									if (!bFILT19_IN) 
									{
										XBYTE[ FILTER ] = F_IN;
										index_tmp = index_tmp;			
									}
									else 
										index_tmp = index_tmp;			
								}
								if( index_tmp > 269)//INDEX_FILT_OUT )
								{
									filter_status = XBYTE[ FILTER ];
									if (!bFILT19_OUT) 
									{
										XBYTE[ FILTER ] = F_OUT;
										index_tmp = index_tmp - 154;			
									}
									else 
										index_tmp = index_tmp;			
								}
							break;
							}
					}
					case 1 : 
					{
						DUMMY01 = XBYTE[DUMMY_AV01];
						if (DUMMY01 > 0x10)
						{
							XBYTE[ FILTER ] = F_IN;
							index_tmp = index_tmp;									
							break;
						}
						else
							{
								if( index_tmp < INDEX_FILT_IN )
								{
									filter_status = XBYTE[ FILTER ];
									if (!bFILT300_IN) 
									{
										XBYTE[ FILTER ] = F_IN;
										index_tmp = index_tmp;			
									}
									else 
										index_tmp = index_tmp;			
								}

								if( index_tmp > INDEX_FILT_OUT ) 
								{
									filter_status = XBYTE[ FILTER ];
									if (!bFILT300_OUT) 
									{
										XBYTE[ FILTER ] = F_OUT;
										index_tmp = index_tmp - 134;			
									}
									else 
										index_tmp = index_tmp;			
								}
							break;
							}
					}
						

					case 2 : 
					{
						
						DUMMY02 = XBYTE[DUMMY_AV02];
						if (DUMMY02 > 0x10)
						{
							XBYTE[ FILTER ] = F_IN;
							index_tmp = index_tmp;									
							break;
						}
						else
						{
							if( index_tmp < 27 )//35
							{
								filter_status = XBYTE[ FILTER ];
								if (!bFILT75_IN) 
								{
									XBYTE[ FILTER ] = F_IN;
									index_tmp = index_tmp;			
								}
								else index_tmp = index_tmp;			
							}
							if( index_tmp > INDEX_FILT_OUT )
							{
								filter_status = XBYTE[ FILTER ];
								if (!bFILT75_OUT) 
								{
									XBYTE[ FILTER ] = F_OUT;
									index_tmp = index_tmp - 186;//186			
								}
								else index_tmp = index_tmp;			
							}
							break;
						}
					}

			         default:break;

				}
							printf("   DUMMY02:= ");
							UartSendWord(DUMMY02);



				if( index_tmp > (signed short)index_max )
				{
					index_tmp = index_max;
				}
				else
				{
					if( index_tmp < 0 )
					{
						index_tmp = 0;
					}
				}
			   	index[ num_ch ] = index_tmp;

		    } 
			else
				bDis_Auto = 1;
			CheckRamReady();
			printf("     index:= ");
			UartSendWord(index_tmp);
			PrintAndSetParam();
			XBYTE[ HIST_CONTROL ] = HIST_RDY_CLEAR;
		 } 
		 XBYTE[ HIST_CONTROL ] = HIST_REQ;
		 if( bStep ) 
		 {
		 	bDebug = 1;
			Debug();
		 }

	}//for ch 0->2

  }//while forever

}

#ifndef barkH
#define barkH

#define NORD19		0x00
#define NORD300		0x01
#define NORD75		0x02
#define NORD_TEMP		0x03


#define ALL_CHANNELS_MASK   0xA3  //0x23
#define VIDEO_CHANNELS_MASK 0x03  //
#define TPV_CHANNEL_1       0x20
#define TPV_CHANNEL_2       0x80

#define NUM_CHANNEL		0x01ff
#define VIDEO_MUX		0x08ff
#define MEM_ADDR 		0x10ff
#define MEM_ADDR_H 		0x11ff

#define MEM_DATA_L 		0x12ff
#define MEM_DATA_H 		0x13ff
#define MEM_DEFINE 		0x14ff
	#define MEM_HIST		0x01
	#define MEM_RS485		0x02


#define START_TEST_RS485 0x47ff

#define HIST_CONTROL 	0x1aff
	#define HIST_REQ 		0x01
	#define HIST_RDY 		0x01
	#define HIST_RDY_CLEAR	0x80
	#define RAM_RDY 		0x10
#define A_FRAME			0x28ff
	#define FRAME_DEFAULT	0
	#define FRAME_MAX		255
	#define FRAME_MIN		  0	
#define A_SHUTTER_H		0x2bff
#define A_SHUTTER_L		0x2aff
	#define SHUTTER_DEFAULT	500
	#define SHUTTER_DEFAULT_HI (SHUTTER_DEFAULT >> 8)
	#define SHUTTER_DEFAULT_LO (SHUTTER_DEFAULT & 0xff)
	#define SHUTTER_MAX		620
#define A_GAIN_H		0x2fff
#define A_GAIN_L		0x2eff
	#define GAIN_DEFAULT	432
	#define GAIN_DEFAULT_HI	(GAIN_DEFAULT >> 8)
	#define GAIN_DEFAULT_LO	(GAIN_DEFAULT & 0xff)
	#define GAIN_MAX		1023
#define RS485_CTRL		0x40ff
	#define RS_PRESET		0x0a /* 00001010 */
	#define RS_RDY 		0x80
	#define RS_OK 		0x01
	#define RS_ERR 		0x02
#define RS485_ERR		0x41ff
#define RS485_NSND		0x42ff
#define RS485_NRCV		0x43ff

#define THERMO_0		0x50ff//ar50,ar51,ar52,ar53
#define THERMO_1		0x51ff
#define THERMO_2		0x52ff
#define THERMO_DSP		0x53ff
#define THERMO_NORD     0x54ff

#define FILTER			0x58ff//aw5801,aw5802,aw5800
	#define F_IN 		0x01
	#define F_OUT 		0x02
	
	
#define A_FOCUS_NARROW 	0x5a	
#define A_FOCUS_MIDDLE 	0x5b
#define A_FOCUS_DUMMY 	0x5c

#define SPEED_FOCUS_3  	0x0003FFFF
#define SPEED_FOCUS_2 	0x00003FFF
#define SPEED_FOCUS_1 	0x000003FF
	
#define FOCUS			0x59ff//aw5901,aw5902,aw5900
#define FOCUS_300		0x5aff//aw5a01,aw5a02,aw5a00
#define FOCUS_75		0x5bff
#define FOCUS_DUMMY		0x5cff
	#define FOC_FAR		0x06
	#define FOC_NEAR	0x05
	#define FOC_STOP	0x00
	#define FOC_Q_STOP	0x03
	#define FOC_Q_FAR	  0x02
	#define FOC_Q_NEAR	0x01
	
#define VSYNC			0x68ff

#define A_WD_PRESET_0   0x68ff//7..0
#define A_WD_PRESET_1   0x69ff//15..8
#define A_WD_PRESET_2   0x6Aff//23..16
#define A_WD_PRESET_3   0x6Bff//31..24
#define A_WD_CTRL       0x6Cff
#define A_WD_STATE      0x6Dff

#define DSP_CTRL		0x70ff
#define AVERAGE			0x80ff
#define HIST_SUM1		0x90ff
#define HIST_SUM2		0x91ff
#define HIST_SUM3		0x92ff
#define PIX_SUM1		0x94ff
#define PIX_SUM2		0x95ff
#define PIX_SUM3		0x96ff
#define PIX_SUM4		0x97ff

	#define DUMMY_AV00	0xC0ff
	#define DUMMY_AV01	0xC1ff
	#define DUMMY_AV02	0xC2ff

#define EXAMINE			0xF8ff

#define INDEX_DEFAULT	150
#define INDEX_MAX		511
#define INDEX_FILT_IN	39
#define INDEX_FILT_OUT	263
#define INDEX_GAIN		475
#define INDEX_FRAME0	282
#define INDEX_FRAME1	306
#define DELTA_INDEX0	(INDEX_GAIN-INDEX_FRAME0)
#define DELTA_INDEX1	(INDEX_GAIN-INDEX_FRAME1)
#define DELTA_INDEX250	0
#define INDEX_GAIN_FINE	229
#define SHUTTER_MAX		620
#define FRAME_LIMIT_DEF	0
#define DELTA_INDEX_DEF	DELTA_INDEX1
//#define QUANT_R			7840		/* 5% from 568x376 0x000029b6*/
#define QUANT_R			11760		/* 3% from 700x560 0x00004c90*/
#define QUANT_L			22118		/* 5% unused */

struct exp
{
	unsigned char	frame;
	unsigned short 	shutter;
	unsigned short	gain;
} ;

struct st_thres
{
	unsigned char	left;
	unsigned char	right;
};

void FocusThermoCorrection( signed char temp );

#endif
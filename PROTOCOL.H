#ifndef protocolH
#define protocolH

#define ME_ADDR 0x15
#define NUM_BYTE_TX 0x06
#define NUM_BYTE_RX 0x05

#define PARAM_MASK 0x3e
/* EXT_TV parameters */
#define PARAM_GAIN 		 0x20 // установка АЦП для текущего канала
#define PARAM_SHUT 		 0x38 // установка затвора для текущего канала
#define PARAM_EXP 		 0x08 // установка экспозиции для текущего канала
#define PARAM_FRAME 	 0x28 // установка числа кадров накопления для текущего канала
#define PARAM_FILTER_300 0x1c //светофильтр узкого канала
#define PARAM_FILTER_75  0x3e //светофильтр среднего канала
#define PARAM_FILTER_19  0x02 //светофильтр широкого канала
#define PARAM_LIMIT 	 0x0a //ограничение накопления
#define PARAM_SHUTTER_LIMIT 0x2c //ограничение затвора
/* end of EXT_TV parameters */
#define PARAM_FOCUS 	 0x34
#define PARAM_MO 		 0x3a
#define PARAM_LO 		 0x06
#define WINDOW_AV		 0x14 		
#define TEMP_ACK 		 0x26//26<<2
	#define TEMP_1 		 0x26//узкий   протокол TEMP_0 0x26
	#define TEMP_2 		 0x16//средний протокол TEMP_1 0x16
	#define TEMP_0 		 0x36//широкий протокол TEMP_2 0x36
	#define TEMP_DSP 	 0x0e//проц  протокол
	#define TEMP_NORD 	 0xce//температура объектива
#define TEMP_ZERO 0x00
#define TYPEMATIC_DELAY 14

#endif

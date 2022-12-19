#ifndef focusH
#define focusH

#define CAM	107605//ADuC842

#ifdef CAM

  #if CAM == 107605
	#define FOCUS_MIN_15 0x08//   0x09-0x99
	#define FOCUS_MAX_15 0x92//   0x07-0xAE middle
	#define FOCUS_MIN_6  0x15//   
	#define FOCUS_MAX_6  0xAA	
	#define INFINITY_15  0x22//
	#define INFINITY_6   0x16// 0x07-0x53
	#define ANALOG_ERROR 0x00//-1//0
	#define DUMMY_LOCAL  0x00	
  #endif

  #if CAM == 105401
	#define FOCUS_MIN_15 0x09//   0x09-0x99
	#define FOCUS_MAX_15 0x99//   0x07-0xAE middle
	#define FOCUS_MIN_6  0x07//   
	#define FOCUS_MAX_6  0xAE	
	#define INFINITY_15  0x28//
	#define INFINITY_6   0x0F// 0x07-0x53
	#define ANALOG_ERROR 0xFE//-1//0
	#define DUMMY_LOCAL  0x00	
  #endif
  
  #if CAM == 104701
	#define FOCUS_MIN 0x08//   0x08-0x8D
	#define FOCUS_MAX 0x8D//   0x0d-0xA6 middle
	#define INFINITY_15 0x2A//
	#define INFINITY_6 0x14// 0x07-0x53
	#define ANALOG_ERROR 2//-1//0
  #endif
    
  #if CAM == 104801
	#define FOCUS_MIN 0x0C
	#define FOCUS_MAX 0xB3
	#define INFINITY_15 0x2C//58 for 23C
	#define INFINITY_6 0x1B//28 for 24C
	#define ANALOG_ERROR -1//0
  #endif

  #if CAM == 104802 
	#define FOCUS_MIN 0x0C
	#define FOCUS_MAX 0xB3
	#define INFINITY_15 0x2C//58 for 23C
	#define INFINITY_6 0x1B//28 for 24C
	#define ANALOG_ERROR -1//0
  #endif
  
  #if CAM == 107201//№1: 2010 05.05.10
	#define FOCUS_MIN 0x0C
	#define FOCUS_MAX 0xB3
	#define INFINITY_15 0x7A//было 43
	#define INFINITY_6 0x3f//было 1F
	#define ANALOG_ERROR 0
  #endif
  
  #if CAM == 107202//№1: 2010 06.05.10
	#define FOCUS_MIN 0x0C
	#define FOCUS_MAX 0xB3
	#define INFINITY_15 0x23
	#define INFINITY_6 0x2e
	#define ANALOG_ERROR 0
  #endif
  
#else 
  #error Не определен номер камеры!
#endif

#endif

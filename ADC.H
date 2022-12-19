#ifndef adcH
#define adcH

// operational modes of both ADCs
#define ADCS_SINGL_CONV_1_5 0x14//AUX_AIN3
#define ADCS_SINGL_CONV_6   0x13//AUX_AIN5
#define ADCS_SINGL_CONV_EXT_T_SENSE  0x15//AUX_AIN3
#define ADCS_SINGL_CONV_INT_T_SENSE 0x18
#define ADCS_CONT_CONV_INT_T_SENSE 0x28

#define ADCS_1_5 0x04//AUX_AIN3
#define ADCS_6   0x03//AUX_AIN5
#define ADCS_EXT_T_SENSE 0x05//AUX_AIN3
#define ADCS_INT_T_SENSE  0x08

#define ADC_MASK 0x0F
#define ADC_POWER_UP 0xBE


#endif

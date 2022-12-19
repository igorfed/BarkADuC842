

char code s_copyright[] =  { 
                             "\n\tBark Firmware ver.2.02  20.02.2015"// 
                             "\n\tprocessor : ADuC842"
                             "\n\tcompiler  : Keil mkVision2 V2.12 Cx51"
                             "\n\tdeveloped by K.Rogatchev"                           
							 "\n\tmodified  by I.Fedorov"
                           };
                           
char code s_debug[] = "DE";//DEBUG
char code s_enter_debug[] = "\n";
char code s_help[] = {
"\n\tCV00 - Switch ON NORD19  (Wide   field)"
"\n\tCV01 - Switch ON NORD300 (Narrow field)"
"\n\tCV02 - Switch ON NORD75  (Middle field)"		
"\n\tCH00 - Switch ON NORD19  (Wide   field)"
"\n\tCH01 - Switch ON NORD300 (Narrow field)"
"\n\tCH02 - Switch ON NORD75	(Middle field)"		
"\n\tCT00 - bit5 = 0, bit7 = 0"
"\n\tCT01 - bit5 = 0, bit7 = 1"
"\n\tCT02 - bit5 = 1, bit7 = 0"	
"\n\tCT03 - bit5 = 1, bit7 = 1"			
"\n\t1    - One step of automat of the light (available under debug mode)"
"\n\tM    - change mode automat to debug and vice versa"
"\n\tw<aa><dd> \t- Write BYTE <dd> to FPGA @ address <aa>"
"\n\tr<aa>\t\t- Read BYTE from FPGA @ address <aa>"
"\n\tAS   - Adc mode auto focuses Stop"
"\n\tAR   - Adc mode auto focuses Run"
"\n\t\tE  - under runing Adc mode: auto focuses Exit"
"\n\tAE   - exit debug mode adc without runnig auto focuses"
"\n\tp?   - help for parametres manual changing"
"\n\ts?   - help for settings of a camera's parametres"
"\n\tf?   - focus parametres settings"
};
char code s_help_parameter[] = {
"\n\t pe+   Parameter Exposure integral increment value"
"\n\t pe-   Parameter Exposure integral decrement value"
"\n\t pg+   Parameter Gain increment value"
"\n\t pg-   Parameter Gain decrement value"
"\n\t pf+   Parameter Frame exposure increment value"
"\n\t pf-   Parameter Frame exposure decrement value"
"\n\tpsr+   Parameter Shutter Rough increment value"
"\n\tpsr-   Parameter Shutter Rough decrement value"
"\n\t pn+   Parameter Narrow field focus increment value slowly " 
"\n\t pn-   Parameter Narrow field focus decrement value slowly"
"\n\t pn*   Par Narrow field focus increment value fast "
"\n\t pn/   Par Narrow field focus decrement value fast"
"\n\t pnn   Par Narrow field focus increment value continue near"
"\n\t pnf   Par Narrow field focus decrement value continue far"
"\n\t pn=   stop continue / request potentiometer data Narrow"
"\n\t pm+   Parameter Middle field focus increment value slowly speed" 
"\n\t pm-   Parameter Middle field focus decrement value slowly speed"
"\n\t pm*   Par Middle field focus increment value fast speed"
"\n\t pm/   Par Middle field focus decrement value fast speed"
"\n\t pmn   Par Middle field focus increment value continue to near"
"\n\t pmf   Par Middle field focus decrement value continue to far"
"\n\t pm=   stop continue /request potentiometer data Middle "
"\n\t pt    request temperature data (t-next request)"
                      };
char code s_help_settings[] = {
"\n\tssrv<dd><dd> Set Parameter Shutter Rough Value"
"\n\tssrl<dd><dd> Set Parameter Shutter Rough Limit value"
"\n\tsgv <dd><dd> Set Parameter Gain Value"
"\n\tsev <dd><dd> Set Parameter Exposure integral value"
"\n\tsni<dd>      Set Parameter Narrow Infinity"
"\n\tsne<dd>      Set Parameter Narrow Error"
"\n\tsmi<dd>      Set Parameter Middle Infinity"
"\n\tsme<dd>      Set Parameter Middle Error"
"\n\tsmo          Set Middle Open  filter"
"\n\tsmc          Set Middle Close filter"
"\n\tsmi<dd>      Set Middle Infinity to<dd>"
"\n\tsno          Set Narrow Open  filter"
"\n\tsnc          Set Narrow Close filter"
"\n\tsni<dd>      Set Narrow Infinity to<dd>"
"\n\tsne<dd>      Set AnalogError to<dd> for Both channel"
"\n\tswo          Set Wide   Open  filter"
"\n\tswc          Set Wide   Close filter"
"\n\tsao          Set All channel Open  filters"
"\n\tsac          Set All channel Close filters"
"\n\tsc<dd>       Set view & ctrl to channel #<dd>"
"\n\tsd           Set default constants for focuses"
"\n\ts#           Set camera ME-108 Serial Number"
"\n\tsl           Set loaded date"
};

char code s_debug_error[] = "\n^Error\a";
//char code s_debug_prompt[] = "\n-";
char code s_hist_error[] = "\nHist not r-dy";
char code s_ram_error[] = "\nRAM not r-dy";
//char code s_sum[] = " Summ=";
char code s_int_rs485[] = "\nRS485!";

char code s_exit_adc_debug[] = "\nexit adc debug!";
char code s_adc_channel3[] = "\n single ADC mode 2_4; adc channel 3!";
char code s_adc_channel4[] = "\n single ADC mode 1_5; adc channel 4!";
char code s_adc_channel5[] = "\n single ADC mode temp sense; adc channel 5!";
char code s_single_ADC_mode[] = "single ADC mode!";
char code s_cont_ADC_mode[] = "continuous ADC mode!";



















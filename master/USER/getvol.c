#include "getvol.h"
#include "led.h"
#include "adc.h"
#include "delay.h"
void getvoltage()
{
		DRIVE_ROW1;
		delay_ms(3);
		scan_row(1);
		UNDRIVE_ROW1;
		delay_ms(3);
		
		DRIVE_ROW2;
		delay_ms(3);
		scan_row(2);
		UNDRIVE_ROW2;
		delay_ms(3);
	
		DRIVE_ROW3;
		delay_ms(3);
		scan_row(3);
		UNDRIVE_ROW3;
		delay_ms(3);
		
		DRIVE_ROW4;
		delay_ms(3);
		scan_row(4);
		UNDRIVE_ROW4;
		delay_ms(3);
		
		DRIVE_ROW5;
		delay_ms(3);
		scan_row(5);
		UNDRIVE_ROW5;
		delay_ms(3);
		
		DRIVE_ROW6;
		delay_ms(3);
		scan_row(6);
		UNDRIVE_ROW6;
		delay_ms(3);
		
		DRIVE_ROW7;
		delay_ms(3);
		scan_row(7);
		UNDRIVE_ROW7;
		delay_ms(3);
		
		DRIVE_ROW8;
		delay_ms(3);
		scan_row(8);
		UNDRIVE_ROW8;
		delay_ms(3);
		
		DRIVE_ROW9;
		delay_ms(3);
		scan_row(9);
		UNDRIVE_ROW9;
		delay_ms(3);
		
		DRIVE_ROW10;
		delay_ms(3);
		scan_row(10);
		UNDRIVE_ROW10;
		delay_ms(3);
		
		DRIVE_ROW11;
		delay_ms(3);
		scan_row(11);
		UNDRIVE_ROW11;
		delay_ms(3);
		
		DRIVE_ROW12;
		delay_ms(3);
		scan_row(12);
		UNDRIVE_ROW12;
		delay_ms(3);
}
void scan_row(int row)
{
		extern uint16_t ADC_ConvertedValue[Channel_Num];
		extern float press_vol[48];
		extern u8 ADC_origin[72];//½ô×Å·Å72¹»ÁË
	
//		press_vol[4*(row-1)] = (float) ADC_ConvertedValue[0]/4096*3.3;
//		press_vol[4*(row-1)+1] = (float) ADC_ConvertedValue[1]/4096*3.3;
//		press_vol[4*(row-1)+2] = (float) ADC_ConvertedValue[2]/4096*3.3;
//		press_vol[4*(row-1)+3] = (float) ADC_ConvertedValue[3]/4096*3.3;
	
	  ADC_origin[6*(row-1)] = ((ADC_ConvertedValue[0] & 0x0ff0) >> 4);
		ADC_origin[6*(row-1)+1] = 0x00;
		ADC_origin[6*(row-1)+1]|= (ADC_ConvertedValue[0]<<4);
		ADC_origin[6*(row-1)+1]|= ((ADC_ConvertedValue[1] & 0x0f00) >> 8);
		ADC_origin[6*(row-1)+2] = (ADC_ConvertedValue[1] & 0x00ff);
		
		ADC_origin[6*(row-1)+3] = ((ADC_ConvertedValue[2] & 0x0ff0) >> 4);
		ADC_origin[6*(row-1)+4] = 0x00;
		ADC_origin[6*(row-1)+4]|= (ADC_ConvertedValue[2]<<4);
		ADC_origin[6*(row-1)+4]|= ((ADC_ConvertedValue[3] & 0x0f00) >> 8);
		ADC_origin[6*(row-1)+5] = (ADC_ConvertedValue[3] & 0x00ff);

//		ADC_origin[8*(row-1)] = ((ADC_ConvertedValue[0] & 0xff00) >> 8);
//		ADC_origin[8*(row-1)+1] = ADC_ConvertedValue[0] & 0x00ff;
//		ADC_origin[8*(row-1)+2] = ((ADC_ConvertedValue[1] & 0xff00) >> 8);
//		ADC_origin[8*(row-1)+3] = ADC_ConvertedValue[1] & 0x00ff;
//		ADC_origin[8*(row-1)+4] = ((ADC_ConvertedValue[2] & 0xff00) >> 8);
//		ADC_origin[8*(row-1)+5] = ADC_ConvertedValue[2] & 0x00ff;
//		ADC_origin[8*(row-1)+6] = ((ADC_ConvertedValue[3] & 0xff00) >> 8);
//		ADC_origin[8*(row-1)+7] = ADC_ConvertedValue[3] & 0x00ff;
}
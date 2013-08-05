#ifndef _LCD_CONFIG_H_
#define _LCD_CONFIG_H_


#define LCD_8BIT_MODE           0	
#define LCD_16BIT_MODE          1	

#define LCD_DRIVER_ILI9320      0
#define LCD_DRIVER_ILI9325      0
#define LCD_DRIVER_ILI9328      0
#define LCD_DRIVER_ILI9331      0
#define LCD_DRIVER_R61509V      0
#define LCD_DRIVER_HX8352       1

#define X_MAX_PIXEL	        128
#define Y_MAX_PIXEL	        160

#if ((LCD_DRIVER_ILI9320)||(LCD_DRIVER_ILI9325)||(LCD_DRIVER_ILI9328)||(LCD_DRIVER_ILI9331))
//9320 reg address
#define WINDOW_XADDR_START		0x0050 // 
#define WINDOW_XADDR_END		0x0051 // 
#define WINDOW_YADDR_START		0x0052 // 
#define WINDOW_YADDR_END		0x0053 // 
#define GRAM_XADDR		    	0x0020 // 
#define GRAM_YADDR		    	0x0021 // 
#define GRAMWR 			    	0x0022 // 
#endif

#if (LCD_DRIVER_R61509V)
//9325 reg address
#define WINDOW_XADDR_START		0x0210 // 
#define WINDOW_XADDR_END		0x0211 // 
#define WINDOW_YADDR_START		0x0212 // 
#define WINDOW_YADDR_END		0x0213 // 
#define GRAM_XADDR		    	0x0200 // 
#define GRAM_YADDR		    	0x0201 // 
#define GRAMWR 			    	0x0202 // 
#endif

#if (LCD_DRIVER_HX8352)
#define WINDOW_XADDR_START_H	        0x0002 // 
#define WINDOW_XADDR_START_L	        0x0003 // 
#define WINDOW_XADDR_END_H		0x0004 // 
#define WINDOW_XADDR_END_L		0x0005 // 

#define WINDOW_YADDR_START_H	        0x0006 // 
#define WINDOW_YADDR_START_L	        0x0007 // 
#define WINDOW_YADDR_END_H		0x0008 // 
#define WINDOW_YADDR_END_L		0x0009 // 

#define GRAM_XADDR_H		        0x0002 // 
#define GRAM_XADDR_L		        0x0003 // 
#define GRAM_YADDR_H		        0x0004 // 
#define GRAM_YADDR_L		        0x0005 // 
#define GRAMWR 			    	0x0022 // GRAM
#endif

#endif


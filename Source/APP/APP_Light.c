#include "YK_APP.h"

#define APP_Light_PRINTF(...)    //printf(__VA_ARGS__)

/*启动 APP LIGHT调光定时器*/
#define APP_LIGHT_START()   timer_run(TIMER1)


void app_light_timer_cb(void *arg);
static LIGHT light;

#if (HW_RGB_ENABLE == 1)
static void HSL_2_RGB(U16 Target_H, U16 Target_S, U16 Target_L, U8 *pTarget_R, U8 *pTarget_G, U8 *pTarget_B);
#endif

#if (HW_RGB_ENABLE == 1)
/****************Gamma 校准表*************************/
/* 256-step brightness table: gamma = 2.3 */ 
const U8 s_Gamma_Table[256] = 
{
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,
      2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
      5,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9, 10, 10, 10,
     11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17,
     18, 18, 19, 19, 20, 20, 21, 21, 22, 23, 23, 24, 24, 25, 26, 26,
     27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 38,
     38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51,
     52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
     69, 70, 71, 72, 73, 74, 75, 76, 78, 79, 80, 81, 82, 84, 85, 86,
     87, 89, 90, 91, 92, 94, 95, 96, 98, 99,100,102,103,104,106,107,
    109,110,112,113,114,116,117,119,120,122,123,125,126,128,130,131,
    133,134,136,138,139,141,143,144,146,148,149,151,153,154,156,158,
    160,161,163,165,167,169,170,172,174,176,178,180,182,183,185,187,
    189,191,193,195,197,199,201,203,205,207,209,211,213,215,218,220,
    222,224,226,228,230,233,235,237,239,241,244,246,248,250,253,255,
};
#endif

#if (HW_RGB_ENABLE == 1)
/*原始RGB值表*/
const U8 s_Contrast_RGB[] =  
{
    0xee,0xe8,0xaa,//light goldenrod
    0xff,0xfa,0xcd,//lemon chiffon
    0xcd,0x85,0x3f,//peru
    0x7b,0x3f,0x00,//chocolate
    0xf4,0xa4,0x60,//sandy brown
    0xff,0x7f,0x50,//coral
    0xff,0x63,0x47,//tomato
    0xff,0x45,0x00,//orange red
    0xff,0xc0,0xcb,//pink
    0xdc,0x14,0x3c,//crimson
    0x87,0xce,0xfa,//light sky blue
    0x64,0x95,0xed,//cornflower blue
    0x3f,0x00,0xff,//ultramarine
    0xaf,0xee,0xee,//light turquoise
    0xbf,0xff,0x00,//lime
    0x22,0x8b,0x22,//forest
    0xff,0x75,0x18,//pumpkin
    0xc8,0x41,0x86,//smitten
    0x93,0x70,0xdb,//medium purple
    0x8a,0x2b,0xe2,//blue violet
    0x4b,0x00,0x82,//indigo
    0x7f,0xff,0xd4,//aquamarine
    0x40,0xe0,0xd0,//turquoise
    0x90,0xee,0x90,//light green
};
#endif

#if (HW_RGB_ENABLE == 1)
/*纠正过的RGB值表*/
const U8 s_Correction_RGB[] = 
{
    0xf0,0xea,0x78,//light goldenrod
    0xff,0xfa,0x64,//lemon chiffon
    0xff,0x55,0x1b,//peru
    0xff,0x38,0x00,//chocolate
    0xff,0x5b,0x28,//sandy brown
    0xff,0x37,0x1c,//coral
    0xff,0x5a,0x3c,//tomato
    0xff,0x13,0x00,//orange red
    0xff,0x6e,0x83,//pink
    0xff,0x0a,0x0a,//crimson
    0x64,0xb0,0xff,//light sky blue
    0x50,0x64,0xff,//cornflower blue
    0x1e,0x00,0xff,//ultramarine
    0xad,0xb3,0xbf,//light turquoise
    0x78,0xff,0x00,//lime
    0x00,0xff,0x00,//forest
    0xff,0x30,0x04,//pumpkin
    0xff,0x1c,0x62,//smitten
    0x9a,0x3a,0xbf,//medium purple
    0x89,0x0a,0xbf,//blue violet
    0x7c,0x00,0xbf,//indigo
    0x6b,0xb3,0x8a,//aquamarine
    0x34,0xb3,0xa8,//turquoise
    0x88,0xb3,0x53,//light green
};
#endif

/*****************************************************************************
Function name :  APP_Light_Init
Description:	 Light任务的初始化
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Light_Init(void)
{
    memset(&light, 0x00, sizeof(light));

    if(PROV_TYPE_WITH_AMANZON == gst_AppMemDB.Provision)
    {
        light.mode = MEM_DB_MESH.LightMode;
        light.lightness = LightStatus.lightness.current;
        
        if(CW_MODE == light.mode)
        {
            light.temperature = LightStatus.temp.current;
        }
        else
        {
            HSL_2_RGB(LightStatus.hue.current, LightStatus.saturation.current, LightStatus.lightness.current, 
                &light.r, &light.g, &light.b);
        }
    }
    else
    {
    	light.mode = MEM_DB_BLE.LightMode;
    	light.lightness = MEM_DB_BLE.Level;
    	
    	if(CW_MODE == light.mode)
    	{
    		light.temperature = MEM_DB_BLE.Temperature;
    	}
    	else
    	{
    		light.r = MEM_DB_BLE.R;
    		light.g = MEM_DB_BLE.G;
    		light.b = MEM_DB_BLE.B;
    	}
    }
    
	if(gst_AppMemDB.OTAWithOff)
    {
        light.onoff = OFF;
    }
    else
    {
        light.onoff = ON;
    }
	
	light_start_transition(1000);
	APP_LIGHT_START();
}

/*****************************************************************************
Function name :  light_start_transition
Description:	 启动渐变转换动作
Input:	         transtime->总的渐变持续时间，单位1ms
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void light_start_transition(U16 transtime)
{
	uint32_t cw_total_count;
	
	APP_Light_PRINTF("start_transition\r\n");
	
	light.start_r = light.current_r;
	light.start_g = light.current_g;
	light.start_b = light.current_b;
	light.start_w = light.current_w;
	light.start_c = light.current_c;

	if(light.onoff == ON)
	{
		if(light.mode == CW_MODE)
		{
			light.target_r = 0;
			light.target_g = 0;
			light.target_b = 0;
			cw_total_count = (YK_CW_DIM_LEVEL_MAX - YK_CW_DIM_LEVEL_MIN) * (U32)light.lightness/100.0 + YK_CW_DIM_LEVEL_MIN;
			light.target_c =  cw_total_count * ((U32)light.temperature - YK_TEMPERATURE_MIN)/(YK_TEMPERATURE_MAX - YK_TEMPERATURE_MIN);
			light.target_w =  cw_total_count - light.target_c;
		
		}
		else if(light.mode == RGB_MODE)
		{
			light.target_r = HW_RGB_LEVEL_MAX*light.r/255*light.lightness/100.0;
			light.target_g = HW_RGB_LEVEL_MAX*light.g/255*light.lightness/100.0;
			light.target_b = HW_RGB_LEVEL_MAX*light.b/255*light.lightness/100.0;
			light.target_c = 0;
			light.target_w = 0;
		}
	}
	else
	{
		light.target_r = 0;
		light.target_g = 0;
		light.target_b = 0;
		light.target_c = 0;
		light.target_w = 0;
	}
	
	APP_Light_PRINTF("light.target_r:%d\r\n",light.target_r);
	APP_Light_PRINTF("light.target_g:%d\r\n",light.target_g);
	APP_Light_PRINTF("light.target_b:%d\r\n",light.target_b);
	APP_Light_PRINTF("light.target_c:%d\r\n",light.target_c);
	APP_Light_PRINTF("light.target_w:%d\r\n",light.target_w);

	light.transtime = transtime;
	light.transtime_elapsed = 0;
	light.transitioning = 1;
	
}
void app_light_timer_cb(void *arg) 
{
	if(light.transitioning)
	{
		light.transtime_elapsed += APP_LIGHT_TIMER_INTVERAL;
		if(light.transtime_elapsed >= light.transtime)/*transcation done*/
		{
			APP_Light_PRINTF("transcation done\r\n");
			light.transtime_elapsed = 0;
			light.transitioning = 0;
			
			light.current_r = light.target_r;
			light.current_g = light.target_g;
			light.current_b = light.target_b;
			light.current_c = light.target_c;
			light.current_w = light.target_w;
			
			BSP_LED_SetWYLevel(light.current_w, light.current_c);
			BSP_LED_SetRGBLevel(light.current_r, light.current_g, light.current_b);
		}
		else
		{
			if(light.target_r >= light.start_r)
			{
			    light.current_r = light.start_r + ((uint32_t)(light.target_r - light.start_r) * light.transtime_elapsed) / light.transtime;
			}	
			else
			{
			    light.current_r = light.start_r - ((uint32_t)(light.start_r - light.target_r) * light.transtime_elapsed) / light.transtime;
			}
			
			if(light.target_g >= light.start_g)
			{
				light.current_g = light.start_g + ((uint32_t)(light.target_g - light.start_g) * light.transtime_elapsed) / light.transtime;
			}
			else
			{
				light.current_g = light.start_g - ((uint32_t)(light.start_g - light.target_g) * light.transtime_elapsed) / light.transtime;
			}
			
			if(light.target_b >= light.start_b)
			{
				light.current_b = light.start_b + ((uint32_t)(light.target_b - light.start_b) * light.transtime_elapsed) / light.transtime;
			}
			else
			{	
				light.current_b = light.start_b - ((uint32_t)(light.start_b - light.target_b) * light.transtime_elapsed) / light.transtime;
			}
			
			if(light.target_c >= light.start_c)
			{
				light.current_c = light.start_c + ((uint32_t)(light.target_c - light.start_c) * light.transtime_elapsed) / light.transtime;
			}
			else
			{
				light.current_c = light.start_c - ((uint32_t)(light.start_c - light.target_c) * light.transtime_elapsed) / light.transtime;
			}
			
			if(light.target_w >= light.start_w)
			{
				light.current_w = light.start_w + ((uint32_t)(light.target_w - light.start_w) * light.transtime_elapsed) / light.transtime;
			}
			else
			{
				light.current_w = light.start_w - ((uint32_t)(light.start_w - light.target_w) * light.transtime_elapsed) / light.transtime;
			}
			
			BSP_LED_SetWYLevel(light.current_w, light.current_c);
			BSP_LED_SetRGBLevel(light.current_r, light.current_g, light.current_b);
		}
	}
}

/*****************************************************************************
Function name :  APP_Light_SetLevel
Description:	 设置灯的亮度    0-100
Input:	         TargetLevel->亮度目标值
	             usTransitionTime->转换时间，如果是0，则立即转换
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Light_SetLevel(U16 TargetLevel, U16 usTransitionTime)
{
	APP_Light_PRINTF("APP_Light_SetLevel TargetLevel = %d\r\n", TargetLevel);
	
	if(TargetLevel == 0)
	{
		light.onoff = OFF;
		light.lightness = 0;
	}
	else
	{
		light.onoff = ON;
		light.lightness = TargetLevel;
	}
	APP_Light_PRINTF("light.lightness %d\r\n", light.lightness);
	
	light_start_transition(usTransitionTime);
}

/*****************************************************************************
Function name :  APP_Light_GetLevel
Description:	 获取灯的亮度    
Input:	         0-100
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
U8 APP_Light_GetLevel(void)
{
	return light.lightness;
}

/*****************************************************************************
Function name :  APP_Light_SetOnOff
Description:	 设置灯的开关 
Input:	         onoff->目标值
	             usTransitionTime->转换时间，如果是0，则立即转换
Return:          
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Light_SetOnOff(U8 onoff, U16 usTransitionTime)
{
	APP_Light_PRINTF("APP_Light_SetOnoff Target = %d\r\n", onoff);
	
	if(onoff)
	{
	    light.onoff = ON;
	}
	else
	{
	    light.onoff = OFF;
	}
		
	light_start_transition(usTransitionTime);
}

/*****************************************************************************
Function name :  APP_Light_GetOnoff
Description:	 获取当前灯的开关状态
Input:	         void
Return:          0x00->OFF; 0x01->ON
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
U8 APP_Light_GetOnoff(void)
{
	return light.onoff;
}

/*****************************************************************************
Function name :  APP_Light_SetTemperature
Description:	 设置灯的色温    0-100
Input:	         TargetTemperature->色温目标值
	             usTransitionTime->转换时间，如果是0，则立即转换
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Light_SetTemperature(U16 TargetTemperature, S16 DeltaUV, U16 usTransitionTime)
{
	APP_Light_PRINTF("APP_Light_SetTemperature TargetTemperature = %d\r\n", TargetTemperature);
	
	if(TargetTemperature > YK_TEMPERATURE_MAX)
	{
	    light.temperature = YK_TEMPERATURE_MAX;
	}
	else if(TargetTemperature < YK_TEMPERATURE_MIN)
	{
	    light.temperature = YK_TEMPERATURE_MIN;
	}
	else
	{
	    light.temperature = TargetTemperature;
	}
	
	light.mode = CW_MODE;
	
	APP_Light_PRINTF("light.temperature%d\r\n", light.temperature);
	
	light_start_transition(usTransitionTime);
	
}

/*****************************************************************************
Function name :  APP_Light_GetTemperature
Description:	 获取当前灯的色温值
Input:	         void
Return:          2200K~7500K
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
U16 APP_Light_GetTemperature(void)
{
	return light.temperature;
}

#if (HW_RGB_ENABLE == 1)
/*****************************************************************************
Function name :  APP_Light_SetRGB
Description:	 设置灯的RGB    
Input:	         Target_R->R目标值
                 Target_G->G目标值
	             Target_B->B目标值
	             Level->RGB的亮度
	             usTransitionTime->转换时间，如果是0，则立即转换
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Light_SetRGB(U16 Target_R, U16 Target_G, U16 Target_B, U16 Level, U16 usTransitionTime)
{
	U8 ucPt = 0;
	APP_Light_PRINTF("APP_Light_SetRGB Target_R = %d, Target_G = %d, Target_B = %d\r\n", Target_R, Target_G, Target_B);
	
	light.mode = RGB_MODE;
	light.r    = Target_R;
	light.g    = Target_G;
	light.b    = Target_B;
	
	/*颜色校准*/
	for(ucPt = 0; ucPt < (sizeof(s_Contrast_RGB) / 3); ucPt++)
	{
		if(Target_R == s_Contrast_RGB[ucPt * 3] && Target_G == s_Contrast_RGB[ucPt * 3 + 1] && Target_B == s_Contrast_RGB[ucPt * 3 + 2])
		{
			APP_Light_PRINTF("RGB Calibration ucPt = %d\r\n", ucPt);
			
			light.r = s_Correction_RGB[ucPt * 3];
			light.g = s_Correction_RGB[ucPt * 3 + 1];
			light.b = s_Correction_RGB[ucPt * 3 + 2];

			break;
		}
	} 
	
	light_start_transition(usTransitionTime);
}

/*****************************************************************************
Function name :  Hue_2_RGB
Description:	 HSL->RGB  
Input:	         v1->
                 v2->
	             vH->
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static float Hue_2_RGB( float v1, float v2, float vH ) 
{
	if ( vH < 0 ) vH += 1;
	if ( vH > 1 ) vH -= 1;
	if (( 6.0 * vH ) < 1 ) return ( v1 + ( v2 - v1 ) * 6.0f * vH );
	if (( 2.0 * vH ) < 1 ) return ( v2 );
	if (( 3.0 * vH ) < 2 ) return ( v1 + ( v2 - v1 ) * ( ( 2.0f/3.0f ) - vH ) * 6.0f );
	return ( v1 );
}

/*****************************************************************************
Function name :  HSL_2_RGB
Description:	 HSL转换到RGB  
Input:	         v1->
                 v2->
	             vH->
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void HSL_2_RGB(U16 Target_H, U16 Target_S, U16 Target_L, U8 *pTarget_R, U8 *pTarget_G, U8 *pTarget_B)
{
    uint8_t Target_R;
	uint8_t Target_G;
	uint8_t Target_B;
    float H = (float)Target_H / 65535.0f;
	float S = (float)Target_S / 65535.0f;
	float L = (float)Target_L / 65535.0f;
	float R,G,B,var_1,var_2;
    
	if ( S == 0 )
	{
		R = L;
		G = L;
		B = L;
	}
	else
	{
		if ( L < 0.5 )
			var_2 = L * ( 1 + S );
		else
			var_2 = ( L + S ) - ( S * L );
		var_1 = 2 * L - var_2;
		R = Hue_2_RGB( var_1, var_2, H + ( 1.0f/3.0f ));
		G = Hue_2_RGB( var_1, var_2, H );
		B = Hue_2_RGB( var_1, var_2, H - ( 1.0f/3.0f ));
	}

	Target_R = (uint8_t)(R*255);
	Target_G = (uint8_t)(G*255);
	Target_B = (uint8_t)(B*255);

    *pTarget_R = Target_R;
    *pTarget_G = Target_G;
    *pTarget_B = Target_B;
}

/*****************************************************************************
Function name :  APP_Light_SetHSL
Description:	 设置灯的HSL  
Input:	         Target_H->色度
                 Target_S->饱和度
	             Target_L->亮度
	             usTransitionTime->转换时间，如果是0，则立即转换
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Light_SetHSL(U16 Target_H, U16 Target_S, U16 Target_L, U16 usTransitionTime)
{
    uint8_t Target_R;
	uint8_t Target_G;
	uint8_t Target_B;

    HSL_2_RGB(Target_H, Target_S, Target_L, &Target_R, &Target_G, &Target_B);

	APP_Light_PRINTF("APP_Light_SetHSL Target_R = %d, Target_G = %d, Target_B = %d\r\n", Target_R, Target_G, Target_B);
	
	light.mode = RGB_MODE;
	light.r    = Target_R;
	light.g    = Target_G;
	light.b    = Target_B;
    
    light_start_transition(usTransitionTime);
}



#endif

/*****************************************************************************
Function name :  APP_Light_Test
Description:	 测试灯的渐变
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Light_Test(void)
{
//	static U8 State = 0;
//	
//	if(gst_APPLightTB.ulDelayCnt > 0)
//    {
//        return;
//    }
//	//APP_Light_SetLevel(0, 200);
//	switch(State)
//	{
//		case 0:
//		{
////			APP_Light_SetRGB(255, 255, 255, 100, 0);
////			APP_Light_SetLevel(0, 0);
////			APP_Light_SetTemperature(0, 0, 0);
//            //APP_Light_SetLevel(100, 5000);
//			//BSP_SM2135EJ_SetWYLevel(232, 0);	
//			//BSP_SM2135EJ_SetRGBLevel(255, 0,  0);
//			gst_APPLightTB.ulDelayCnt = TmrSeconds(2);
//			
//		}break;
//		
//		case 1:
//		{
////			APP_Light_SetRGB(0, 0, 0, 100, 0);
////			APP_Light_SetLevel(0, 0);
////			APP_Light_SetTemperature(100, 0, 0);
//            //APP_Light_SetLevel(0, 5000);
//			//BSP_SM2135EJ_SetWYLevel(0, 232);	
//			//BSP_SM2135EJ_SetRGBLevel(0, 255,  0);
//			gst_APPLightTB.ulDelayCnt = TmrSeconds(2);
//			
//		}break;
//		
////		case 2:
////		{
////			BSP_SM2135EJ_SetRGBLevel(0, 0,  255);
////			gst_APPLightTB.ulDelayCnt = TmrSeconds(2);
////			
////		}break;			
//	}
//	
//	State++;
//	if(State > 1)
//	{
//		State = 0;
//	
//	}
//	
////	BSP_SM2135EJ_SetWYLevel(200, 100);	

}
	


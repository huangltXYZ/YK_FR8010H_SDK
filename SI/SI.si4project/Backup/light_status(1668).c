#include "YK_APP.h"

LIGHT_STATUS LightStatus;

static U16 Constrain_Value(U16 Value, U16 Min, U16 Max);

/*****************************************************************************
Function name :  Constrain_Value
Description:	 Լ��light��״ֵ̬
Input:	         Value->�����״ֵ̬
                 Min->��Сֵ
				 Max->���ֵ
Return:          Լ�����ֵ
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static U16 Constrain_Value(U16 Value, U16 Min, U16 Max)
{
	U16 Out;
	if(Value < Min)
	{
		Out = Min;
	}
	else if(Value > Max)
	{
		Out = Max;
	}
	else
	{
	    Out = Value;
	}
	
	return Out;
}

/*****************************************************************************
Function name :  Light_Default_Status_Init
Description:	 �ϵ��ʼ��lightĬ�ϲ���
Input:	         void
Return:          void
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void Light_Default_Status_Init(void)
{
	/*���Ƚ�MEMģ������ݵ��뵽light status��*/
	LightStatus.dtt = MEM_DB_MESH.dtt;
	
	LightStatus.onpowerup = MEM_DB_MESH.onpowerup;
	
	LightStatus.lightness.def       = MEM_DB_MESH.lightness_default;
    LightStatus.lightness.last      = MEM_DB_MESH.lightness_last;
	LightStatus.lightness.target    = MEM_DB_MESH.lightness_target;
	LightStatus.lightness.range_min = MEM_DB_MESH.lightness_range_min;
	LightStatus.lightness.range_max = MEM_DB_MESH.lightness_range_max;
	
	LightStatus.temp.def          = MEM_DB_MESH.temp_default;
	LightStatus.temp.target       = MEM_DB_MESH.temp_target;	
	LightStatus.temp.range_min    = MEM_DB_MESH.temp_range_min;
	LightStatus.temp.range_max    = MEM_DB_MESH.temp_range_max;
	LightStatus.temp.delta_def    = MEM_DB_MESH.duv_default;
	LightStatus.temp.delta_target = MEM_DB_MESH.duv_target;
	
	LightStatus.hue.target    = MEM_DB_MESH.hue_target;
	LightStatus.hue.range_min = MEM_DB_MESH.hue_range_min;
	LightStatus.hue.range_max = MEM_DB_MESH.hue_range_max;
	
	LightStatus.saturation.target    = MEM_DB_MESH.saturation_target;
	LightStatus.saturation.range_min = MEM_DB_MESH.saturation_range_min;
	LightStatus.saturation.range_max = MEM_DB_MESH.saturation_range_max;	
	
	LightStatus.lightness.last = LightStatus.lightness.last > 0 ? 
	                             Constrain_Value(LightStatus.lightness.last, LightStatus.lightness.range_min, LightStatus.lightness.range_max) : 0;

	LightStatus.lightness.def = LightStatus.lightness.def > 0 ? 
	                             Constrain_Value(LightStatus.lightness.def, LightStatus.lightness.range_min, LightStatus.lightness.range_max) : 0;
								 
	LightStatus.lightness.target = LightStatus.lightness.target > 0 ? 
	                             Constrain_Value(LightStatus.lightness.target, LightStatus.lightness.range_min, LightStatus.lightness.range_max) : 0;
    
	LightStatus.temp.def = Constrain_Value(LightStatus.temp.def, LightStatus.temp.range_min, LightStatus.temp.range_max);							 
	LightStatus.temp.target = Constrain_Value(LightStatus.temp.target, LightStatus.temp.range_min, LightStatus.temp.range_max);									 
}








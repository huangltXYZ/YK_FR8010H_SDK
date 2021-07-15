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
Function name :  Light_Status_Default_Init
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
void Light_Status_Default_Init(void)
{
	/*���Ƚ�MEMģ������ݵ��뵽light status��*/
	LightStatus.dtt = MEM_DB_MESH.dtt;
	
	LightStatus.onpowerup = MEM_DB_MESH.onpowerup;
	
	LightStatus.lightness.def       = MEM_DB_MESH.lightness_default;
    LightStatus.lightness.last      = MEM_DB_MESH.lightness_last;
	LightStatus.lightness.target    = MEM_DB_MESH.lightness_target;
	LightStatus.lightness.range_min = MEM_DB_MESH.lightness_range_min;
	LightStatus.lightness.range_max = MEM_DB_MESH.lightness_range_max;
    LightStatus.lightness.delta      = MEM_DB_MESH.lightness_delta;

    
	
	LightStatus.temp.def          = MEM_DB_MESH.temp_default;
	LightStatus.temp.target       = MEM_DB_MESH.temp_target;	
	LightStatus.temp.range_min    = MEM_DB_MESH.temp_range_min;
	LightStatus.temp.range_max    = MEM_DB_MESH.temp_range_max;
	LightStatus.temp.delta_def    = MEM_DB_MESH.duv_default;
	LightStatus.temp.delta_target = MEM_DB_MESH.duv_target;
	
	LightStatus.hue.target     = MEM_DB_MESH.hue_target;
	LightStatus.hue.range_min = MEM_DB_MESH.hue_range_min;
	LightStatus.hue.range_max = MEM_DB_MESH.hue_range_max;
	
	LightStatus.saturation.target     = MEM_DB_MESH.saturation_target;
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

    switch(LightStatus.onpowerup)
    {
        case GENERIC_ON_POWERUP_OFF:
        {
            LightStatus.lightness.current = 0;

        }break;
        
        case GENERIC_ON_POWERUP_DEFAULT:
        {
            if(LightStatus.lightness.def == 0U)
            {
                LightStatus.lightness.current = LightStatus.lightness.last;
            }
            else
            {
                LightStatus.lightness.current = LightStatus.lightness.def;
            }

        }break;     

        case GENERIC_ON_POWERUP_RESTORE:
        {
            LightStatus.lightness.current = LightStatus.lightness.target == 0 ? 
                LightStatus.lightness.range_min : LightStatus.lightness.target;

            LightStatus.temp.current = LightStatus.temp.target;    
            LightStatus.temp.delta_current = LightStatus.temp.delta_target;    

            LightStatus.hue.current = LightStatus.hue.target;
            LightStatus.saturation.current = LightStatus.saturation.target;

            LightStatus.lightness.last = LightStatus.lightness.current;

        }break;
    }

    LightStatus.lightness.target = LightStatus.lightness.current;
    LightStatus.temp.target = LightStatus.temp.current;
    LightStatus.temp.delta_target = LightStatus.temp.delta_current;    
    LightStatus.hue.target = LightStatus.hue.current;
    LightStatus.saturation.target = LightStatus.saturation.current;

    if(gst_AppMemDB.OTAWithOff)
    {
        LightStatus.lightness.target = 0;
        LightStatus.lightness.current = 0;

        gst_AppMemDB.OTAWithOff = 0;
        APP_MEM_Fast_Save();
    }
}


/*****************************************************************************
Function name :  Light_Status_Set_Target
Description:	 ����Ŀ��״̬
Input:	         Type->״̬����;pTargetĿ��ֵ
Return:          void
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void Light_Status_Set_Target(LIGHT_STATUS_TYPE Type, void *pTarget)
{
    if(Type >= LIGHT_STATUS_TYPE_MAX)
    {
        return;
    }

    switch(Type)
    {
        case LIGHT_STATUS_TYPE_ONOFF:
        {
            if(*((bool *) pTarget))
            {
                LightStatus.lightness.target = LightStatus.lightness.last; 
            }
            else
            {
                LightStatus.lightness.target = 0;
            }
            LightStatus.lightness.current = LightStatus.lightness.target;
			
			MESH_MODEL_LOG("LightStatus.lightness.target = %d  LightStatus.lightness.current = %d\r\n", LightStatus.lightness.target, LightStatus.lightness.current);
            
        }break;

        default:
            break;

    }

    if((Type != LIGHT_STATUS_TYPE_ONOFF)&&
       (LightStatus.onpowerup == GENERIC_ON_POWERUP_RESTORE) &&
       (LightStatus.lightness.target >= LIGHTNESS_MIN))
    {
        if(LightStatus.lightness.last != LightStatus.lightness.target)
        {
            LightStatus.lightness.last = LightStatus.lightness.target;
        }
        
        APP_MEM_Slow_Save();
    }
}

/*****************************************************************************
Function name :  Light_Status_Get_Current
Description:	 ��ȡ��ǰ״̬
Input:	         Type->״̬����
Return:          ��Ӧ���͵ĵ�ǰֵ
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
S32 Light_Status_Get_Current(LIGHT_STATUS_TYPE Type)
{
    S32 Current;
    if(Type >= LIGHT_STATUS_TYPE_MAX)
    {
        return INFINITE;
    }

    switch(Type)
    {
        case LIGHT_STATUS_TYPE_ONOFF:
        {
            if(LightStatus.lightness.current)
            {
                Current = 1; 
            }
            else
            {
                Current = 0;
            }

        }break;

        default:
            break;

    } 

    return Current;
}


/*****************************************************************************
Function name :  Light_Status_Get_Target
Description:	 ��ȡĿ��״̬
Input:	         Type->״̬����
Return:          ��Ӧ���͵�Ŀ��ֵ
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
S32 Light_Status_Get_Target(LIGHT_STATUS_TYPE Type)
{
    S32 Target;
    if(Type >= LIGHT_STATUS_TYPE_MAX)
    {
        return INFINITE;
    }

    switch(Type)
    {
        case LIGHT_STATUS_TYPE_ONOFF:
        {
            if(LightStatus.lightness.target)
            {
                Target = 1; 
            }
            else
            {
                Target = 0;
            }

        }break;

        default:
            break;

    } 

    return Target;
}


/*****************************************************************************
Function name :  Light_Status_Save
Description:	 ��ȡĿ��״̬
Input:	         Type->״̬����
Return:          ��Ӧ���͵�Ŀ��ֵ
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void Light_Status_Save(void)
{
    MEM_DB_MESH.dtt = LightStatus.dtt;
	
	MEM_DB_MESH.onpowerup = LightStatus.onpowerup;
	
    MEM_DB_MESH.lightness_default   = LightStatus.lightness.def;
    MEM_DB_MESH.lightness_last      = LightStatus.lightness.last;
    MEM_DB_MESH.lightness_target    = LightStatus.lightness.target;
    MEM_DB_MESH.lightness_range_min = LightStatus.lightness.range_min;
    MEM_DB_MESH.lightness_range_max = LightStatus.lightness.range_max;
    
    MEM_DB_MESH.temp_default   = LightStatus.temp.def;
    MEM_DB_MESH.temp_target    = LightStatus.temp.target;
    MEM_DB_MESH.temp_range_min = LightStatus.temp.range_min;
    MEM_DB_MESH.temp_range_max = LightStatus.temp.range_max;
    MEM_DB_MESH.duv_default    = LightStatus.temp.delta_def;
    MEM_DB_MESH.duv_target     = LightStatus.temp.delta_target;
    
    MEM_DB_MESH.hue_target    = LightStatus.hue.target;
    MEM_DB_MESH.hue_range_min = LightStatus.hue.range_min;
    MEM_DB_MESH.hue_range_max = LightStatus.hue.range_max;
    
    MEM_DB_MESH.saturation_target    = LightStatus.saturation.target;
    MEM_DB_MESH.saturation_range_min = LightStatus.saturation.range_min;
    MEM_DB_MESH.saturation_range_max = LightStatus.saturation.range_max;	

    APP_MEM_Slow_Save();
}




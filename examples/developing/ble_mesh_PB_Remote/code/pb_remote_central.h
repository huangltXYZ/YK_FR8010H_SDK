/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */
#ifndef BLE_SIMPLE_CENTRAL_H
#define BLE_SIMPLE_CENTRAL_H

 /*
 * INCLUDES (包含头文件)
 */
 #include "gap_api.h"
/*
 * MACROS (宏定义)
 */

/*
 * CONSTANTS (常量定义)
 */


/*
 * TYPEDEFS (类型定义)
 */


/*
 * GLOBAL VARIABLES (全局变量)
 */


/*
 * LOCAL VARIABLES (本地变量)
 */
 
/*
 * LOCAL FUNCTIONS (本地函数)
 */

/*
 * EXTERN FUNCTIONS (外部函数)
 */

/*
 * PUBLIC FUNCTIONS (全局函数)
 */

void pb_remote_connected(uint8_t conidx);
void pb_remote_central_init(void);

#endif


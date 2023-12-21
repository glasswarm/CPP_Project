#ifndef __PPP_PROTOCOL_DECODE_H__
#define __PPP_PROTOCOL_DECODE_H__


//#include <stdio.h>
//#include <string.h>
#include <cstdio>
#include <cstring>
#include <iostream>

//-----------------------------------------
//用来设置数组大小,和形参大小，尽量设置大些，防止数据溢出
#define PPP_Decode_digit 1024


//-----------------------------------------
//定义的函数
int            New_Buffer();                                                              //分离buffer用函数
void           receive_separate   (unsigned char PPP_Recive_Buffer[2 * PPP_Decode_digit]);//调试用
unsigned char *PPP_Protocol_Decode(unsigned char input_array      [2 * PPP_Decode_digit]);//解码分离后的数据帧

//-----------------------------------------
//接收数组声明
extern unsigned char New_receive      [2 * PPP_Decode_digit];//刷新buffer数组声明
extern unsigned char PPP_Recive_single[2 * PPP_Decode_digit];//接收数组声明


#endif

//-------------------------------------------------------------------------------------------------------------
//DeCode(Decode_Receive)数据格式
/*
 * 0x7E -seq- ---还原数据--- 0x7E
 *
*/
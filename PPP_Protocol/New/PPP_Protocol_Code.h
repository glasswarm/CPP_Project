#ifndef __PPP_PROTOCOL_CODE_H__
#define __PPP_PROTOCOL_CODE_H__

#include <stdio.h>
#include <string.h>

//-----------------------------------------
//用来设置数组大小,尽量设置大些，防止数据溢出
#define PPP_Code_digit 1024

//-----------------------------------------
//输入数组的十位和个位的大小，例如example[input_sample_decade][input_sample_digit]
#define input_sample_decade 188
#define input_sample_digit  120

#define input_decade input_sample_decade
#define input_digit  input_sample_digit

void test_init();
unsigned char *PPP_Protocol_Code(unsigned char input_array[input_sample_decade][input_sample_digit]);//将摄像头数组打包成PPP数据

#endif

//-------------------------------------------------------------------------------------------------------------
//Code(PPP_Send)数据格式
/*
 * 0x7D 0x8E 200 ---百位内位置数据--- 101 xx -百位以上位置数据- || 0x7D 0x8D 200 ---百位内位置数据--- 101 xx -百位以上位置数据- ||
 * 0x7D 0x00 200 ---百位内位置数据--- 101 xx -百位以上位置数据- || 0x7D seq  200 ---百位内位置数据--- 101 xx -百位以上位置数据- ||
 * 0x7E   -seq-  ---常态数据--- 0x7E
 *
*/

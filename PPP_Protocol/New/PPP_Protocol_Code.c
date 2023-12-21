#include "PPP_Protocol_Code.h"

//------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------
//input_Data部分
unsigned char test[input_decade][input_digit];
int  test_T_Count,test_F_Count;

void test_init()
{
     for(test_T_Count = 0; test_T_Count < 94 ; test_T_Count++)   //例子数组180*120
         for(test_F_Count = 0; test_F_Count < input_digit ; test_F_Count++)
             test[test_T_Count][test_F_Count] = 0x78;

//    for(test_T_Count = 0; test_T_Count < input_decade ; test_T_Count++)   //例子数组180*120
//    {
//        for(test_F_Count = 0; test_F_Count < 60 ; test_F_Count++)
//            test[test_T_Count][test_F_Count] = 0x7D;
//        for(test_F_Count = 60; test_F_Count < input_digit ; test_F_Count++)
//            test[test_T_Count][test_F_Count] = 0x79;
//
//    }


//     test[0][20] = 0x7E; test[2][22] = 0x7E; test[8][55] = 0x7E;
//     test[2][24] = 0x7D; test[7][21] = 0x7D;
////     test[5][23] = 0x00; test[6][25] = 0x00;
//     test[0][60] = 0x20; test[3][63] = 0x20;


//     test[20][20] = 0x7E; test[18][22] = 0x7E; test[16][55] = 0x7E;
//     test[21][24] = 0x7D; test[27][21] = 0x7D;
//     test[25][23] = 0x00; test[26][25] = 0x00;
//     test[20][60] = 0x20; test[23][63] = 0x20;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------
//PPP数组分配大小
char    PPP_Special        [4][2 * PPP_Code_digit]; //7E,7D,00,Seq
char    PPP_Normal            [2 * PPP_Code_digit]; //增加一位，防止在末尾出现需要将，例如0x7E拆分成0x7D
char    PPP_Send              [2 * PPP_Code_digit];


//------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------
//PPP数组初始化
int  PPP_Special_digit  [4]; //7E,7D,00,Seq
int  record_Special     [4]; //7E,7D,00,Seq

char PPP_Special_Init   [3] = {0x7D,0x10,0xC8};//帧头，加的基数，百位内代表
char PPP_Special_Init_1_[4] = {0x7E,0x7D,0x00,0x20};//7E,7D,00,Seq的[1]的加数

char Sequence = 0x20;//帧序列号

//------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------
//input_Data计数
int  decade;
int  digit ;

//------------------------------------
//PPP_Data计数
int  PPP_digit       ;
int  PPP_Normal_digit;

//------------------------------------
//表示一帧传输完毕
int  flag;

void Code_init()
{
     int A,B;
     for(A = 0; A < 4; A++) //为每个帧赋帧头帧尾
         for(B =0; B < 3; B++) 
             PPP_Special[A][B] = PPP_Special_Init[B];

     for(A = 0; A < 4; A++) //增加数0x10
     {
         if(A == 3)PPP_Special[A][1] = Sequence; //序列
         else PPP_Special[A][1] += PPP_Special_Init_1_[A];
     }

     for(A = 0; A < 4; A++) PPP_Special_digit[A] = 3;
          
     PPP_Normal[0] = 0x7E; //帧头0x7E
     PPP_Normal[1] = Sequence; //帧序列
     PPP_Normal_digit = 2;
}



void PPP_Special_Code_Process(int r_Special)
{
     if (record_Special[r_Special] < PPP_digit / 100)//记录是否进位100
     {
         PPP_Special[r_Special][ PPP_Special_digit[r_Special]   ] = 101; //因为在此设定特殊字符的标号不可能大于100，所以以101为进了100位数的标志
         PPP_Special[r_Special][ PPP_Special_digit[r_Special]+1 ] = PPP_digit / 100;
         PPP_Special_digit[r_Special] += 2;
     }
     record_Special[r_Special] = PPP_digit/100;
     PPP_Special[r_Special][ PPP_Special_digit[r_Special] ] = ( !(PPP_digit % 100) ) ? 200 : (PPP_digit % 100);//字符串在遇到0时会截止，所以需要将0变为200
     PPP_Special_digit[r_Special]++;
}

void splice()//拼接数组
{
     int record_pointer = 0;
	 memcpy(PPP_Send                 , PPP_Special[0], PPP_Special_digit[0]); record_pointer += PPP_Special_digit[0];
     memcpy(PPP_Send + record_pointer, PPP_Special[1], PPP_Special_digit[1]); record_pointer += PPP_Special_digit[1];
     memcpy(PPP_Send + record_pointer, PPP_Special[2], PPP_Special_digit[2]); record_pointer += PPP_Special_digit[2];
     memcpy(PPP_Send + record_pointer, PPP_Special[3], PPP_Special_digit[3]); record_pointer += PPP_Special_digit[3];
     memcpy(PPP_Send + record_pointer, PPP_Normal    , PPP_Normal_digit    );
     //strcat(PPP_Send,PPP_Special[0]);
}

void Clear_Code_array()//清空数组
{
     memset(&PPP_Send      , 0, sizeof(PPP_Send      ));//sizeof是定义这个数组的大小,而不是遇到0的大小
     memset(&PPP_Normal    , 0, sizeof(PPP_Normal    ));
     memset(&PPP_Special[0], 0, sizeof(PPP_Special[0]));
     memset(&PPP_Special[1], 0, sizeof(PPP_Special[1]));
     memset(&PPP_Special[2], 0, sizeof(PPP_Special[2]));
     memset(&PPP_Special[3], 0, sizeof(PPP_Special[3]));
     
}

//int Count = 0;

unsigned char *PPP_Protocol_Code(unsigned char input_array[input_sample_decade][input_sample_digit])
{
//------------------------------------------------
//清空,初始化数组
     Clear_Code_array();
     Code_init();

     for(PPP_digit = 2; PPP_digit < PPP_Code_digit-1; PPP_digit++)
     {
//-----------------------------------
//当decade = input_decade时，代表一张图片已经传完，使decade，序列号变复位，同时跳出循环，开始下一张图片的传输
         if (decade == input_decade)
         {
             printf("digit is %d\n",digit);
             decade = 0;
             Sequence = 0x1F;//0x1F + 1 = 0x20,即恢复了0x20的数据
             flag = 1;//表示一帧传输完毕
             break;
         }
//-----------------------------------
//特殊字符
         if      (input_array[decade][digit] == 0x7E)     PPP_Special_Code_Process(0);
         else if (input_array[decade][digit] == 0x7D)     PPP_Special_Code_Process(1);
         else if (input_array[decade][digit] == 0x00)     PPP_Special_Code_Process(2);
         else if (input_array[decade][digit] == Sequence) PPP_Special_Code_Process(3);
//-----------------------------------
//常规字符数组
         else
         {
             PPP_Normal[PPP_Normal_digit] = input_array[decade][digit];
             PPP_Normal_digit++;
         }

         decade = (digit == input_digit - 1 ) ? ++decade :  decade;
         digit  = (digit == input_digit - 1 ) ? 0        : ++digit;
     }
//-----------------------------------
// 补帧尾
     Sequence++;//帧序列号自增
     PPP_Normal[PPP_Normal_digit] = 0x7E; //for在结束时正好+1，帧尾0x7E
     PPP_Normal_digit++;
//--------------------------------------------------
//拼接数组
     splice();
     return (unsigned char *)PPP_Send;
}

//---------------------------------------
//example main
// int main()
// {
//     test_init();
//     while(!flag)
//     {
//         PPP_Protocol_Code(test);
//         printf("I\'m head，%s，I\'m tail \n",PPP_Send);
//     }
//     return 0;
// }
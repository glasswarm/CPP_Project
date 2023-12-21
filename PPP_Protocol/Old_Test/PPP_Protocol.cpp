#include <iostream>
#include <cstdio>
#include <cstring>

//using namespace std;


//------------------------------------
//input_Data部分
#define input_decade 180
#define input_digit  120
unsigned char test[input_decade][input_digit];

//------------------------------------
//PPP_Data部分
#define PPP_Count 1024
char    PPP_Send[PPP_Count+1]; //增加一位，防止在末尾出现需要将，例如0x7E拆分成0x7D
char    PPP_TXT[2*PPP_Count];


int  test_T_Count,test_F_Count;
void PPP_init()
{
    for(test_T_Count = 0; test_T_Count < input_decade ; test_T_Count++)   //例子数组180*120
        for(test_F_Count = 0; test_F_Count < input_digit ; test_F_Count++)
            test[test_T_Count][test_F_Count] = 0x79;
    test[0][20] = 0x7D;
    test[0][21] = 0x7D;
    test[0][22] = 0x7D;
    test[0][23] = 0x7D;
    test[0][24] = 0x7D;
    test[0][25] = 0x7D;

    test[8][55] = 0x7D;
}

//------------------------------------
//input_Data部分
int  decade         = 0;
int  digit          = 0;

//------------------------------------
//PPP_Data部分
int  PPP_digit      = 0;
int  Sequence_Count = 0x30;//帧序列号

//------------------------------------
//最后结尾是特殊字符还是普通字符的标志flag
int flag = 0;
void PPP_Protocol(unsigned char **input_Data)
{
     memset(&PPP_Send, 0, sizeof(PPP_Send));//清空数组，用0填充，其实可要可不要，因为PPP协议只看第一个0x7E和第二个0x7E
     //char PPP_Send[PPP_Count+1]; //定义局部数组，因为数组在初始化后就无法回归未初始化状态，所以选择局部变量，在结束以后释放内存，回归未初始化
     PPP_Send[0] = 0x7E; //帧头0x7E
     PPP_Send[1] = Sequence_Count; //帧序列

     for(PPP_digit = 2; PPP_digit < PPP_Count-1; PPP_digit++)
     {
         decade = (digit == input_digit - 1) ? ++decade :  decade;
         digit  = (digit == input_digit - 1) ? 0        : ++digit;
         if (decade == input_decade)//当decade = input_decade时，代表一张图片已经传完，使decade，序列号变复位，同时跳出循环，开始下一张图片的传输
         {
             decade = 0;
             Sequence_Count = 0x10;
             //PPP_digit += 1;//没有了for结尾的+1，跳出循环帧尾的序列号需要+1
             std::cout << "1" << std::endl;
             flag = 1;//表示一帧传输完毕
             break;

         }
         //std::cout << "1" << std::endl;
         PPP_Send[PPP_digit] = test[decade][digit];
         //std::cout << "ppp_digit is " << PPP_digit << ", decade is " << decade << ", digit is " << digit << std::endl;
//-----------------------------------
//特殊字符
         if (PPP_Send[PPP_digit] == 0x7E)
         {
             PPP_Send[PPP_digit]     = 0x7D;
             PPP_Send[PPP_digit + 1] = 0x7E + 0x10;
             PPP_digit += 1;
             //std::cout << PPP_Send << std::endl;
         }
         else if (PPP_Send[PPP_digit] == 0x7D)
         {
             PPP_Send[PPP_digit]     = 0x7D;
             PPP_Send[PPP_digit + 1] = 0x7D - 0x10;
             PPP_digit += 1;
         }
         else if (PPP_Send[PPP_digit] == Sequence_Count)
         {
             PPP_Send[PPP_digit]     = 0x7D;
             PPP_Send[PPP_digit + 1] = Sequence_Count + 0x10; //PPP默认减0x20，但在这里我减了0x10
             PPP_digit += 1;
         }
         else if (PPP_Send[PPP_digit] == 0x7D)
         {
             PPP_Send[PPP_digit]     = 0x7D;
             PPP_Send[PPP_digit + 1] = 0x00 + 0x10; //PPP默认减0x20，但在这里我减了0x10
             PPP_digit += 1;
         }
     }
     Sequence_Count++;//帧序列号自增
     PPP_Send[PPP_digit] = 0x7E; //for在结束时正好+1，帧尾0x7E
     int c = sprintf(PPP_TXT,"%s",PPP_Send);
     std::cout << PPP_digit << std::endl;
     std::cout << strlen(PPP_Send) << std::endl;
     std::cout << PPP_Send[1022] << PPP_Send[1023] << PPP_Send[1024] << std::endl;//<< PPP_Send[1025] << PPP_Send[1026] << PPP_Send[1027] << PPP_Send[1028]<< std::endl;

     std::cout << PPP_Send << std::endl;
}



int  main() {
     std::cout << "Hello, World!" << std::endl;
     PPP_init();
     for(int i = 0;i < 48;i++)
     {
         PPP_Protocol((unsigned char **)&test);
         //int c = sprintf(PPP_TXT,"%s",PPP_Send);
         //std::cout << PPP_TXT << std::endl;
         //sprintf(PPP_TXT,"%d",PPP_Send[1]);
         //std::cout << PPP_TXT << std::endl;
         //std::cout << PPP_digit << std::endl;
     }
     //char TXT[3] = {60,0,50};
     //int c = sprintf(PPP_TXT,"%s",TXT);
     //std::cout << PPP_TXT << std::endl;
     ////sprintf(PPP_TXT,"%d",PPP_Send[23]);
     //std::cout << c << std::endl;

     return 0;
}

































#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>

//using namespace std;


//------------------------------------
//input_Data部分
#define input_decade 180
#define input_digit  120
unsigned char test[input_decade][input_digit];

//------------------------------------
//PPP_Data部分
#define PPP_Count 1024
char    PPP_Normal[2*PPP_Count]; //增加一位，防止在末尾出现需要将，例如0x7E拆分成0x7D
char    PPP_Send[2*PPP_Count];
char    PPP_TXT[2*PPP_Count];

char    PPP_Special_7E[2*PPP_Count];
int     PPP_Special_7E_digit = 3;
int     record_7E;

char    PPP_Special_7D[2*PPP_Count];
int     PPP_Special_7D_digit;
char    record_7D;

char    PPP_Special_00[2*PPP_Count];
int     PPP_Special_00_digit;
char    record_00;

char    PPP_Special_Sequence[2*PPP_Count];
int     PPP_Special_Sequence_digit;
char    record_Sequence;

int     Sequence = 0x20;//帧序列号
int  test_T_Count,test_F_Count;


void PPP_init()
{
    for(test_T_Count = 0; test_T_Count < input_decade ; test_T_Count++)   //例子数组180*120
        for(test_F_Count = 0; test_F_Count < input_digit ; test_F_Count++)
            test[test_T_Count][test_F_Count] = 0x78;

    test[0][20] = 0x7E; test[2][22] = 0x7E; test[8][55] = 0x7E;
    test[2][24] = 0x7D; test[7][21] = 0x7D;
    test[5][23] = 0x00; test[6][25] = 0x00;
    test[0][60] = 0x20; test[3][63] = 0x20;


    unsigned char Init[3] = {0x7D,0x10,0xC8};
    for(int i=0; i < 3; i++)
    {
        PPP_Special_7E      [i] = Init[i];
        PPP_Special_7D      [i] = Init[i];
        PPP_Special_00      [i] = Init[i];
        PPP_Special_Sequence[i] = Init[i];
    }
    PPP_Special_7E      [1] +=     0x7E;
    PPP_Special_7D      [1] +=     0x7D;
    PPP_Special_00      [1] +=     0x00;
    PPP_Special_Sequence[1]  =     0x20;

    PPP_Special_7E_digit       = 3;
    PPP_Special_7D_digit       = 3;
    PPP_Special_00_digit       = 3;
    PPP_Special_Sequence_digit = 3;
}

//PPP_Send;
//0x7D 0x8E 200 ---数据--- 101 xx -数据- | 0x7D 0x8D 200 ---数据--- 101 xx -数据- | 0x7E ---数据--- 0x7ED

//------------------------------------
//input_Data部分
int  decade         = 0;
int  digit          = 0;

//------------------------------------
//PPP_Data部分
int  PPP_digit        = 0;
int  PPP_Normal_digit = 2;

//------------------------------------
//最后结尾是特殊字符还是普通字符的标志flag
int flag = 0;
void PPP_Protocol_Code(unsigned char **input_Data)
{
     memset(&PPP_Normal, 0, sizeof(PPP_Normal));//清空数组，用0填充，其实可要可不要，因为PPP协议只看第一个0x7E和第二个0x7E
     //char PPP_Send[PPP_Count+1]; //定义局部数组，因为数组在初始化后就无法回归未初始化状态，所以选择局部变量，在结束以后释放内存，回归未初始化
     PPP_Normal[0] = 0x7E; //帧头0x7E
     PPP_Normal[1] = Sequence; //帧序列



     for(PPP_digit = 2; PPP_digit < PPP_Count-1; PPP_digit++)
     {
         decade = (digit == input_digit - 1) ? ++decade :  decade;
         digit  = (digit == input_digit - 1) ? 0        : ++digit;
//-----------------------------------
//当decade = input_decade时，代表一张图片已经传完，使decade，序列号变复位，同时跳出循环，开始下一张图片的传输
         if (decade == input_decade)
         {
             decade = 0;
             Sequence = 0x20;
             std::cout << "1" << std::endl;
             flag = 1;//表示一帧传输完毕
             break;
         }

//-----------------------------------
//特殊字符
         if (test[decade][digit] == 0x7E)
         {
             if (record_7E < PPP_digit / 100)//记录是否进位100
             {
                 PPP_Special_7E[PPP_Special_7E_digit   ] =              101;//因为在此设定特殊字符的标号不可能大于100，所以以101为进了100位数的标志
                 PPP_Special_7E[PPP_Special_7E_digit + 1] = PPP_digit / 100;
                 PPP_Special_7E_digit += 2;
             }
             record_7E = PPP_digit/100;

             PPP_Special_7E[PPP_Special_7E_digit] = ( !(PPP_digit % 100) ) ? 200 : (PPP_digit % 100);//字符串在遇到0时会截止，所以需要将0变为200
             PPP_Special_7E_digit++;
         }

         else if (test[decade][digit] == 0x7D)
         {
             if (record_7D < PPP_digit / 100)//记录是否进位100
             {
                 PPP_Special_7D[PPP_Special_7D_digit    ] =             101;//因为在此设定特殊字符的标号不可能大于100，所以以101为进了100位数的标志
                 PPP_Special_7D[PPP_Special_7D_digit + 1] = PPP_digit / 100;
                 PPP_Special_7D_digit += 2;
             }
             record_7D = PPP_digit/100;

             PPP_Special_7D[PPP_Special_7D_digit] = ( !(PPP_digit % 100) ) ? 200 : (PPP_digit % 100);//字符串在遇到0时会截止，所以需要将0变为200
             PPP_Special_7D_digit++;
         }

         else if (test[decade][digit] == 0x00)
         {
             if (record_00 < PPP_digit / 100)//记录是否进位100
             {
                 PPP_Special_00[PPP_Special_00_digit    ] =             101;//因为在此设定特殊字符的标号不可能大于100，所以以101为进了100位数的标志
                 PPP_Special_00[PPP_Special_00_digit + 1] = PPP_digit / 100;
                 PPP_Special_00_digit += 2;
             }
             record_00 = PPP_digit / 100;

             PPP_Special_00[PPP_Special_00_digit] = ( !(PPP_digit % 100) ) ? 200 : (PPP_digit % 100);//字符串在遇到0时会截止，所以需要将0变为200
             PPP_Special_00_digit++;
         }

         else if (test[decade][digit] == Sequence)
         {
             if (record_Sequence < PPP_digit / 100)//记录是否进位100
             {
                 PPP_Special_Sequence[PPP_Special_Sequence_digit    ] =             101;//因为在此设定特殊字符的标号不可能大于100，所以以101为进了100位数的标志
                 PPP_Special_Sequence[PPP_Special_Sequence_digit + 1] = PPP_digit / 100;
                 PPP_Special_Sequence_digit += 2;
             }
             record_Sequence = PPP_digit / 100;

             PPP_Special_Sequence[PPP_Special_Sequence_digit] = ( !(PPP_digit % 100) ) ? 200 : (PPP_digit % 100);//字符串在遇到0时会截止，所以需要将0变为200
             PPP_Special_Sequence_digit++;
         }
         else
         {
             PPP_Normal[PPP_Normal_digit] = test[decade][digit];
             PPP_Normal_digit++;
         }

         //std::cout << "ppp_digit is " << PPP_digit << ", decade is " << decade << ", digit is " << digit << std::endl;

     }
     Sequence++;//帧序列号自增
     PPP_Normal[PPP_Normal_digit] = 0x7E; //for在结束时正好+1，帧尾0x7E

     //--------------------------------------------------
     sprintf(PPP_TXT,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",PPP_Special_7E[0],PPP_Special_7E[1],PPP_Special_7E[2],PPP_Special_7E[3],PPP_Special_7E[4],PPP_Special_7E[5],PPP_Special_7E[6],PPP_Special_7E[7],PPP_Special_7E[8],PPP_Special_7E[9]);
     std::cout << "PPP_Special_7E is   " <<  PPP_TXT << std::endl;
     int cc = sprintf(PPP_TXT,"%s",PPP_Special_7E);
     //std::cout << PPP_TXT << std::endl;
     //std::cout << PPP_Special_7E << std::endl;
     std::cout << cc << std::endl;

     sprintf(PPP_TXT,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",PPP_Special_7D[0],PPP_Special_7D[1],PPP_Special_7D[2],PPP_Special_7D[3],PPP_Special_7D[4],PPP_Special_7D[5],PPP_Special_7D[6],PPP_Special_7D[7],PPP_Special_7D[8],PPP_Special_7D[9]);
     std::cout << "PPP_Special_7D is   " <<  PPP_TXT << std::endl;
     std::cout << strlen(PPP_Special_7D) << std::endl;

     sprintf(PPP_TXT,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",PPP_Special_00[0],PPP_Special_00[1],PPP_Special_00[2],PPP_Special_00[3],PPP_Special_00[4],PPP_Special_00[5],PPP_Special_00[6],PPP_Special_00[7],PPP_Special_00[8],PPP_Special_00[9]);
     std::cout << "PPP_Special_00 is   " <<  PPP_TXT << std::endl;
     std::cout << strlen(PPP_Special_00) << std::endl;

     sprintf(PPP_TXT,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",PPP_Special_Sequence[0],PPP_Special_Sequence[1],PPP_Special_Sequence[2],PPP_Special_Sequence[3],PPP_Special_Sequence[4],PPP_Special_Sequence[5],PPP_Special_Sequence[6],PPP_Special_Sequence[7],PPP_Special_Sequence[8],PPP_Special_Sequence[9]);
     std::cout << "PPP_Special_Se is   " <<  PPP_TXT << std::endl;
     std::cout << strlen(PPP_Special_Sequence) << std::endl;


     //--------------------------------------------------
     strcat(PPP_Special_7E,PPP_Special_7D);
     strcat(PPP_Special_7E,PPP_Special_00);
     strcat(PPP_Special_7E,PPP_Special_Sequence);
     strcat(PPP_Special_7E,PPP_Normal);
     strcat(PPP_Send,PPP_Special_7E);
     //int c = sprintf(PPP_TXT,"%s",PPP_Send);
     //std::cout << PPP_digit << std::endl;
     //std::cout << strlen(PPP_Send) << std::endl;
     //std::cout << PPP_Send[1022] << PPP_Send[1023] << PPP_Send[1024] << std::endl;//<< PPP_Send[1025] << PPP_Send[1026] << PPP_Send[1027] << PPP_Send[1028]<< std::endl;


     //std::cout << PPP_Special_7E << std::endl;
//
//     std::cout << PPP_Normal << std::endl;
//     std::cout << strlen(PPP_Normal) << std::endl;
//
//     sprintf(PPP_TXT,"%s",PPP_Send);
//     std::cout << strlen(PPP_Send) << std::endl;
//     std::cout << "PPP_TXT is   " <<  PPP_TXT << std::endl;

}


int Recive_digit;
int Data_flag;
unsigned char *PPP_Recive;
char Decode_Recive[2*1024];
int head,tail;
int Decode_decade;
int Decode_flag;
int Decode_digit;

void PPP_Protocol_Decode()
{


     //if (PPP_Recive[Recive_digit] == 0x7D)//帧头0x7D,0x8E,帧尾0x7E
     //    if (PPP_Recive[Recive_digit + 1] == 0x8E)
     //    {
     //        Data_flag = 1;
     //        head = Recive_digit;
     //    }
     //if (Data_flag == 1 && PPP_Recive[Recive_digit] == 0x7E) Data_flag = 2;
     //if (Data_flag == 2 && PPP_Recive[Recive_digit] == 0x7E)
     //{
     //    Data_flag = 0;
     //    tail      = Recive_digit;
     //}

    PPP_Recive = (unsigned char *)PPP_Send;
    Decode_flag = 1;
    while(Decode_flag)//bug，我把0都改成200了，得改回去
    {
        if (PPP_Recive[Recive_digit] == 0x7D)//帧头0x7D,0x8E,帧尾0x7E
        {
            if (PPP_Recive[Recive_digit+1] == 0x8E)
            {
                if (PPP_Recive[Recive_digit+3] == 101)//百位内没有数据
                {
                    Recive_digit+=4;
                    Decode_decade = PPP_Recive[Recive_digit] * 100;
                    for(int B = 0; B < 2*1024; B++)
                    {
                        Recive_digit++;

                        if (PPP_Recive[Recive_digit] == 0x7D || PPP_Recive[Recive_digit] == 0x7E)break;//遇到下一个数的信息
                        if (PPP_Recive[Recive_digit] == 101)//遇到进位标志位
                        {
                            Recive_digit++;
                            Decode_decade = PPP_Recive[Recive_digit] * 100;//更新百位上的数字
                            continue;
                        }
                        Decode_Recive[Decode_decade + PPP_Recive[Recive_digit]] = 0x8E - 0x10;
                    }
                }
                else if (PPP_Recive[Recive_digit+3] != 101)//百位内有数据
                {
                    Recive_digit += 2;
                    Decode_decade = 0;//刷新Decode的百位数字
                    for (int B = 0; B < 2*1024; B++)
                    {
                        std::cout << "1" << std::endl;
                        Recive_digit++;
                        if (PPP_Recive[Recive_digit] == 0x7D || PPP_Recive[Recive_digit] == 0x7E)break;//遇到下一个数的信息
                        if (PPP_Recive[Recive_digit] == 101)//遇到进位标志位
                        {
                            Recive_digit++;
                            Decode_decade = PPP_Recive[Recive_digit] * 100;//更新百位上的数字
                            continue;
                        }
                        Decode_Recive[Decode_decade + PPP_Recive[Recive_digit]] = 0x8E - 0x10;
                    }
                }

            }

            else if (PPP_Recive[Recive_digit+1] == 0x8D)
            {
                if (PPP_Recive[Recive_digit+3] == 101)//百位内没有数据
                {
                    Recive_digit+=4;
                    Decode_decade = PPP_Recive[Recive_digit] * 100;
                    for(int B = 0; B < 2*1024; B++)
                    {
                        Recive_digit++;

                        if (PPP_Recive[Recive_digit] == 0x7D || PPP_Recive[Recive_digit] == 0x7E)break;//遇到下一个数的信息
                        if (PPP_Recive[Recive_digit] == 101)//遇到进位标志位
                        {
                            Recive_digit++;
                            Decode_decade = PPP_Recive[Recive_digit] * 100;//更新百位上的数字
                            continue;
                        }
                        Decode_Recive[Decode_decade + PPP_Recive[Recive_digit]] = 0x8D - 0x10;
                    }
                }
                else if (PPP_Recive[Recive_digit+3] != 101)//百位内有数据
                {
                    Recive_digit+=2;
                    Decode_decade = 0;
                    for (int B = 0; B < 2*1024; B++)
                    {
                        Recive_digit++;
                        Decode_decade = 0;//刷新Decode的百位数字
                        if (PPP_Recive[Recive_digit] == 0x7D || PPP_Recive[Recive_digit] == 0x7E)break;//遇到下一个数的信息
                        if (PPP_Recive[Recive_digit] == 101)//遇到进位标志位
                        {
                            Recive_digit++;
                            Decode_decade = PPP_Recive[Recive_digit] * 100;//更新百位上的数字
                            continue;
                        }
                        Decode_Recive[Decode_decade + PPP_Recive[Recive_digit]] = 0x8D - 0x10;
                    }
                }

            }


            else if (PPP_Recive[Recive_digit+1] == 0x10)
            {
                if (PPP_Recive[Recive_digit+3] == 101)//百位内没有数据
                {
                    Recive_digit+=4;
                    Decode_decade = PPP_Recive[Recive_digit] * 100;
                    for(int B = 0; B < 2*1024; B++)
                    {
                        Recive_digit++;

                        if (PPP_Recive[Recive_digit] == 0x7D || PPP_Recive[Recive_digit] == 0x7E)break;//遇到下一个数的信息
                        if (PPP_Recive[Recive_digit] == 101)//遇到进位标志位
                        {
                            Recive_digit++;
                            Decode_decade = PPP_Recive[Recive_digit] * 100;//更新百位上的数字
                            continue;
                        }
                        Decode_Recive[Decode_decade + PPP_Recive[Recive_digit]] = 0x10 - 0x09;
                    }
                }
                else if (PPP_Recive[Recive_digit+3] != 101)//百位内有数据
                {
                    Recive_digit+=2;
                    Decode_decade = 0;//刷新Decode的百位数字
                    for (int B = 0; B < 2*1024; B++)
                    {
                        Recive_digit++;
                        if (PPP_Recive[Recive_digit] == 0x7D || PPP_Recive[Recive_digit] == 0x7E)break;//遇到下一个数的信息
                        if (PPP_Recive[Recive_digit] == 101)//遇到进位标志位
                        {
                            Recive_digit++;
                            Decode_decade = PPP_Recive[Recive_digit] * 100;//更新百位上的数字
                            continue;
                        }
                        Decode_Recive[Decode_decade + PPP_Recive[Recive_digit]] = 0x10 + 0x10;
                    }
                }
            }
            else
            {
                unsigned char Decode_Sequence = PPP_Recive[Recive_digit+1];
                if (PPP_Recive[Recive_digit+3] == 101)//百位内没有数据
                {
                    Recive_digit+=4;
                    Decode_decade = PPP_Recive[Recive_digit] * 100;
                    for(int B = 0; B < 2*1024; B++)
                    {
                        Recive_digit++;

                        if (PPP_Recive[Recive_digit] == 0x7D || PPP_Recive[Recive_digit] == 0x7E)break;//遇到下一个数的信息
                        if (PPP_Recive[Recive_digit] == 101)//遇到进位标志位
                        {
                            Recive_digit++;
                            Decode_decade = PPP_Recive[Recive_digit] * 100;//更新百位上的数字
                            continue;
                        }
                        Decode_Recive[Decode_decade + PPP_Recive[Recive_digit]] = Decode_Sequence - 0x10;
                    }
                }
                else if (PPP_Recive[Recive_digit+3] != 101)//百位内有数据
                {
                    Recive_digit+=2;
                    Decode_decade = 0;
                    for (int B = 0; B < 2*1024; B++)
                    {
                        Recive_digit++;
                        Decode_decade = 0;//刷新Decode的百位数字
                        if (PPP_Recive[Recive_digit] == 0x7D || PPP_Recive[Recive_digit] == 0x7E)break;//遇到下一个数的信息
                        if (PPP_Recive[Recive_digit] == 101)//遇到进位标志位
                        {
                            Recive_digit++;
                            Decode_decade = PPP_Recive[Recive_digit] * 100;//更新百位上的数字
                            continue;
                        }
                        Decode_Recive[Decode_decade + PPP_Recive[Recive_digit]] = Decode_Sequence - 0x10;
                    }
                }

            }


        }
        else if (PPP_Recive[Recive_digit] == 0x7E)
        {
            Recive_digit++;
            for(int A = 0; A < 2*1024; A++)
            {
                if (Decode_Recive[Decode_digit] != 0)
                {
                    Decode_digit++;
                    continue;
                }
                if (PPP_Recive[Recive_digit] == 0x7E)
                {
                    Decode_flag = 0;
                    break;
                }
                Decode_Recive[Decode_digit] = PPP_Recive[Recive_digit];
                Recive_digit++;
                Decode_digit++;
            }
        }

    }
}

int  main() {
     //std::cout << "Hello, World!" << std::endl;
     PPP_init();
     //for(int i = 0;i < 48;i++)
     //{

    //char x = 0xC8;
    //sprintf(PPP_TXT,"%d",x);
    //std::cout << PPP_TXT << std::endl

           PPP_Protocol_Code((unsigned char **)&test);
           PPP_Protocol_Decode();
           //sprintf(PPP_TXT,"%d",PPP_Send[0]);
           //std::cout << PPP_TXT << std::endl;

    //sprintf(PPP_TXT,"%d",Decode_Recive[1]);
    sprintf(PPP_TXT,"%d",Decode_Recive[1016]);
    std::cout << PPP_TXT << std::endl;

           std::cout << Decode_Recive << std::endl;
           std::cout << strlen(Decode_Recive) << std::endl;
           char * p = std::find(Decode_Recive, Decode_Recive + strlen(Decode_Recive), 0x10);

           if (p != Decode_Recive + strlen(Decode_Recive))//判断是否查找成功
           {
               //std::cout << p << std::endl;
           }
           else
           {
               std::cout << "NO" << std::endl;
           }
         //int c = sprintf(PPP_TXT,"%s",PPP_Send);
         //std::cout << PPP_TXT << std::endl;
         //sprintf(PPP_TXT,"%d",PPP_Send[1]);
         //std::cout << PPP_TXT << std::endl;+
         //std::cout << PPP_digit << std::endl;
     //}
     //char TXT[3] = {60,0,50};
     //int c = sprintf(PPP_TXT,"%s",TXT);
     //std::cout << PPP_TXT << std::endl;
     ////sprintf(PPP_TXT,"%d",PPP_Send[23]);
     //std::cout << c << std::endl;

     return 0;
}
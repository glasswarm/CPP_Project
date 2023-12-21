#include "PPP_Protocol_Decode.h"

//------------------------------------------------
//分离buffer的变量定义
unsigned char PPP_Recive_single[2 * PPP_Decode_digit];//接收数组定义

int head,middle,tail;                                 //枕头帧尾
int separate_digit;
int receive_end_flag = 1;                             //接收完毕数据
int middle_flag;                                      //标志数组进入中继状态
int middle_digit;
int receive_7D_middle_flag;
int separate_flag;                                    //表示一个buffer分离完成


void receive_separate(unsigned char PPP_Recive_Buffer[2 * PPP_Decode_digit])//分离buffer用的函数
{
     int A = 0,B = 0;
     while(receive_end_flag)
     {
//------------------------------------------------
//一个帧接收完了，复位
           if (receive_end_flag == 4)
           {
               head             = 0;
               middle_digit     = 0;
               tail             = 0;
               receive_7D_middle_flag  = 0;
               receive_end_flag = 1;
               memset(&PPP_Recive_single , 0, sizeof(PPP_Recive_single));
           }
//------------------------------------------------
//正常状态
           if (PPP_Recive_Buffer[separate_digit] == 0x7D && receive_end_flag == 1)//帧头
           {
               if (PPP_Recive_Buffer[separate_digit + 1] == 0x8E)
               {
                   head             = separate_digit;
                   receive_end_flag = 2;
               }
               else if(PPP_Recive_Buffer[separate_digit + 1] == 0x00)//代表一个buffer,0x7D后面没有0x8E,而0x8E在下一个buffer内
               {
                   receive_end_flag = 7;
                   separate_digit   = 0;

                   separate_flag    = 1;//表示一个buffer分离完成
                   break;
               }

           }

           else if (PPP_Recive_Buffer[separate_digit] == 0x7E && receive_end_flag == 2)//帧尾
           {
               receive_end_flag = 3;
           }

           else if (PPP_Recive_Buffer[separate_digit] == 0x7E && receive_end_flag == 3)//帧尾
           {
               if (receive_7D_middle_flag == 0)
               {
                   tail = separate_digit;
                   receive_end_flag = 4;
                   for (A = 0, B = head; B <= tail; A++, B++)
                   {
                       PPP_Recive_single[A] = PPP_Recive_Buffer[B];
                   }
                   separate_digit++;
                   break;
               }
               else if (receive_7D_middle_flag == 1)//代表一个buffer,0x7D后面没有0x8E,0x8E在下一个buffer内
               {
                   tail = separate_digit;
                   receive_end_flag = 4;
                   PPP_Recive_single[0] = 0x7D;
                   for (A = 1, B = head; B <= tail; A++, B++)
                   {
                       PPP_Recive_single[A] = PPP_Recive_Buffer[B];
                   }
                   separate_digit++;
               }

           }

//------------------------------------------------
//中继状态
           if (PPP_Recive_Buffer[0] == 0x8E && receive_end_flag == 7)//代表一个buffer,0x7D后面没有0x8E,且在下一个buffer接收到了0x8E
           {
               receive_7D_middle_flag = 1;
               receive_end_flag = 2;
               head = separate_digit;
           }

           if (PPP_Recive_Buffer[separate_digit + 1] == 0x00 && ( receive_end_flag == 2 || receive_end_flag == 3) ) //代表进入中继状态
           {
               if(receive_end_flag == 2)receive_end_flag = 5;
               if(receive_end_flag == 3)receive_end_flag = 6;
               middle_flag = 1;

               if (receive_7D_middle_flag == 0)
               {
                   for(A = 0 , B = head; B <= separate_digit; A++ , B++)
                   {
                       PPP_Recive_single[A] = PPP_Recive_Buffer[B];
                   }
                   separate_digit   = 0;
                   middle           = A;

                   separate_flag    = 1;//表示一个buffer分离完成
                   break;
               }

               else if(receive_7D_middle_flag == 1)//代表一个buffer,0x7D后面没有0x8E,0x8E在下一个buffer内
               {
                   PPP_Recive_single[0] = 0x7D;
                   for(A = 1 , B = head; B <= separate_digit; A++ , B++)
                   {
                       PPP_Recive_single[A] = PPP_Recive_Buffer[B];
                   }
                   separate_digit   = 0;
                   middle           = A;

                   separate_flag    = 1;//表示一个buffer分离完成
                   break;
               }

           }

           else if (middle_flag == 1 && receive_end_flag == 5)                      //没有接收到中间帧头的,中继状态.
           {
               if (PPP_Recive_Buffer[separate_digit] == 0x7E)                       //下一个buffer接收到中间帧头,进入追踪帧尾
               {
                   memcpy(PPP_Recive_single + middle, PPP_Recive_Buffer, separate_digit + 1 );
                   middle_digit     = (separate_digit + 1);
                   middle          += (separate_digit + 1);
                   receive_end_flag =                   6 ;
               }

               else if (PPP_Recive_Buffer[separate_digit] == 0x00 )                  //下一个buffer没有接收到中间帧头,继续中继状态.
               {
                   strcat((char *)PPP_Recive_single,(char *)PPP_Recive_Buffer);
                   middle += separate_digit;
                   separate_digit = 0;

                   separate_flag  = 1;//表示一个buffer分离完成
                   break;
               }
           }

           else if (middle_flag == 1 && receive_end_flag == 6)                       //中继,追踪帧尾状态.
           {
               if (PPP_Recive_Buffer[separate_digit] == 0x7E)                        //下一个buffer接收到帧尾，退出中继状态，回归正常状态
               {
                   tail             = separate_digit;
                   middle_flag      =              0;
                   receive_end_flag =              4;
                   memcpy(PPP_Recive_single + middle, PPP_Recive_Buffer + middle_digit, separate_digit + 1 - middle_digit );
                   break;
               }

               else if (PPP_Recive_Buffer[separate_digit] == 0x00 )                  //下一个buffer没有接收到帧尾,继续中继状态.
               {
                   middle += separate_digit;
                   middle_digit   = 0;
                   separate_digit = 0;
                   strcat((char *)PPP_Recive_single,(char *)PPP_Recive_Buffer);

                   separate_flag  = 1;//表示一个buffer分离完成
                   break;
               }

           }
           separate_digit ++;
     }
}

//-----------------------------------------------
//刷新TCP_buffer用函数
unsigned char New_receive[2 * PPP_Decode_digit];

int New_Buffer()
{
     if (separate_flag == 1)
     {
         return 1;
     }
     else
     {
         receive_separate(New_receive);
         return 0;
     }
}


//------------------------------------------------
//收到的数据解码时的标志数
int  Receive_digit;

//------------------------------------------------
//解码变量定义
int  Decode_decade;
int  Decode_digit;
int  Decode_end_flag; //解码完成标志
int  Decode_0x20_flag;//收到首序列的标志
//------------------------------------------------
//00变量定义
int           Decode_00     [2 * PPP_Decode_digit];//为0专门设置的数组
unsigned char Decode_Receive[2 * PPP_Decode_digit];//解码数组定义
unsigned int  Decode_00_Digit_max;                 //0数字的下标最大值
unsigned int  Decode_00_Digit;

unsigned int  One_frame_end;                       //一张图片传输完成

void Decode_Init() //特殊数解码
{
     Decode_Receive[0] = 0x7E;
     Decode_end_flag = 1;

     Decode_digit  = 0;
     Decode_decade = 0;
     Receive_digit = 0;

     Decode_00_Digit     = 0;
     Decode_00_Digit_max = 0;
}

void Clear_Decode_array()//清空数组
{
    memset(&Decode_00         , 0, sizeof(Decode_00         ));//sizeof是定义这个数组的大小,而不是遇到0的大小
    memset(&Decode_Receive    , 0, sizeof(Decode_Receive    ));//sizeof是定义这个数组的大小,而不是遇到0的大小
}

void Special_Decode_Process(unsigned char input_array[2 * PPP_Decode_digit],unsigned char DECODED_DATA) //特殊数解码
{
     int A = 0;//for用变量

     if (input_array[Receive_digit + 3] == 101)//百位内没有数据
     {
         Receive_digit += 4;
         Decode_decade = input_array[Receive_digit] * 100;

         for (A = 0; A < 2 * 1024; A++)
         {
              Receive_digit++;

              if (input_array[Receive_digit] == 0x7D || input_array[Receive_digit] == 0x7E)//遇到下一个数的信息
              {
                  if (DECODED_DATA == 0x10)Decode_00[Decode_00_Digit_max] = 65535;//对0进行特殊处理
                  break;
              }
              if (input_array[Receive_digit] == 101)//遇到进位标志位
              {
                  Receive_digit++;
                  Decode_decade = input_array[Receive_digit] * 100;//更新百位上的数字.
                  if (input_array[Receive_digit + 1] == 200) input_array[Receive_digit + 1] = 0;
                  continue;
              }
              if (DECODED_DATA == 0x10)//对0进行特殊处理
              {
                  Decode_00[Decode_00_Digit_max] = Decode_decade + input_array[Receive_digit];
                  Decode_00_Digit_max++;
              }

              Decode_Receive[Decode_decade + input_array[Receive_digit]] = DECODED_DATA - 0x10;
         }
     }

     else if (input_array[Receive_digit + 3] != 101)//百位内有数据
     {
          if (input_array[Receive_digit + 3] == 200) input_array[Receive_digit + 3] = 0;
          Receive_digit += 2;
          Decode_decade = 0;//刷新Decode的百位数字
          for (A = 0; A < 2 * 1024; A++)
          {

               Receive_digit++;
               if (input_array[Receive_digit] == 0x7D || input_array[Receive_digit] == 0x7E)//遇到下一个数的信息
               {
                   if (DECODED_DATA == 0x10)Decode_00[Decode_00_Digit_max] = 65535;//对0进行特殊处理
                   break;
               }
               if (input_array[Receive_digit] == 101)//遇到进位标志位
               {
                   Receive_digit++;
                   Decode_decade = input_array[Receive_digit] * 100;//更新百位上的数字
                   if (input_array[Receive_digit + 1] == 200) input_array[Receive_digit + 1] = 0;
                   continue;
               }
               if (DECODED_DATA == 0x10)//对0进行特殊处理
               {
                   Decode_00[Decode_00_Digit_max] = Decode_decade + input_array[Receive_digit];
                   Decode_00_Digit_max++;
               }
               Decode_Receive[Decode_decade + input_array[Receive_digit]] = DECODED_DATA - 0x10;
          }    
      }
}


unsigned char *PPP_Protocol_Decode(unsigned char input_array[2 * PPP_Decode_digit])
{
//------------------------------------------------
//清空,初始化数组
    Clear_Decode_array();
    Decode_Init();

    while(Decode_end_flag)//0得改成200，不然buffer接收的时候不知道哪个是结尾
    {
        int A = 0;

        if (input_array[Receive_digit] == 0x7D)//帧头0x7D,0x8E,帧尾0x7E
        {
            if     (input_array[Receive_digit + 1] == 0x8E)Special_Decode_Process(input_array , 0x8E);//0x7E数据
            else if(input_array[Receive_digit + 1] == 0x8D)Special_Decode_Process(input_array , 0x8D);//0x7D数据
            else if(input_array[Receive_digit + 1] == 0x10)Special_Decode_Process(input_array , 0x10);//0x00数据
            else//seq数据
            {
                unsigned char Decode_Sequence = input_array[Receive_digit+1];
                Special_Decode_Process(input_array , Decode_Sequence);
            }
        }
        //if(input_array[Receive_digit] == 0x7D)Decode_flag = 0;
        else if (input_array[Receive_digit] == 0x7E)
        {
            int sequence = input_array[Receive_digit + 1]; //将序列号放入最后
            Receive_digit++;
            for(A = 0; A < 2*1024; A++)
            {
                if (Decode_Receive[Decode_digit] != 0)
                {
                    Decode_digit++;
                    continue;
                }
                if (Decode_digit == Decode_00[Decode_00_Digit])//0的特殊处理,如果没有[0]位就是65535，如果有就是正常数据
                {
                    Decode_00_Digit++;
                    Decode_digit++;
                    continue;
                }
                if (input_array[Receive_digit] == 0x7E)
                {
                    //Decode_Receive[Decode_digit] = 0x7E;//sequence;
                    Decode_end_flag = 0;
                    break;
                }
                Decode_Receive[Decode_digit] = input_array[Receive_digit];
                Receive_digit++;
                Decode_digit++;

            }
        }
    }
    Decode_Receive[Decode_digit] = 0x7E;//补尾帧

//---------------------------------------
//判断是否一帧完成
    if (Decode_Receive[1] == 0x20 && One_frame_end == 0)
    {
        One_frame_end    = 1;
        Decode_0x20_flag = 1;
    }
    else if (Decode_Receive[1] == 0x20 && One_frame_end == 0)
    {
        One_frame_end    = 0;
    }
//---------------------------------------
//接收第一帧,序列0x20开始,接收到0x20代表了起始帧
    if(Decode_0x20_flag) return (unsigned char *)Decode_Receive;
    else                 return NULL;
}



//---------------------------------------
//example main

//int  main()
//{
//----------------------------------
////PPP_Code
//test_init();
//for(int Count = 0 ; Count < 23 ; Count++)
//{
////PPP_Protocol_Code(test);
////printf("I\'m head, %s ,I\'m tail \n",PPP_Send);
////----------------------------------
////PPP_Decode
//PPP_Protocol_Decode((unsigned char *)PPP_Protocol_Code(test));
////PPP_Protocol_Decode((unsigned char *)PPP_Send);
//printf("I\'m Decode_head, %s ,I\'m Decode_tail \n",Decode_Receive);
//std::cout << strlen((const char*)Decode_Receive) << std::endl;
//std::cout << Count << std::endl;
//}
//return 0;
//}
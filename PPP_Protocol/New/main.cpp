#include <iostream>
#include "PPP_Protocol_Decode.h"
extern "C"
{
   #include "PPP_Protocol_code.h"
}

extern unsigned char test[input_decade][input_digit        ];
extern          char PPP_Send         [2 * PPP_Code_digit  ];
extern unsigned char Decode_Receive   [2 * PPP_Decode_digit];//解码数组定义
extern          int  One_frame_end;
extern unsigned char PPP_Recive_single[2 * PPP_Decode_digit];
extern          int  receive_end_flag;
extern          int  middle_flag;
extern          int  middle;

unsigned char R [4096] = {7,8,9,4,5,6,12,3,0x7E};

unsigned char L [2048] = {0x7D,0x8E,0xc8,0x07,0x7d,0x8d,0xc8,0x03,0x7d,0x10,0xc8,0x02,0x7d,0x20,0xc8,0x7e};
unsigned char L1[2048] = {0x20,0x22,0x54};
unsigned char L2[2048] = {0x78,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x97,0x7e,0x7D};

unsigned char L3[2048] = {0x8E,0xc8,0x7d,0x8d,0xc8,0x7d,0x10,0xc8,0x7d,0x20,0xc8};
unsigned char L4[2048] = {0x20,0x22,0x55};
unsigned char L5[2048] = {0x88,0x88,0x88,0x7e,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x7e};

int  main()
{
//----------------------------------
//PPP_Code
    test_init();
    int Count = 0;

//    while(1)
//    {
//        while(!New_Buffer())
//        {
////----------------------------------
////PPP_Decode
//            unsigned char *Decode = PPP_Protocol_Decode(PPP_Recive_single);
//            if(Decode == NULL)continue;
//
//            Count++;
//        }
//        //New_receive = buffer;//此处buffer为TCP接收buffer
//    }

    unsigned char *F  = PPP_Protocol_Code(test);

    strcat((char *)R , (const char *)F);
    strcat((char *)R , (const char *)F);
    strcat((char *)R , (const char *)L);


//    receive_separate(R);
//    receive_separate(R);
//    receive_separate(R);

//-----------------------
//测试分离buffer是否正常
    receive_separate(L);
    receive_separate(L1);
    receive_separate(L2);
    std::cout << "This is L2 len ," << strlen((const char *)PPP_Recive_single) << ",end" << std::endl;
//-----------------------
//分离出来的帧,显示
    printf("%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",PPP_Recive_single[0],PPP_Recive_single[1],PPP_Recive_single[2],PPP_Recive_single[3],PPP_Recive_single[4],PPP_Recive_single[5],PPP_Recive_single[6],PPP_Recive_single[7],PPP_Recive_single[8],PPP_Recive_single[9],PPP_Recive_single[10],PPP_Recive_single[11],PPP_Recive_single[12],PPP_Recive_single[13]);
    printf("%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",PPP_Recive_single[14],PPP_Recive_single[15],PPP_Recive_single[16],PPP_Recive_single[17],PPP_Recive_single[18],PPP_Recive_single[19],PPP_Recive_single[20],PPP_Recive_single[21],PPP_Recive_single[22],PPP_Recive_single[23],PPP_Recive_single[24],PPP_Recive_single[25],PPP_Recive_single[26],PPP_Recive_single[27]);
    printf("%x,%x,%x,%x\n",PPP_Recive_single[28],PPP_Recive_single[29],PPP_Recive_single[30],PPP_Recive_single[31]);
//-----------------------
//分离出来的帧,进行解码
    unsigned char *Decode = PPP_Protocol_Decode(PPP_Recive_single);
//-----------------------
//解码帧显示
    printf("\n%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",Decode[0],Decode[1],Decode[2],Decode[3],Decode[4],Decode[5],Decode[6],Decode[7],Decode[8],Decode[9],Decode[10],Decode[11],Decode[12],Decode[13]);
    printf("%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",Decode[14],Decode[15],Decode[16],Decode[17],Decode[18],Decode[19],Decode[20],Decode[21],Decode[22],Decode[23],Decode[24],Decode[25],Decode[26],Decode[27]);
    std::cout << Decode << std::endl;

//-----------------------
//测试一次断点后的再次断点，能否正常
    receive_separate(L2);
    receive_separate(L3);
    receive_separate(L4);
    receive_separate(L5);
    std::cout << "\nThis is L5 len ," << strlen((const char *)PPP_Recive_single) << ",end" << std::endl;
//-----------------------
//分离出来的帧,显示
    printf("%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",PPP_Recive_single[0],PPP_Recive_single[1],PPP_Recive_single[2],PPP_Recive_single[3],PPP_Recive_single[4],PPP_Recive_single[5],PPP_Recive_single[6],PPP_Recive_single[7],PPP_Recive_single[8],PPP_Recive_single[9],PPP_Recive_single[10],PPP_Recive_single[11],PPP_Recive_single[12],PPP_Recive_single[13]);
    printf("%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",PPP_Recive_single[14],PPP_Recive_single[15],PPP_Recive_single[16],PPP_Recive_single[17],PPP_Recive_single[18],PPP_Recive_single[19],PPP_Recive_single[20],PPP_Recive_single[21],PPP_Recive_single[22],PPP_Recive_single[23],PPP_Recive_single[24],PPP_Recive_single[25],PPP_Recive_single[26],PPP_Recive_single[27]);
    printf("%x,%x,%x,%x\n",PPP_Recive_single[28],PPP_Recive_single[29],PPP_Recive_single[30],PPP_Recive_single[31]);
//-----------------------
//分离出来的帧,进行解码
    unsigned char *Decode1 = PPP_Protocol_Decode(PPP_Recive_single);
//-----------------------
//解码帧显示
    printf("\n%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",Decode1[0],Decode1[1],Decode1[2],Decode1[3],Decode1[4],Decode1[5],Decode1[6],Decode1[7],Decode1[8],Decode1[9],Decode1[10],Decode1[11],Decode1[12],Decode1[13]);
    printf("%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",Decode1[14],Decode1[15],Decode1[16],Decode1[17],Decode1[18],Decode1[19],Decode1[20],Decode1[21],Decode1[22],Decode1[23],Decode1[24],Decode1[25],Decode1[26],Decode1[27]);
    std::cout << Decode1 << std::endl;
}


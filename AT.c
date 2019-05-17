#include "AT.h"
#include "string.h" 

static unsigned char RevData[COUNT_REVDATA] = {0};
static enumAtMode UsartMode = ATMODE;
//==================================================待移植函数==================================================
//AT命令发送延时函数
//默认为10ms
static void ATDelay(void)
{
    delay_ms(10);
}

//串口发送AT命令数据
void UsartSendCmd(unsigned char* dat)
{
    if(dat != NULL)
    {
        uartSendstring(dat);
    }
}
//==================================================待移植函数==================================================
//获取串口数据缓存地址
unsigned char *GetRevDataAddr(void)
{
  return RevData;
}
//清除缓存
//实际上为添加结束符
void ClearRevData(void)
{
	RevData[0] = 0;//添加结束符
	GetUsartData(0,1);
}
//获取串口数据
//注意串口接收完毕后发送 GetUsartData(0,1)
//为解决连续发送AT指令时，程序来不及自动发送 GetUsartData(0,1)，可以通过在发送AT指令后增加 GetUsartData(0,1) 来手动清除缓存
void GetUsartData(unsigned char dat,unsigned char isstop)
{
    static int idx = 0;
    if(isstop)
    {
        UsartMode = ATMODE;
        idx = 0;
    }
    else
    {
		//缓存的最后一个必须为结束符 此时不再接收数据
		if(idx >= COUNT_REVDATA-1)
		{
			RevData[idx] = 0;//添加结束符
			UsartMode = REVDATA;
		}
		else
		{
			RevData[idx] = dat;
			RevData[idx+1] = 0;//添加结束符
			UsartMode = REVDATA;
			idx++;
		}
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//ATK-SIM900A 各项测试(拨号测试、短信测试、GPRS测试)共用代码

//sim900a发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
static unsigned char* ATCheck(unsigned char *str)
{
	char *strx=0;
    strx=strstr((const char*)RevData,(const char*)str);
	return (unsigned char*)strx;
}
//向sim900a发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
unsigned char ATSend(unsigned char *cmd,unsigned char *ack,unsigned short waittime)
{
	unsigned char res=0;

    UsartSendCmd(cmd);

	// USART3_RX_STA=0;
	// if((unsigned int)cmd<=0XFF)
	// {
	// 	while((USART3->SR&0X40)==0);//等待上一次数据发送完成  
	// 	USART3->DR=(unsigned int)cmd;
	// }else u3_printf("%s\r\n",cmd);//发送命令

	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			ATDelay();
            if(ATMODE == UsartMode)//接收到期待的应答结果
            {
                if(ATCheck(ack))break;//得到有效数据 
            }
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
static unsigned char chr2hex(unsigned char chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
static unsigned char hex2chr(unsigned char hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}



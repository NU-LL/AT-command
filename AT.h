#ifndef __AT_H__
#define __AT_H__

//AT指令接收缓存区大小
#define COUNT_REVDATA    1024

typedef enum
{
    REVDATA = 1,
    ATMODE,
}enumAtMode;

void GetUsartData(unsigned char dat,unsigned char isstop);
unsigned char ATSend(unsigned char *cmd,unsigned char *ack,unsigned short waittime);

#endif


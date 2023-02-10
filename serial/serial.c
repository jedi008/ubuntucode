#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>


#define RCommandLen 14
int set_opt(int,int,int,char,int);

char command_handle(int fd, char bufferCommand[RCommandLen]);

int check_end(int n_receive_byte, char* bufferCommand);

int write_bytes(int fd, int len, char* data);


int main(int argc, char *argv[])
{
	int fd,n_byte,n_receive_byte;
	char *uart3 = argv[1];//"/dev/ttyS1";

	char bufferR[2];
	char bufferW[2];
	char bufferCommand[RCommandLen];	

	memset(bufferR, '\0', 2);
	memset(bufferW, '\0', 2);
	memset(bufferCommand, '\0', RCommandLen);

	if((fd=open(uart3,O_RDWR,0777))<0)
	{
		printf("failed\n");
	}
	else{
		printf("success\n");
		set_opt(fd, 115200, 8, 'N', 1); 
	}
		
	n_receive_byte = 0;
    while(1)
    {	
        n_byte = 0;
        memset(bufferR, '\0', 2);
        memset(bufferW, '\0', 2);


        //MCU串口发送接收都是单字节(单字符)函数
        if((n_byte = read(fd, bufferR, 1)) == 1)
        {
            bufferCommand[n_receive_byte] = bufferR[0];
			++n_receive_byte;

			int r = check_end(n_receive_byte, bufferCommand);
			if(r == 0)
			{
				command_handle(fd, bufferCommand);
				n_receive_byte = 0;
				memset(bufferCommand, '\0', RCommandLen);
			}
			else if( r > 1)//error
			{
				//show command
				printf("receive bufferCommand: 0x");
				for(int i=0;i<RCommandLen;i++)
				{
					printf("%02x", bufferCommand[i]);
				}
				printf("\n");

				tcflush(fd,TCIFLUSH);//清空缓冲区
				n_receive_byte = 0;
				memset(bufferCommand, '\0', RCommandLen);
				printf("error!!!\n");
				char data[8] = {0xaa, 0xaa, 0x07, 0x02, 0x00, (char)r, 0xee, 0xee};
				write(fd,data,8);
			}
        }
    }

	close(fd);
	return 0;
}
//gcc serial.c -o serial





//串口通用初始化函数
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;//定义结构体newtio和oldtio
	//将原串口的数据取到oldtio
	if  ( tcgetattr( fd,&oldtio)  !=  0) { 
		perror("SetupSerial 1");
		return -1;
	}
	//将newio清零和设置c_cflag
	bzero( &newtio, sizeof( newtio ) );
	newtio.c_cflag  |=  CLOCAL | CREAD;//使能接收和忽略控制线
	newtio.c_cflag &= ~CSIZE;
	//设置数据位
	switch( nBits )
	{
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
			newtio.c_cflag |= CS8;
			break;
	}
	//设置校验位
	switch( nEvent )
	{
		//偶校验
		case 'O':
			newtio.c_cflag |= PARENB;//使能奇偶校验
			newtio.c_cflag |= PARODD;//偶校验
			newtio.c_iflag |= (INPCK | ISTRIP);//输入校验并忽略第八位
			break;
		case 'E': 
			newtio.c_iflag |= (INPCK | ISTRIP);
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;//取消偶校验（置零偶校验位），开启奇校验
			break;
		case 'N':  
			newtio.c_cflag &= ~PARENB;//不进行奇偶校验
			break;
	}
	//设置波特率
	switch( nSpeed )
	{
		case 2400:
			cfsetispeed(&newtio, B2400);
			cfsetospeed(&newtio, B2400);
			break;
		case 4800:
			cfsetispeed(&newtio, B4800);
			cfsetospeed(&newtio, B4800);
			break;
		case 9600:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
		case 115200:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
		case 460800:
			cfsetispeed(&newtio, B460800);
			cfsetospeed(&newtio, B460800);
			break;
		default:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
	}
	//设置停止位
	if( nStop == 1 )
		newtio.c_cflag &=  ~CSTOPB;//一位停止位
	else if ( nStop == 2 )
		newtio.c_cflag |=  CSTOPB;//两位停止位

	newtio.c_cc[VTIME]  = 0;//不设置读取超时
	newtio.c_cc[VMIN] = 0;//读取最小字符数为0

	tcflush(fd,TCIFLUSH);//清空缓冲区
	
	//使配置生效
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		perror("com set error");
		return -1;
	}
	//	printf("set done!\n\r");
	return 0;
}


char command_handle(int fd, char bufferCommand[RCommandLen])
{
	//show command
	printf("receive bufferCommand: 0x");
	for(int i=0;i<RCommandLen;i++)
	{
		printf("%02x", bufferCommand[i]);
	}
	printf("\n");

	int type = bufferCommand[3];
	switch (type)
	{
	case 1:
	{
		printf("start\n");
		char data[8] = {0xaa, 0xaa, 0x07, 0x01, 0x00, (char)type, 0xee, 0xee};
		write(fd,data,8);
	}
		break;
	case 2:
	{
		printf("stop\n");
		char data[8] = {0xaa, 0xaa, 0x07, 0x01, 0x00, (char)type, 0xee, 0xee};
		write(fd,data,8);
	}
		break;
	default:
		break;
	}
	



	
	return 0;
}


int check_end(int n_receive_byte, char* bufferCommand)
{
	if(n_receive_byte < 6) return 1;//too shot. It must not be over.

	if(bufferCommand[0] != 0xAA || (bufferCommand[1] != 0xAA && bufferCommand[1] != '\0'))
		return 2;
	
	int datalen = bufferCommand[2];
	if(datalen > RCommandLen-1)
		return 3;
	
	if(n_receive_byte > RCommandLen)
		return 4;
	
	if(bufferCommand[datalen-2] == 0xEE && bufferCommand[datalen-1] == 0xEE)
	{
		return 0;//end
	}

	if(bufferCommand[datalen-2] == 0xEE && bufferCommand[datalen-1] == '\0')
	{
		return 1;//wait end tag
	}

	if(bufferCommand[datalen-2] == '\0' && bufferCommand[datalen-1] == '\0')
	{
		return 1;//wait end tag
	}

	return 5;
}

int write_bytes(int fd, int len, char* data)
{
	for(int i=0;i<len;i++)
	{
		write(fd,data+i,1);
	}
	
}
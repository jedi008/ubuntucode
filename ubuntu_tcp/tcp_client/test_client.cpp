//客户端
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
 
int main(){
    //1.创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd){
        perror("socket error");
        exit(-1);
    }
 
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));//清空
    //2.填充服务器网络信息结构体 --需要指定连接哪个服务器
    server_addr.sin_family = AF_INET;
    //网络字节序的端口号，可以是 8888  9999 6789 等都可以
    server_addr.sin_port = htons(8888);
    //IP地址
    //不能随便填，可以填自己主机的IP地址
    //如果只是在本地测试，也可以填 127.0.0.1
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t addrlen = sizeof(server_addr);
 
    //3.与服务器建立连接
    if(-1 == connect(sockfd, (struct sockaddr *)&server_addr, addrlen)){
        perror("connect error");
        exit(-1);
    }
 
    //4.与服务器通信
    char buff[128] = {0};
    fgets(buff, 128, stdin);
    buff[strlen(buff)-1] = '\0';//清除 \n
    write(sockfd, buff, 128);
    read(sockfd, buff, 128);
    printf("收到回复:[%s]\n", buff);
 
    //5.关闭套接字
    close(sockfd);
 
    return 0;
}
//g++ -Wall -o client test_client.cpp -std=c++11 -lpthread

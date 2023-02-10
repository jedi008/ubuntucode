//服务器端
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
    //1.创建套接字   ---相当于 买了一个手机
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd){
        perror("socket error");
        exit(-1);
    }
 
    //创建服务器网络信息结构体  ---相当于办了一张卡
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));//清空
    //2.填充服务器网络信息结构体
    server_addr.sin_family = AF_INET;
    //网络字节序的端口号，可以是 8888  9999 6789 等都可以
    server_addr.sin_port = htons(8888);
    //IP地址
    //不能随便填，可以填自己主机的IP地址
    //如果只是在本地测试，也可以填 127.0.0.1
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t addrlen = sizeof(server_addr);
 
    //3.将套接字和网络信息结构体进行绑定---相当于把卡插入手机里
    if(-1 == bind(sockfd, (struct sockaddr *)&server_addr, addrlen)){
        perror("bind error");
        exit(-1);
    }
 
    //4.将服务器的套接字设置成被动监听状态
    if(-1 == listen(sockfd, 5)){
        perror("listen error");
        exit(-1);
    }
 
    //定义一个结构体，保存客户端的信息
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));//清空
    socklen_t clientaddrlen = sizeof(client_addr);
    //5.阻塞等待客户端连接
    int acceptfd = accept(sockfd, (struct sockaddr *)&client_addr, &clientaddrlen);
    if(-1 == acceptfd){
        perror("accept error");
        exit(-1);
    }
 
    printf("客户端 %s:%d 连接到服务器了\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
 
    //6.与客户端通信
    char buff[128] = {0};
    read(acceptfd, buff, 128);
 
    printf("%s-%d:[%s]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buff);
 
    strcat(buff, "--server");
    write(acceptfd, buff, 128);
 
    //7.关闭套接字
    close(acceptfd);
    close(sockfd);
 
    return 0;
}
//g++ -Wall -o server test_server.cpp -std=c++11 -lpthread

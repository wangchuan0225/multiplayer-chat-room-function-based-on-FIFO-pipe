#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include "clientinfo.h"
#define FIFO_1 "sendmessage/REG_FIFO"
#define FIFO_2 "sendmessage/LOGIN_FIFO"
#define FIFO_3 "sendmessage/MSG_FIFO"
#define FIFO_4 "sendmessage/LOGOUT_FIFO"
int main(int argc,char *argv[])
{
   if (atoi(argv[1])==1){
    // 打开服务器管道用于发送请求
    mkfifo(FIFO_1,0777);
    int serverFd = open(FIFO_1, O_RDWR);
       if(serverFd==-1){
        perror("Failed to open server FIFO for writing");
        exit(1);
    }
    char receive_path[100] = "recievemessage/";
    strcat(receive_path, argv[2]); // 使用用户名作为管道名
    mkfifo(receive_path, 0777);
    
    // 用户注册
    REGISTER newUser;
    newUser.op=atoi(argv[1]);
    strcpy(newUser.username, argv[2]);
    strcpy(newUser.password, argv[3]);
    strcpy(newUser.myfifo,receive_path);
    int a=write(serverFd,&newUser,sizeof(newUser));
    	if(a==-1)
      	perror("can't send");
    close(serverFd);
    //printf("%s \n %s \n %s",newUser.username,newUser.password,newUser,newUser.myfifo);
    }
    
    //进行登录服务
    if (atoi(argv[1])==2){
    // 打开服务器管道用于发送登录请求
    mkfifo(FIFO_2,0777);
    int serverFd;
    if ((serverFd = open(FIFO_2, O_RDWR)) < 0) {
        perror("Failed to open server FIFO for writing");
        exit(1);
    }
    char receive_path[100] = "recievemessage/";
    strcat(receive_path, argv[2]); // 使用用户名作为管道名
    mkfifo(receive_path, 0777);
    // 用户登录
    LOGIN loginUser;
    strcpy(loginUser.username, argv[2]);
    strcpy(loginUser.password, argv[3]);
    strcpy(loginUser.myfifo,receive_path);
    loginUser.op=atoi(argv[1]);
    int nwrite= write(serverFd,&loginUser,sizeof(loginUser));
    if(nwrite==-1)
	    printf("write error");
    //printf("%s \n %s \n %s",loginUser.username,loginUser.password,loginUser.myfifo);
    close(serverFd);
    }
    //信息发送
    if (atoi(argv[1])==3){
    // 打开服务器管道用于发送聊天消息
   
    mkfifo(FIFO_3,0777);
    int serverFd;
    if( (serverFd = open(FIFO_3, O_RDWR)) < 0) {
        perror("Failed to open server FIFO for writing");
        exit(1);
    
    }
    // 发送聊天消息给其他用户
    MESSAGE sendmessage;char sendname[50];char messages[100];
    sendmessage.op=atoi(argv[1]);
    int sumpeople=atoi(argv[2]);//输入发送人数
    strcpy(sendname,argv[3]);//本人名称
    strcpy(messages,argv[4]);//消息
    for(int i=0;i<sumpeople;i++)
    {
      MESSAGE sendmessage;
      strcpy(sendmessage.destname,argv[i+5]);//目标名称
      strcpy(sendmessage.myfifo,sendname);//目标名称
      strcpy(sendmessage.message,messages);//目标名称
      int nwrite= write(serverFd,&sendmessage,sizeof(sendmessage));
      if(nwrite<=0)
	    printf("write error");
    }
	  
    close(serverFd);
    }
    
    if (atoi(argv[1])==4){
    // 打开服务器管道用于发送退出
    mkfifo(FIFO_4,0777);
    int serverFd;
    if ((serverFd = open(FIFO_4, O_RDWR)) < 0) {
        perror("Failed to open server FIFO for writing");
        exit(1);
    }
    // 用户登录
    LOGIN loginUser;
    strcpy(loginUser.username, argv[2]);
    strcpy(loginUser.password, argv[3]);
    loginUser.op=atoi(argv[1]);
    int nwrite= write(serverFd,&loginUser,sizeof(loginUser));
    if(nwrite==-1)
	    printf("write error");
    //printf("%s \n %s \n %s",loginUser.username,loginUser.password,loginUser.myfifo);
    close(serverFd);
    }

}
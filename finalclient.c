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
    // �򿪷������ܵ����ڷ�������
    mkfifo(FIFO_1,0777);
    int serverFd = open(FIFO_1, O_RDWR);
       if(serverFd==-1){
        perror("Failed to open server FIFO for writing");
        exit(1);
    }
    char receive_path[100] = "recievemessage/";
    strcat(receive_path, argv[2]); // ʹ���û�����Ϊ�ܵ���
    mkfifo(receive_path, 0777);
    
    // �û�ע��
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
    
    //���е�¼����
    if (atoi(argv[1])==2){
    // �򿪷������ܵ����ڷ��͵�¼����
    mkfifo(FIFO_2,0777);
    int serverFd;
    if ((serverFd = open(FIFO_2, O_RDWR)) < 0) {
        perror("Failed to open server FIFO for writing");
        exit(1);
    }
    char receive_path[100] = "recievemessage/";
    strcat(receive_path, argv[2]); // ʹ���û�����Ϊ�ܵ���
    mkfifo(receive_path, 0777);
    // �û���¼
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
    //��Ϣ����
    if (atoi(argv[1])==3){
    // �򿪷������ܵ����ڷ���������Ϣ
   
    mkfifo(FIFO_3,0777);
    int serverFd;
    if( (serverFd = open(FIFO_3, O_RDWR)) < 0) {
        perror("Failed to open server FIFO for writing");
        exit(1);
    
    }
    // ����������Ϣ�������û�
    MESSAGE sendmessage;char sendname[50];char messages[100];
    sendmessage.op=atoi(argv[1]);
    int sumpeople=atoi(argv[2]);//���뷢������
    strcpy(sendname,argv[3]);//��������
    strcpy(messages,argv[4]);//��Ϣ
    for(int i=0;i<sumpeople;i++)
    {
      MESSAGE sendmessage;
      strcpy(sendmessage.destname,argv[i+5]);//Ŀ������
      strcpy(sendmessage.myfifo,sendname);//Ŀ������
      strcpy(sendmessage.message,messages);//Ŀ������
      int nwrite= write(serverFd,&sendmessage,sizeof(sendmessage));
      if(nwrite<=0)
	    printf("write error");
    }
	  
    close(serverFd);
    }
    
    if (atoi(argv[1])==4){
    // �򿪷������ܵ����ڷ����˳�
    mkfifo(FIFO_4,0777);
    int serverFd;
    if ((serverFd = open(FIFO_4, O_RDWR)) < 0) {
        perror("Failed to open server FIFO for writing");
        exit(1);
    }
    // �û���¼
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
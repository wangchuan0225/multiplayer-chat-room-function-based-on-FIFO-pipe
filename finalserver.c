#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include "clientinfo.h"
#define LOGFILES_DIRECTORY "/var/log/chat-logs/"
#define FIFO_1 "sendmessage/REG_FIFO"
#define FIFO_2 "sendmessage/LOGIN_FIFO"
#define FIFO_3 "sendmessage/MSG_FIFO"
#define FIFO_4 "sendmessage/LOGOUT_FIFO"
#define MAX_ONLINE_USERS 4

REGISTER users[10];
  
int numUsers = 0;
char buffer[100];
int useable[MAX_ONLINE_USERS];
int fdl;

int read_log_entries(const char target_receiver[50], int max_entries) 
{
     LogEntry entries[10];
    const char file_path[50] = "/var/log/chat-logs/server.log";
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("�޷����ļ� %s\n", file_path);
        return -1;
    }

    char line[MAX_LINE_LENGTH];

    int num_entries = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL && num_entries < max_entries) {
        // ɾ����ĩβ�Ļ��з�������еĻ���
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // �������е�����
        char *token = strtok(line, ",");
        int count = 0;
        char last_word[10];
        char receiver[50];
        char sender[50];
        char message[100];
        while (token != NULL && count < 5) {
            switch (count) {
                case 1:
                    strcpy(sender,token);
                    break;
                case 2:
                    strcpy(receiver, token);
                    break;
                case 3:
                    strcpy(message,token);
                case 4:
                    strcpy(last_word , token);
                    break;
            }
            token = strtok(NULL, ",");
            count++;
        }

        if (count == 5 && strcmp(receiver, target_receiver) == 0 &&  strcmp(last_word, "fail") == 0) {
           strcpy(entries[num_entries].sender, sender);
           strcpy(entries[num_entries].receiver, receiver);
            strcpy(entries[num_entries].message, message);
            for (int i = 0; i < strlen(entries[num_entries].message); ++i) {
              if (entries[num_entries].message[i] == '/') {
                  entries[num_entries].message[i] = ' ';
                }
              }
            strcpy(entries[num_entries].status, last_word);
            num_entries++;
        }
    }
    char targetfifo[50];
    
     for(int i=0;i<numUsers;i++)
	     {
		    if(strcmp(users[i].username,entries[num_entries-1].receiver)==0)
	        {	strcpy(targetfifo, users[i].myfifo);
			      break;
		      } 
        }//�ҵ��ռ����ܵ�
    for(int i=0;i<num_entries;i++)
    {
         sprintf(buffer,"user %s send you a message %s \n",entries[i].sender,entries[i].message);
         fdl=open(targetfifo,O_WRONLY|O_NONBLOCK);
         write(fdl,buffer,strlen(buffer)+1);
         close(fdl);
    }
    fclose(file);
    return num_entries;
}


int main()
{
  REGISTER onlineuser[MAX_ONLINE_USERS];
int nowuser=0;
for(int i=0;i<MAX_ONLINE_USERS;i++)
    useable[i]=-1;
memset(buffer,'\0',100);
int fdl;
fd_set readfds;
char buffer2[100];
memset(buffer2,'\0',100);
  signal(SIGSEGV, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  unlink(FIFO_1);
  unlink(FIFO_2);
  unlink(FIFO_3);
  unlink(FIFO_4);
  mkfifo(FIFO_1, 0777);
  mkfifo(FIFO_2, 0777);
  mkfifo(FIFO_3, 0777);
  mkfifo(FIFO_4,0777);
  //����ÿ���ܵ��ҵ�����Ϣ��
  while (1) {
   char time_str[26];
        // �ȴ���һ�ܵ���������
	int fd1=open(FIFO_1,O_RDONLY|O_NONBLOCK);
	int fd2=open(FIFO_2,O_RDONLY|O_NONBLOCK);
	int fd3=open(FIFO_3,O_RDONLY|O_NONBLOCK);
  int fd4=open(FIFO_4,O_RDONLY|O_NONBLOCK);
  FD_ZERO(&readfds);
  FD_SET(fd1, &readfds);
  FD_SET(fd2, &readfds);
  FD_SET(fd3, &readfds);
  FD_SET(fd4,&readfds);
  int fd=fd1;
 	if(fd<fd2)
	fd=fd2;
	if(fd<fd3)
	fd=fd3;
  if(fd<fd4)
  fd=fd4;
	int sel = select(fd + 1, &readfds, NULL, NULL, NULL);
  if (sel < 0) {
    perror("select");
    return -1;
    }

  if (FD_ISSET(fd1,&readfds)) {
    // �����û�ע������
    REGISTER newUser;
    read(fd1, &newUser, sizeof(newUser));
    printf("get new ");
            // ����Ƿ��Ѿ�������ͬ�û���
    int userExists = 0;
    for (int i = 0; i < numUsers; i++) {
      if (strcmp(users[i].username, newUser.username) == 0) {
        userExists = 1;//�ж��û����Ƿ����
        break;
        }
      }

    if (!userExists) {
                // ע�����û�
      users[numUsers] = newUser;
      numUsers++;
      sprintf(buffer,"Registered user: %s\n",newUser.username);
              //  printf("Registered user: %s\n", newUser.username);
		  int fdl=open(newUser.myfifo,O_WRONLY|O_NONBLOCK);
		  if(fdl<0)
		  perror("open");
		  write(fdl,buffer,strlen(buffer)+1);
		  printf("Register user : %s\n numUser= %d \n",newUser.username,numUsers);
		  close(fdl);
      get_current_time(time_str);
      write_to_log(newUser.username, "Registration,", time_str);//д����־
      }
    else { 
		  sprintf(buffer," user %s already exists\n",newUser.username);
      int fdl=open(newUser.myfifo,O_WRONLY|O_NONBLOCK);
      write(fdl,buffer,strlen(buffer)+1);
		  close(fdl);
      printf("Username already exists: %s\n", newUser.username);
         }
      close(fd1);
    }

    if (FD_ISSET(fd2,&readfds)) {
            // �����û���¼����
      LOGIN loginUser;
      read(fd2, &loginUser, sizeof(loginUser));
      int userAuthenticated = -1;
      for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].username, loginUser.username) == 0 &&//�ж��Ƿ����û�����
        strcmp(users[i].password, loginUser.password) == 0 && nowuser!=4) {
          userAuthenticated = i;//�鿴�Ƿ��������û�
          break;
            }
          }
      for(int i=0;i<MAX_ONLINE_USERS;i++)
        if(useable[i]==userAuthenticated)
          userAuthenticated=-1;//��ֹ�ظ���¼

      if (userAuthenticated!=-1) {
        sprintf(buffer,"user logged in: %s, online user is :%d\n",loginUser.username,nowuser+1);
        for(int i=0;i<MAX_ONLINE_USERS;i++)
        {
          if(useable[i]!=-1)//����λ�����˵�¼ʱ����¼��Ϣ��������
          {
             fdl=open(onlineuser[i].myfifo,O_WRONLY|O_NONBLOCK);
             write(fdl,buffer,strlen(buffer)+1);
             close(fdl);
          }
        }
		    fdl=open(loginUser.myfifo,O_WRONLY|O_NONBLOCK);
		    write(fdl,buffer,strlen(buffer)+1);
        int isempty=0;
        for(int i=0;i<MAX_ONLINE_USERS;i++)
        {
          if(useable[i]==-1)
            {isempty=i;break;}//�ҵ���λ���µ�¼�ļ���
        }
        useable[isempty]=userAuthenticated;
        onlineuser[isempty]=users[userAuthenticated];
        nowuser++;
		    close(fdl);
        get_current_time(time_str);
        write_to_log(loginUser.username, "Login,", time_str);//д����־
        printf("User logged in: %s\n", loginUser.username);
                // ���û����͵�¼�ɹ���Ϣ
         // ������ദ�� 100 ����־��Ŀ
        int num_entries = read_log_entries( loginUser.username, 100);
        printf("%d\n",num_entries);
        } 
      else {
        sprintf(buffer,"login failed : %s\n",loginUser.username);
        fdl=open(loginUser.myfifo,O_RDWR|O_NONBLOCK);
        int nwrite= write(fdl,buffer,strlen(buffer)+1);
		    if(nwrite<0)
        perror("write");
        close(fdl);
        printf("Login failed for user: %s\n", loginUser.username);
                // ���û����͵�¼ʧ����Ϣ
            }

          close(fd2);
        }

    if (FD_ISSET(fd3,&readfds)) {
            // ������������
           // ��ȡ��Ϣ��Ŀ���û����û�����Ȼ��ת����Ϣ��Ŀ���û��Ĺܵ�
	    printf("get message\n");
      char targetfifo[50];
      char sourcefifo[50];
	    MESSAGE sendmessage;
	    read(fd3,&sendmessage,sizeof(sendmessage));
	    memset(targetfifo,'\0',50);int destonline=0;
      
      for(int i=0;i<MAX_ONLINE_USERS;i++)
      {
        if(useable[i]!=-1&&(strcmp(onlineuser[i].username,sendmessage.myfifo)==0))
          {destonline+=1;break;}
      }//�жϷ������Ƿ�����
      for(int i=0;i<MAX_ONLINE_USERS;i++)
      {
        if(useable[i]!=-1&&(strcmp(onlineuser[i].username,sendmessage.destname)==0))
          {destonline+=2;break;}
      }//�ж��ռ����Ƿ�����
	     for(int i=0;i<numUsers;i++)
	     {
		    if(strcmp(users[i].username,sendmessage.destname)==0)
	        {	strcpy(targetfifo, users[i].myfifo);
			      break;
		      } 
        }//�ҵ��ռ����ܵ�
       for(int i=0;i<numUsers;i++)
	     {
		    if(strcmp(users[i].username,sendmessage.myfifo)==0)
	        {	strcpy(sourcefifo, users[i].myfifo);
			      break;
		      } 
        }//�ҵ��������ܵ�
       if(destonline==3)
       { 
	       sprintf(buffer,"user %s send you a message %s \n",sendmessage.myfifo,sendmessage.message);
         fdl=open(targetfifo,O_WRONLY|O_NONBLOCK);
         write(fdl,buffer,strlen(buffer)+1);
	       printf("%s\n",buffer);
         close(fdl);
         char name[200];
         strcpy(name,sendmessage.myfifo);
         strcat(name,",");
         strcat(name,sendmessage.destname);
         strcat(name,",sendmessage success,");
         get_current_time(time_str);
         write_to_log(sendmessage.myfifo, name,time_str);
         write_to_log(sendmessage.destname, name, time_str);//д����־
        }//�����˺��ռ��˶�����
        else if(destonline==1)
        {
          sprintf(buffer,"user %s isn't online\n",sendmessage.destname);
          fdl=open(sourcefifo,O_WRONLY|O_NONBLOCK);
          write(fdl,buffer,strlen(buffer)+1);
          close(fdl);
          char name[200];
          strcpy(name,sendmessage.myfifo);
          strcat(name,",");
          strcat(name,sendmessage.destname);
          strcat(name,",");
          strcat(name,sendmessage.message);
          strcat(name,",fail");
          write_to_log("server", name,"\0");
        }//�ռ��˲�����
        close(fd3);
   }     
        
        
        //�����˳�����
      if(FD_ISSET(fd4,&readfds))
      {
         LOGIN loginUser;
      read(fd4, &loginUser, sizeof(loginUser));
      int userAuthenticated = -1;
      for (int i = 0; i < 4; i++) {
        if (strcmp(onlineuser[i].username, loginUser.username) == 0 &&
        strcmp(onlineuser[i].password, loginUser.password) == 0 ) {
          userAuthenticated = i;
          break;
            }
          }
          
      int temp=0;
      for(int i=0;i<MAX_ONLINE_USERS;i++)
        if(useable[i]==userAuthenticated)
          temp=1;//��ֹ�ظ��˳�
      if(temp==0)
        userAuthenticated=-1;
      if (userAuthenticated!=-1) {
        sprintf(buffer,"user logged out: %s\n",loginUser.username);
        useable[userAuthenticated]=-1;
        for(int i=0;i<MAX_ONLINE_USERS;i++)
        {
          if(useable[i]!=-1)//����λ�����ˣ��˳�ʱ����Ϣ��������
          {
             fdl=open(onlineuser[i].myfifo,O_WRONLY|O_NONBLOCK);
             write(fdl,buffer,strlen(buffer)+1);
             close(fdl);
             
          }
        }
        for(int i=0;i<MAX_ONLINE_USERS;i++)
        {
          if(useable[i]==-1)
            continue;
          char buffer2[100];
          sprintf(buffer2,"user exit: %s\n",onlineuser[i].username);
          for(int j=0;j<MAX_ONLINE_USERS;j++)
          {
            if(useable[j]==-1)
              continue;
            fdl=open(onlineuser[j].myfifo,O_WRONLY|O_NONBLOCK);
             write(fdl,buffer2,strlen(buffer)+1);
             printf("%s \n",buffer2);
             close(fdl);
          }
        }
		    fdl=open(loginUser.myfifo,O_WRONLY|O_NONBLOCK);
		    write(fdl,buffer,strlen(buffer)+1);
        useable[userAuthenticated]=-1;
        nowuser--;
		    close(fdl);
        get_current_time(time_str);
        write_to_log(loginUser.username, "Logout,", time_str);//д����־
        printf("User logged out: %s\n", loginUser.username);
                
            } 
      else {
        sprintf(buffer,"logout failed : %s\n",loginUser.username);
        fdl=open(loginUser.myfifo,O_RDWR|O_NONBLOCK);
        int nwrite= write(fdl,buffer,strlen(buffer)+1);
		    if(nwrite<0)
		      perror("write");
        close(fdl);
        printf("Logout failed for user: %s\n", loginUser.username);
                // ���û����͵�¼ʧ����Ϣ
            }

          close(fd2);
      }
    }
}

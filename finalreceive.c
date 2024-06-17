#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_BUF 200 // 定义消息缓冲区大小

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <receive_path>\n", argv[0]);
        return 1;
    }

    char receive_path[100];
    strcpy(receive_path, "recievemessage/");
    strcat(receive_path, argv[1]); // 使用命令行参数作为管道名

   

    fd_set read_fds;
    struct timeval timeout;

    while (1) {
         int receive_fd = open(receive_path, O_RDONLY); // 打开管道进行读取
    if (receive_fd == -1) {
        perror("Failed to open receive pipe for reading");
        return 1;
    }
        FD_ZERO(&read_fds);
        FD_SET(receive_fd, &read_fds);


        int activity = select(receive_fd + 1, &read_fds, NULL, NULL,NULL);

        if (activity == -1) {
            perror("select");
            break;
        } 
        else {
            if (FD_ISSET(receive_fd, &read_fds)) {
                char buffer[MAX_BUF];
                int num_bytes = read(receive_fd, buffer, sizeof(buffer));
                if (num_bytes > 0) {
                    buffer[num_bytes] = '\0'; // 添加字符串结束符
                    printf("\nReceived message: %s\n", buffer);
                } 
               
            }
        }
    close(receive_fd);
    }

    
    return 0;
}

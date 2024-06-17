#ifndef _CLIENTINFO_H
#define _CLIENTINFO_H
#define MAX_LINE_LENGTH 1024
#define LOGFILES_DIRECTORY "/var/log/chat-logs/"
#define MAX_POOL_SIZE 4
typedef struct{
	char myfifo[100];
	char username[50];
	char password[50];
	int op;
}REGISTER,LOGIN,*CLIENTREGISTER;

typedef struct{
int op;
char myfifo[100];
char destname[100];
char message[100];
}MESSAGE;

typedef struct {
    char sender[50];
    char receiver[50];
    char message[100];
    char status[10];
} LogEntry;


void write_to_log(const char *username, const char *event, const char *time) {
    char log_file_path[100];
    snprintf(log_file_path, sizeof(log_file_path), "%s%s.log", LOGFILES_DIRECTORY, username);

    FILE *log_file = fopen(log_file_path, "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(1);
    }

    fprintf(log_file, "%s,%s%s\n", username, event, time);
    fclose(log_file);
}

void get_current_time(char *time_str) {
    time_t rawtime;
    struct tm timeinfo;

    time(&rawtime);
    localtime_r(&rawtime, &timeinfo);

    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", &timeinfo);
}
//�߳̽ṹ
typedef struct {
    pthread_t tid;
    int status; // 0 ��ʾ���У�1 ��ʾæµ
    // �����߳���Ϣ
} Thread;

// �̳߳ؽṹ
typedef struct {
    int poolSize;
    Thread threads[MAX_POOL_SIZE];
    // �����̳߳���Ϣ
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ThreadPool;

ThreadPool initializeThreadPool() {
    ThreadPool pool;
    pool.poolSize = MAX_POOL_SIZE;

    // ��ʼ������������������
    pthread_mutex_init(&pool.mutex, NULL);
    pthread_cond_init(&pool.cond, NULL);

    // �����̳߳��ڵ�ÿ���߳�
    for (int i = 0; i < pool.poolSize; ++i) {
        pool.threads[i].status = 0; // �����߳�Ϊ����״̬
        // ����Ϊ tid �����ڴ棬�� pthread_create �����߳�ʱ�Զ�����
    }
    return pool;
}

#endif


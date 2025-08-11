#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define FTOK_PATH "msgqueue.key"
#define FTOK_PROJ_ID 60
#define MSG_TYPE 1

struct mydata {

    char data[2048];
};

struct message {
    long msg_type;
    char data[];
};

int get_msg_queue_id() 
{
    FILE *fp = fopen(FTOK_PATH, "a+");
    if (!fp) 
    {
        perror("Failed to create/access ftok file");
        exit(1);
    }
    fclose(fp);

    key_t key = ftok(FTOK_PATH, FTOK_PROJ_ID);
    if (key == -1) 
    {
        perror("ftok failed");
        exit(1);
    }

    int msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) 
    {
        perror("msgget failed");
        exit(1);
    }

    return msgid;
}

int main() {
    int msgid = get_msg_queue_id();

    while (1) {
        struct mydata user_data;

        char temp[10];
        printf("Sending Data (y/n):");
        fgets(temp, sizeof(temp), stdin);
        if (temp[0] == 'n' || temp[0] == 'N')
            break; 
            
        printf("Enter description:");
        fgets(user_data.data, sizeof(user_data.data), stdin);
        user_data.data[strcspn(user_data.data, "\n")] = '\0';

        size_t data_size = sizeof(struct mydata);
        size_t msg_size = sizeof(struct message) + data_size;

        struct message *msg = malloc(msg_size);
        if (!msg)
        {
            perror("malloc failed");
            exit(1);
        }

        msg->msg_type = MSG_TYPE;
        memcpy(msg->data, &user_data, data_size);

        if (msgsnd(msgid, msg, data_size, 0) == -1) 
        {
            perror("msgsnd failed");
            free(msg);
            exit(1);
        }

        printf("Data sent to queue\n");

        free(msg);

        // message receive  immediately
        struct message *recv_msg = malloc(msg_size);
        if (!recv_msg) 
        {
            perror("malloc for receive failed");
            exit(1);
        }
         if (msgrcv(msgid, recv_msg, data_size, MSG_TYPE, 0) == -1)
        {
            perror("msgrcv failed");
            free(recv_msg);
            exit(1);
        }

        struct mydata received;
        memcpy(&received, recv_msg->data, data_size);
        printf("Received Data:%s\n", received.data);
        free(recv_msg);
    }

    // Delete the queue
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("Failed to delete message queue");
        exit(1);
    }
    printf("Deleted Message queue\n");
    return 0;
}

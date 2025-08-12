#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define MAX_SIZE 2048
#define MSG_TYPE 1
#define FTOK_PATH "msgqueue.key"  
#define FTOK_PROJ_ID 70

// Define the message structure
struct message {
    long msg_type;
    char msg_text[MAX_SIZE];
};

//  generate message queue ID
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

    struct message msg;
    msg.msg_type = MSG_TYPE;

    
    printf("Enter message to send: ");//send the message
    if (fgets(msg.msg_text, MAX_SIZE, stdin) == NULL) 
    {
        fprintf(stderr, "No input provided.\n");
        exit(1);
    }

     //Remove newline if present
    size_t len = strlen(msg.msg_text);


    // Send the message
    if (msgsnd(msgid, &msg, len , 0) == -1) {
        perror("msgsnd failed");
        exit(1);
    }

    printf("Message is sent: %s", msg.msg_text);

    // received the Immediately message 
    if (msgrcv(msgid, &msg, MAX_SIZE, MSG_TYPE, 0) == -1) {
        perror("Message received failed");
        exit(1);
    }

    printf("Message received: %s", msg.msg_text);

    
    if (msgctl(msgid, IPC_RMID, NULL) == -1) 
    {
        perror("msgctl (remove queue) failed");
        exit(1);
    }

    printf("deleted Message queue.\n");

    return 0;
}

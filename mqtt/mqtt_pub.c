#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "C_Publisher_Input"
#define TOPIC       "my/test/topic"
#define QOS         1
#define TIMEOUT     10000L
#define MAX_PAYLOAD 2048

int main() {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    char payload[MAX_PAYLOAD + 1];  // +1 for null terminator
    printf("Enter the data:");

    // Read user input (can include spaces)
    fgets(payload, sizeof(payload), stdin);

    // Remove newline if present
    size_t len = strlen(payload);
    if (payload[len - 1] == '\n') {
        payload[len - 1] = '\0';
        len--;
    }

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = (int)strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);

    printf("Publishing message to topic '%s':", TOPIC);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message delivered (%d bytes).\n", pubmsg.payloadlen);

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}

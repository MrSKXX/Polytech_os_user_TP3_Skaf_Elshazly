#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "creme.h"

#define PORT 9998
#define LBUF 1024

static int server_pid = -1;

void beuip_start(char *pseudo) {
    if (server_pid != -1) return;
    server_pid = fork();
    if (server_pid == 0) {
        execl("./servbeuip", "servbeuip", pseudo, NULL);
        exit(1);
    }
}

void beuip_stop(void) {
    if (server_pid != -1) {
        kill(server_pid, SIGTERM);
        server_pid = -1;
    }
}

static void send_to_local(char *msg_out, int msg_len) {
    int sid;
    struct sockaddr_in Sock;
    if ((sid=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) < 0) return;
    bzero(&Sock, sizeof(Sock));
    Sock.sin_family = AF_INET;
    Sock.sin_addr.s_addr = inet_addr("127.0.0.1");
    Sock.sin_port = htons(PORT);
    sendto(sid, msg_out, msg_len, 0, (struct sockaddr *)&Sock, sizeof(Sock));
    close(sid);
}

void beuip_liste(void) {
    char msg_out[16];
    sprintf(msg_out, "3BEUIP");
    send_to_local(msg_out, 6);
}

void beuip_mess_pseudo(char *target, char *msg) {
    char msg_out[LBUF];
    int offset = sprintf(msg_out, "4BEUIP%s", target);
    msg_out[offset] = '\0';
    offset++;
    strcpy(msg_out + offset, msg);
    send_to_local(msg_out, offset + strlen(msg));
}

void beuip_mess_all(char *msg) {
    char msg_out[LBUF];
    sprintf(msg_out, "5BEUIP%s", msg);
    send_to_local(msg_out, strlen(msg_out));
}
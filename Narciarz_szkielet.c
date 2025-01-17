#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <errno.h>
#include <string.h>

struct Karnet {
    int id;
    int vip_status;  // 1 - vip, 0 - zwykly
    int childs;
    double time;
};

// Struktura do kolejki
struct msgBuf {
    long mtype;          
    struct Karnet karnet;  
};

// Czas trwania petli (sekundy)
int TK = 10;

int main() {
    time_t start_time, current_time;
    time(&start_time);


    int msgid = msgget(555, 0666);
    if (msgid == -1) {
        perror("[Narciarz] Blad msgget (otwieranie kolejki)");
        exit(EXIT_FAILURE);
    }

    printf("[Narciarz PID=%d] Otwarta kolejka komunikatow ID=%d\n", getpid(), msgid);

    while (1) {
        time(&current_time);
        if (difftime(current_time, start_time) >= TK) {
            break;
        }

        struct msgBuf buf;
        if (msgrcv(msgid, &buf, sizeof(buf.karnet), 0, 0) == -1) {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EIDRM) {
                break;
            }
            perror("[Narciarz] Blad msgrcv");
            break;
        }
        sleep(3);

        printf("[Narciarz PID=%d] Odczyt Karnet: ID=%d, VIP=%d, Childs=%d, Time=%.2f\n",
               getpid(),
               buf.karnet.id,
               buf.karnet.vip_status,
               buf.karnet.childs,
               buf.karnet.time);
    }
    return 0;
}


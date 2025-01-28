#include "queue.h"

int utworz_kolejke(int key) {
    int id = msgget(key, 0600 | IPC_CREAT);
    if (id == -1) {
        perror("Nie moglem utworzyc kolejki komunikatow");
        exit(EXIT_FAILURE);
    }
    return id;
}

void wyslij_karnet_do_kolejki(int msgid, struct Karnet *k) {
    struct msgBuf msg;
    msg.mtype = 1;
    msg.kar = *k;

    if (msgsnd(msgid, &msg, sizeof(msg.kar), 0) == -1) {
        perror("msgsnd blad");
        exit(EXIT_FAILURE);
    }
}

void usun_kolejke(int msgid) {
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Nie moglem usunac kolejki komunikatow");
        exit(EXIT_FAILURE);
    }
    printf("Kolejka komunikatow (ID: %d) zostala usunieta.\n", msgid);
}

int dodaj_kolejke(int key) {
    int id = msgget(key, 0600);
    if (id == -1) {
        perror("Nie moglem dodac kolejki komunikatow");
        exit(EXIT_FAILURE);
    }
    return id;
}

#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include "structs.h"


int utworz_kolejke(int key) {
    int id = msgget(key, 0666 | IPC_CREAT);
    if (id == -1) {
        perror("Nie moglem utworzyc kolejki komunikatow");
        exit(EXIT_FAILURE);
    }
    // printf("Utworzono kolejke komunikatow o ID: %d\n", id);
    return id;
}
void wyslij_karnet_do_kolejki(int msgid, struct Karnet *k) {
    struct msgBuf msg;
    // Ustawiamy typ komunikatu (dowolna liczba > 0)
    msg.mtype = 1;
    msg.kar = *k;   // skopiuj zawartość Karnetu

    // Wysyłamy przez msgsnd
    if (msgsnd(msgid, &msg, sizeof(msg.kar), 0) == -1) {
        perror("msgsnd blad");
        exit(EXIT_FAILURE);
    }
    // Można wyświetlić info, że wysłano
    // printf("Wyslano Karnet o ID: %d do kolejki.\n", k->id);
}
void usun_kolejke(int msgid) {
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Nie moglem usunac kolejki komunikatow");
        exit(EXIT_FAILURE);
    }
    printf("Kolejka komunikatow (ID: %d) zostala usunieta.\n", msgid);
}
int odbierz_kolejke() {
    int msgid = msgget(555, 0666);
    if (msgid == -1) {
        perror("[Narciarz] Blad msgget (otwieranie kolejki)");
        exit(EXIT_FAILURE);
    }
    return msgid;
}

#endif //QUEUE_H
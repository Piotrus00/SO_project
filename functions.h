#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <errno.h>

//  zmienne globalne zdefiniowane gdzies w plikach
extern int TK;
extern int pamiec;
extern int odlaczenie1;
extern int odlaczenie2;
extern int *adres;


int generateRandomNumber(int min, int max) {
    return min + rand() % (max - min + 1);
}
double ticket_price(double total_price, double time, int age, int vip, int dzieci) {
    int p = 50;    // stala cena za godzine = 50zl
    int vip_p = 200; // cena za godzine z vipem = 200zl
    int vip_or_not_vip = (vip) ? vip_p : p;

    if (age < 12 || age > 65) {
        total_price += vip_or_not_vip * 0.75 * time;
    } else {
        total_price += vip_or_not_vip * time;
    }
    for (int i = 1; i <= dzieci; i++) {
        total_price += vip_or_not_vip * 0.75 * time;
    }
    return total_price;
}

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



#endif //FUNCTIONS_H

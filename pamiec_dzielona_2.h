//
// Created by kali on 1/23/25.
//

#ifndef PAMIEC_DZIELONA2_H
#define PAMIEC_DZIELONA2_H


#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>

extern int pamiec2;
extern int odlaczenie3;
extern int odlaczenie4;
extern int *adres2;


void upd2(int key) {
    pamiec2 = shmget(key, 12, 0600 | IPC_CREAT);
    if (pamiec2 == -1) {
        printf("Problemy z utworzeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
    // else {
    // printf("Pamiec dzielona zostala utworzona : %d\n", pamiec);
    // }
}
void upa2() {
    adres2 = (int *)shmat(pamiec2, NULL, 0);
    if (adres2 == (int *)(-1)) {
        printf("Problem z przydzieleniem adresu.\n");
        exit(EXIT_FAILURE);
    }
    // else {
    //     printf("Przestrzen adresowa zostala przyznana : %p\n", (void*)adres);
    // }
}
void odlacz_pamiec2() {
    odlaczenie3 = shmctl(pamiec2, IPC_RMID, 0);
    sleep(1);
    odlaczenie4 = shmdt(adres2);
    if (odlaczenie3== -1 || odlaczenie4 == -1) {
        printf("Problemy z odlaczeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Pamiec dzielona zostala odlaczona.\n");
    }
}
#endif //PAMIEC_DZIELONA2_H

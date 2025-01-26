//
// Created by kali on 1/17/25.
//

#ifndef PAMIEC_DZIELONA_H
#define PAMIEC_DZIELONA_H


#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>

extern int pamiec;
extern int odlaczenie1;
extern int odlaczenie2;
extern int *adres;



void upd(int key, int size) {
    pamiec = shmget(key, size, 0600 | IPC_CREAT);
    if (pamiec == -1) {
        printf("Problemy z utworzeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
    // else {
    // printf("Pamiec dzielona zostala utworzona : %d\n", pamiec);
    // }
}
void upa() {
    adres = (int *)shmat(pamiec, NULL, 0);
    if (adres == (int *)(-1)) {
        printf("Problem z przydzieleniem adresu.\n");
        exit(EXIT_FAILURE);
    }
    // else {
    //     printf("Przestrzen adresowa zostala przyznana : %p\n", (void*)adres);
    // }
}
void odlacz_pamiec() {
    odlaczenie1 = shmctl(pamiec, IPC_RMID, 0);
    sleep(1);
    odlaczenie2 = shmdt(adres);
    if (odlaczenie1 == -1 || odlaczenie2 == -1) {
        printf("Problemy z odlaczeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Pamiec dzielona zostala odlaczona.\n");
    }
}

void upd_nietworz(int key, int size) {
    pamiec = shmget(key, size, 0600);
    if (pamiec == -1) {
        printf("Problemy z utworzeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
    // else {
    // printf("Pamiec dzielona zostala utworzona : %d\n", pamiec);
    // }
}
#endif //PAMIEC_DZIELONA_H

#include "pamiec_dzielona.h"

void upd(int key, int size) {
    pamiec = shmget(key, size, 0600 | IPC_CREAT);
    if (pamiec == -1) {
        printf("Problemy z utworzeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
}

void upa() {
    adres = (int *)shmat(pamiec, NULL, 0);
    if (adres == (int *)(-1)) {
        printf("Problem z przydzieleniem adresu.\n");
        exit(EXIT_FAILURE);
    }
}

void odlacz_pamiec() {
    odlaczenie1 = shmctl(pamiec, IPC_RMID, 0);
    sleep(1);
    odlaczenie2 = shmdt(adres);
    if (odlaczenie1 == -1 || odlaczenie2 == -1) {
        printf("Problemy z odlaczeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec dzielona zostala odlaczona.\n");
    }
}

void upd_nietworz(int key, int size) {
    pamiec = shmget(key, size, 0600);
    if (pamiec == -1) {
        printf("Problemy z utworzeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
}

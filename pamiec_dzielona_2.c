#include "pamiec_dzielona_2.h"

void upd2(int key) {
    pamiec2 = shmget(key, 12, 0600 | IPC_CREAT);
    if (pamiec2 == -1) {
        printf("Problemy z utworzeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
}

void upa2() {
    adres2 = (int *)shmat(pamiec2, NULL, 0);
    if (adres2 == (int *)(-1)) {
        printf("Problem z przydzieleniem adresu.\n");
        exit(EXIT_FAILURE);
    }
}

void odlacz_pamiec2() {
    odlaczenie3 = shmctl(pamiec2, IPC_RMID, 0);
    sleep(1);
    odlaczenie4 = shmdt(adres2);
    if (odlaczenie3 == -1 || odlaczenie4 == -1) {
        printf("Problemy z odlaczeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec dzielona zostala odlaczona.\n");
    }
}

#include "semafory.h"

int utworz_nowy_semafor(int key, int nsems) {
    int semafor = semget(key, nsems, 0600 | IPC_CREAT);
    if (semafor == -1) {
        printf("Nie moglem utworzyc nowego semafora.\n");
        exit(EXIT_FAILURE);
    }
    return semafor;
}

void semafor_p(int semafor, int sem_num) {
    struct sembuf bufor_sem = {sem_num, -1, 0};
    if (semop(semafor, &bufor_sem, 1) == -1) {
        if (errno == EINTR) {
            semafor_p(semafor, sem_num);
        } else {
            exit(EXIT_FAILURE);
        }
    }
}

void semafor_v(int semafor, int sem_num, int sem_op) {
    struct sembuf bufor_sem = {sem_num, sem_op, 0};
    if (semop(semafor, &bufor_sem, 1) == -1) {
        printf("Nie moglem otworzyc semafora.\n");
        exit(EXIT_FAILURE);
    }
}

void usun_semafor(int semafor, int sem_num) {
    if (semctl(semafor, sem_num, IPC_RMID) == -1) {
        printf("Nie mozna usunac semafora.\n");
        exit(EXIT_FAILURE);
    }
    printf("Semafor zostal usuniety.\n");
}

int dodaj_nowy_semafor(int key, int nsems) {
    int semafor = semget(key, nsems, 0600);
    if (semafor == -1) {
        printf("Nie moglem dodac nowego semafora.\n");
        exit(EXIT_FAILURE);
    }
    return semafor;
}

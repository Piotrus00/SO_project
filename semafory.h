#ifndef SEMAFORY_H
#define SEMAFORY_H


#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <errno.h>


int utworz_nowy_semafor(int key, int nsems) {
    int semafor;
    semafor = semget(key, nsems, 0600 | IPC_CREAT);
    if (semafor == -1) {
        printf("Nie moglem utworzyc nowego semafora.\n");
        exit(EXIT_FAILURE);
    }
    // else {
    //     printf("Semafor zostal utworzony : %d\n", semafor);
    // }
    return semafor;
}


void semafor_p(int semafor, int sem_num) {
    int zmien_sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num = sem_num;
    bufor_sem.sem_op  = -1;
    bufor_sem.sem_flg = 0;

    zmien_sem = semop(semafor, &bufor_sem, 1);
    if (zmien_sem == -1) {
        if (errno == EINTR) {
            semafor_p(semafor, sem_num);
        } else {
            exit(EXIT_FAILURE);
        }
    }
}


void semafor_v(int semafor, int sem_num, int sem_op) {
    int zmien_sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num = sem_num;
    bufor_sem.sem_op  = sem_op;
    bufor_sem.sem_flg = 0;

    zmien_sem = semop(semafor, &bufor_sem, 1);
    if (zmien_sem == -1) {
        printf("Nie moglem otworzyc semafora.\n");
        exit(EXIT_FAILURE);
    }
}

void usun_semafor(int semafor, int sem_num) {
    int sem;
    sem = semctl(semafor, sem_num, IPC_RMID);
    if (sem == -1) {
        printf("[1]Nie mozna usunac semafora.\n");
        exit(EXIT_FAILURE);
    }
    printf("Semafor zostal usuniety : %d\n", sem);
}

int dodaj_nowy_semafor(int key, int nsems) {
    int semafor;
    semafor = semget(key, nsems, 0600);
    if (semafor == -1) {
        printf("Nie moglem dodac nowego semafora.\n");
        exit(EXIT_FAILURE);
    }
    return semafor;
}


#endif //SEMAFORY_H
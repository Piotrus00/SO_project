#ifndef SEMAFORY_H
#define SEMAFORY_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <errno.h>

int utworz_nowy_semafor(int key, int nsems);
void semafor_p(int semafor, int sem_num);
void semafor_v(int semafor, int sem_num, int sem_op);
void usun_semafor(int semafor, int sem_num);
int dodaj_nowy_semafor(int key, int nsems);

#endif // SEMAFORY_H

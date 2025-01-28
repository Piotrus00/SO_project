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

void upd2(int key);
void upa2();
void odlacz_pamiec2();

#endif // PAMIEC_DZIELONA2_H

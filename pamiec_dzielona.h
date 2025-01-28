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

void upd(int key, int size);
void upa();
void odlacz_pamiec();
void upd_nietworz(int key, int size);

#endif // PAMIEC_DZIELONA_H

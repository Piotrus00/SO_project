#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include "structs.h"

int utworz_kolejke(int key);
void wyslij_karnet_do_kolejki(int msgid, struct Karnet *k);
void usun_kolejke(int msgid);
int dodaj_kolejke(int key);

#endif // QUEUE_H

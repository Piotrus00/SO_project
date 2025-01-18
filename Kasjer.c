#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>

#include "structs.h"
#include "semafory.h"
#include "pamiec_dzielona.h"
#include "queue.h"
#include "functions_kasjer.h"

int TK=10;
int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;

int main() {
    struct Karnet karnet;
    key_t key_pd_kajser = 111;

    time_t start_time, current_time;
    time(&start_time);

    int sem1 = dodaj_nowy_semafor(123);
    int msgid = msgget(555, 0666);

    if (msgid == -1) {
        perror("[Kasjer] Blad msgget (otwieranie kolejki)");
        exit(EXIT_FAILURE);
    }

    int with_child[10] = {1, 1, 1, 1, 2, 1, 0, 0, 0, 0}; // ilość dzieci
    int ticket_type[5] = {4, 4, 6, 8, 24};               // czas trwania biletu
    int vip_chance[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}; // szanse na vipa

    int child, age, id = 0;
    double sum_price = 0;

    srand(time(NULL));
    upd(key_pd_kajser);
    upa();

    // Główna pętla:
    while (1) {
        time(&current_time);
        if (difftime(current_time, start_time) >= TK) {
            break;
        }

        // Generowanie przykładowych danych
        age = generateRandomNumber(9, 90);
        if (age > 18) {
            child = with_child[generateRandomNumber(0, 9)];
        } else {
            child = 0;
        }
        int type = ticket_type[generateRandomNumber(0, 4)];
        int vip  = vip_chance[generateRandomNumber(0, 9)];

        if (*adres != 0) {
            semafor_p(sem1, 0);
            int new_id = *adres + 1;
            *adres = new_id;
            id = *adres;
        } else {
            id = 1;
            *adres = id;
        }
        semafor_v(sem1, 0, 1);

        sum_price = ticket_price(sum_price, type, age, vip, child);


        karnet.id = id;
        karnet.vip_status = vip;
        karnet.childs = child;
        karnet.time = type;

        wyslij_karnet_do_kolejki(msgid, &karnet);

        sum_price = 0;

        // symulacja opóźnienia przed nastepnym klientem
        usleep(200000); // 0.2 sekundy

        // dodac plik tekstowy z danymi karnetu i cena do raportu tak to juz raczej gotowy
    }


    // Sprzątanie
    karnet.id = -1;
    karnet.vip_status = 0;
    karnet.childs = 0;
    karnet.time = 0;
    wyslij_karnet_do_kolejki(msgid, &karnet);
    odlacz_pamiec();
    return 0;
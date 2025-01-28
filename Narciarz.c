#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>

#include "queue.h"
#include "pamiec_dzielona.h"
#include "pamiec_dzielona_2.h"
#include "semafory.h"
#include "structs.h"

// Struktura do wysłania karnetu do kolejki VIP/normal

int N = 20;

int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;
int msgid_normal, msgid_vip;

int pamiec2;
int odlaczenie3;
int odlaczenie4;
int *adres2;

//funkcja losujaca liczbe z danego przedzialu
int generateRandomNumber(int min, int max);

//funkcja sprawdzajaca czas na karnecie
int czy_czas_na_karnecie(struct Karnet k);

//funkcja zapisujaca do plik dane karnetu przy wejsciu nowego narciarza
void zapis(struct Karnet k);

//funkcja zapisujaca godzine wyjscia ze stoku narciarza
void zapis_2(struct Karnet k);



int main(){
    //klucze
    key_t kol_kasjer_narciarz = 9000;
    key_t kol_nrm_narciarz_prac = 9001;
    key_t kol_vip_narciarz_prac = 9002;
    key_t kol_narciarz_krzeselka = 9004;

    key_t s_narciarz_narciarz = 356;
    key_t s_narciarz_pracownik = 357;

    key_t key_pd_narciarz_pracownik = 125;
    key_t pd_time_man = 202;


    //przylaczanie pamieci dzielonej

    //bez flagi IPC_CREAT moze byc duzo procesow dla pewnosci
    upd_nietworz(key_pd_narciarz_pracownik, 20);
    upa();
    upd2(pd_time_man);
    upa2();


    struct msgBuf buf;

    //dodawanie kolejek
    int msgid = dodaj_kolejke(kol_kasjer_narciarz);
    int msgid_vip = dodaj_kolejke(kol_vip_narciarz_prac);
    int msgid_normal = dodaj_kolejke(kol_nrm_narciarz_prac);
    int msgid_con = dodaj_kolejke(kol_narciarz_krzeselka);

    //dodawanie semaforow
    int s_crit_zone = dodaj_nowy_semafor(s_narciarz_narciarz, 3);
    int s_peron = dodaj_nowy_semafor(s_narciarz_pracownik, 1);

    //pobieramy dane narciarza
    if (msgrcv(msgid, &buf, sizeof(buf.kar), 0, 0) == -1){
        perror("[Narciarz] Blad msgrcv");
        exit(EXIT_FAILURE);
    }
    struct Karnet k = buf.kar;

    semafor_v(s_crit_zone,0,1);
    semafor_v(s_crit_zone,1,1);

    //ilosc bramek 4
    semctl(s_crit_zone, 2, SETVAL, 4);


    semafor_v(s_peron,0,1);

    //zapis karnetu nowego narciarza do pliku
    semafor_p(s_crit_zone,1);
    zapis(k);
    semafor_v(s_crit_zone,1,1);


    struct MsgNarciarz msg;
    memcpy(&msg.k, &k, sizeof(struct Karnet));


    // Ustalamy typ komunikatu (np. 1,2,3) zależnie od liczby dzieci
    if (k.childs == 2) {
        msg.mtype = 3;
    } else if (k.childs == 1) {
        msg.mtype = 2;
    } else {
        msg.mtype = 1;
    }
    int grupa = k.childs + 1;

    while (czy_czas_na_karnecie(k)) {

        //symulacja 4 bramek

         semafor_p(s_crit_zone, 2);

        // sprawdzamy dostepnosc miejsc na peronie
         while (1) {
             semafor_p(s_peron, 0);
             if (adres[3] + grupa <= N) {
                 adres[3] += grupa;
                 semafor_v(s_peron, 0, 1);
                 break;
             }
             semafor_v(s_peron,0,1);
             usleep(10000);
         }
        //jezeli miejsce na peronie sie zwolnilo

        // Wysyłamy do kolejki normalnej albo VIP
        semafor_p(s_crit_zone, 0);
        if (k.vip_status) {
            if (msgsnd(msgid_vip, &msg, sizeof(msg.k), 0) == -1) {
                perror("[Narciarz] msgsnd vip");
            }
        } else {
            if (msgsnd(msgid_normal, &msg, sizeof(msg.k), 0) == -1) {
                perror("[Narciarz] msgsnd normal");
            }
        }
        semafor_v(s_crit_zone,0,1);

        //gdy dostanie sie do kolejki to kolejni moga przejsc przez bramki
        semafor_v(s_crit_zone,2, 1);

        //Czekamy, aż ID narciarza pojawi się w kolejce komunikatow czyli wysiadzie z krzeselka na gorze
        struct MsgBufDone msgd;
        if (msgrcv(msgid_con, &msgd, sizeof(msgd.id), k.id, 0) == -1) {
            perror("msgrcv error");
            exit(EXIT_FAILURE);
        }

        //Zjazd!!!
    int chances[10] = {1,1,1,2,2,2,3,3,3,4};
    int pathing;
    pathing = chances[generateRandomNumber(0, 9)];
    if (pathing == 1) {
        //Trasa T1
        usleep(50000);
    }
    if (pathing == 2) {
        //Trasa T2
        usleep(70000);
    }
    if (pathing == 3) {
        //Trasa T3
        usleep(90000);
    }
    }

    semafor_p(s_crit_zone,1);
    zapis_2(k);
    semafor_v(s_crit_zone,1,1);
    //sprzatanie
    odlaczenie1 = shmdt(adres);
    if (odlaczenie1 == -1) {
        printf("Problemy z odlaczeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
    odlaczenie3 = shmdt(adres2);
    if (odlaczenie3 == -1) {
        printf("Problemy z odlaczeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}


int generateRandomNumber(int min, int max) {
    return min + rand() % (max - min + 1);
}

int czy_czas_na_karnecie(struct Karnet k) {
    if (adres2[0]==-1) {
        return 0;
    }
    if (k.hours < adres2[0] ||
        (k.hours == adres2[0] && k.min < adres2[1]) ||
        (k.hours == adres2[0] && k.min == adres2[1] && k.sec <= adres2[2])) {
        return 0;
        }
    return 1;
}

void zapis(struct Karnet k) {
    FILE *plik = fopen("karnet_dane.txt", "a");
    if (plik == NULL) {
        perror("Blad otwarcia pliku");
        exit(EXIT_FAILURE);
    }
    fprintf(plik, "ID: %d, VIP: %d, Godzina: %02d:%02d:%02d\n", k.id, k.vip_status, k.hours, k.min, k.sec);
    fclose(plik);
}

void zapis_2(struct Karnet k) {
    FILE *plik2 = fopen("karnet_wyjscie.txt", "a");
    if (plik2 == NULL) {
        perror("Blad otwarcia pliku");
        exit(EXIT_FAILURE);
    }
    fprintf(plik2, "ID: %d, VIP: %d, Godzina Karnet: %02d:%02d:%02d Godzina Wyjscia: %02d:%02d:%02d\n",
        k.id, k.vip_status,
        k.hours, k.min, k.sec,
        adres2[0], adres2[1], adres2[2]
        );
    fclose(plik2);
}



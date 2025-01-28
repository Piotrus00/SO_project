#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "pamiec_dzielona.h"
#include "pamiec_dzielona_2.h"
#include "queue.h"
#include "structs.h"
#include "semafory.h"

pthread_mutex_t mutex_queue = PTHREAD_MUTEX_INITIALIZER; // Mutex do synchronizacji dostępu do kolejki

int s_krzeselka_count;

volatile sig_atomic_t queueStopped = 0;

int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;

int pamiec2;
int odlaczenie3;
int odlaczenie4;
int *adres2;

//zapis do pliku id_narciarza - czas przejscia przez bramki
void zapis(struct Karnet k);

// Handler pracownika1 (prac1) – zatrzymuje kolejkę
void stop_queue_handler(int signo);

// Handler pracownika2 (prac2) – wznawia kolejkę
void start_queue_handler(int signo);

//watek odpowiedzialny za wjazd krzeselek
void* wjazd_krzeselek(void* args);

int main() {
    printf("Pracownik %d w gotowosci!\n", getpid());
    //klucze
    key_t kol_nrm_narciarz_prac = 9001;
    key_t kol_vip_narciarz_prac = 9002;
    key_t kol_narciarz_krzeselka = 9004;

    key_t key_pd_narciarz_pracownik = 125;

    key_t s_narciarz_pracownik = 357;
    key_t s_krzeselka_count_key = 359;

    key_t pd_time_man_202 = 202;

    //obsluga sygnalow
    signal(SIGUSR1, stop_queue_handler);
    signal(SIGUSR2, start_queue_handler);

    int tab[3] = {0,0,0};
    int i = 0;
    int remain;
    int grupa;
    int obieg_krzeselek=0;

    //przylaczenie/utworzenie pamieci dzielonej
    upd(key_pd_narciarz_pracownik, 20);
    upa();
    upd2(pd_time_man_202);
    upa2();
    //dodawanie kolejek
    int msgid_vip = dodaj_kolejke(kol_vip_narciarz_prac);
    int msgid_normal = dodaj_kolejke(kol_nrm_narciarz_prac);
    int msgid_con = dodaj_kolejke(kol_narciarz_krzeselka);

    //dodawanie semaforow
    int s_peron = dodaj_nowy_semafor(s_narciarz_pracownik, 1);
    semafor_v(s_peron, 0, 1);
    s_krzeselka_count = dodaj_nowy_semafor(s_krzeselka_count_key, 1);
    //ustawienie liczby krzeselek na 40
    semctl(s_krzeselka_count, 0, SETVAL, 40);

    //petla wykonujaca sie do zakonczenia czasu dzialania stacji i do momentu gdy peron jest pusty
    while (1){
        while (queueStopped) {
            sleep(1);
        }
        remain = 3;
        i = 0;
        grupa = 0;

        for (int z = 0; z < 3; z++) {
            tab[z] = 0;
        }

        struct MsgBuf2 buf;


        //odczyt z kolejki z prorytetem dla vipow
        //brani priorytetowo do krzeselek zgodnie z trescia zadania
        //kolejki odczytuja najpierw vipow i odejmuja od parametru remain czyli pozostalej ilosci osob mozliwych
        //do wpuszczenia na krzeselko poniewaz krzeselka sa 3 osobowe maksymalnie odczytuje 3 osoby i opuszcza petle
        //gdy kolejka jest pusta pobieramy dane z kolejki zwyklej (nie vip) na tej samej zasadzie
        while (remain >= 3 && msgrcv(msgid_vip, &buf, sizeof(buf.k), 3, IPC_NOWAIT) > 0) {
            tab[i++] = buf.k.id;
            remain -= 3;
            grupa += 1 + buf.k.childs;
            zapis(buf.k);
        }
        while (remain >= 2 && msgrcv(msgid_vip, &buf, sizeof(buf.k), 2, IPC_NOWAIT) > 0) {
            tab[i++] = buf.k.id;
            remain -= 2;
            grupa += 1 + buf.k.childs;
            zapis(buf.k);
        }
        while (remain >= 1 && msgrcv(msgid_vip, &buf, sizeof(buf.k), 1, IPC_NOWAIT) > 0) {
            tab[i++] = buf.k.id;
            remain -= 1;
            grupa += 1 + buf.k.childs;
            zapis(buf.k);
        }

        while (remain >= 3 && msgrcv(msgid_normal, &buf, sizeof(buf.k), 3, IPC_NOWAIT) > 0) {
            tab[i++] = buf.k.id;
            remain -= 3;
            grupa += 1 + buf.k.childs;
            zapis(buf.k);
        }
        while (remain >= 2 && msgrcv(msgid_normal, &buf, sizeof(buf.k), 2, IPC_NOWAIT) > 0) {
            tab[i++] = buf.k.id;
            remain -= 2;
            grupa += 1 + buf.k.childs;
            zapis(buf.k);
        }
        while (remain >= 1 && msgrcv(msgid_normal, &buf, sizeof(buf.k), 1, IPC_NOWAIT) > 0) {
            tab[i++] = buf.k.id;
            remain -= 1;
            grupa += 1 + buf.k.childs;
            zapis(buf.k);
        }

        //przed zakonczeniem pracownik czeka az krzeselka wykonaja 1 pelny obieg zeby napewno nikt nie zostal
        if (adres2[0]==-1 && adres[3]<1  && tab[0] == 0 && tab[1]==0 && tab[2]==0) {
            obieg_krzeselek++;
            if (obieg_krzeselek>=40) {
                break;
            }
        }


        //usuwamy osoby z peronu bo wsiadaja do krzeselek
        semafor_p(s_peron, 0);
        adres[3] -= grupa;
        semafor_v(s_peron, 0, 1);

        // Tworzymy strukturę dla wątku
        id_na_krzeselku *id_krzeselko = malloc(sizeof(id_na_krzeselku));
        if (!id_krzeselko) {
            perror("[Pracownik] malloc");
            exit(EXIT_FAILURE);
        }
        id_krzeselko->tablica[0] = tab[0];
        id_krzeselko->tablica[1] = tab[1];
        id_krzeselko->tablica[2] = tab[2];
        id_krzeselko->key = msgid_con;

        // Zanim wystartujemy wątek, „rezerwujemy” krzesłko do max 40
        //jezeli nie ma dostepnych krzeselek to narciarze czekaja az przyjada
        semafor_p(s_krzeselka_count, 0);
        // uruchamiamy watek krzeselko
        pthread_t jazda_krzeselko;
        int err = pthread_create(&jazda_krzeselko, NULL, wjazd_krzeselek, id_krzeselko);
        if (err) {
            fprintf(stderr, "[Pracownik] Błąd w pthread_create: %d\n", err);
        }
        pthread_detach(jazda_krzeselko);
    }
    sleep(5);  //zgodnie z trecia zadania i sprzatanie
    odlacz_pamiec();
    return 0;
}

void zapis(struct Karnet k) {
    FILE *plik = fopen("karnet_peron.txt", "a");
    if (plik == NULL) {
        perror("Blad otwarcia pliku");
        exit(EXIT_FAILURE);
    }
    fprintf(plik, "%d, VIP: %d, Godzina: %02d:%02d:%02d\n",
            k.id, k.vip_status, adres2[0], adres2[1], adres2[2]);
    fclose(plik);
}

void stop_queue_handler(int signo) {
    if (signo == SIGUSR1) {
        queueStopped = 1;
        printf("[prac1] Otrzymano SIGUSR1 -> Zatrzymuję kolejkę.\n");
    }
}

void start_queue_handler(int signo) {
    if (signo == SIGUSR2) {
        queueStopped = 0;
        printf("[prac2] Otrzymano SIGUSR2 -> Wznawiam kolejkę.\n");
    }
}

void* wjazd_krzeselek(void* args) {
    id_na_krzeselku *t = (id_na_krzeselku*) args;

    // Symulacja jazdy krzesełka
    for (int step = 0; step < 20; step++) {
        while (queueStopped) {
            usleep(20000);
        }
        usleep(50000);
    }
    printf("[krzeselko] jade!!\n");

    // Po dojechaniu na górę wysyłamy powiadomienia dla każdego
    // z 3 miejsc w krześle jezeli krzeselka nie sa puste (jeśli != 0)
    pthread_mutex_lock(&mutex_queue);
    for (int i = 0; i < 3; i++) {
        int occupant_id = t->tablica[i];
        if (occupant_id != 0) {
            // Tworzymy strukturę powiadomienia
            struct MsgBufDone msg;
            msg.mtype       = occupant_id;
            msg.id = occupant_id;

            // Synchronizacja dostępu do kolejki za pomocą mutexa
            if (msgsnd(t->key, &msg, sizeof(msg.id), 0) == -1) {
                perror("[Pracownik] msgsnd do narciarza (msgid_con)");
            }
        }
    }
    semafor_v(s_krzeselka_count, 0, 1);
    free(t);
    pthread_mutex_unlock(&mutex_queue);
    return NULL;
}

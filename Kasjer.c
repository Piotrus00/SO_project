#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/wait.h>

#include "structs.h"
#include "semafory.h"
#include "pamiec_dzielona.h"
#include "pamiec_dzielona_2.h"
#include "queue.h"
#include "functions_kasjer.h"

int Tk = 21;


int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;

int pamiec2;
int odlaczenie3;
int odlaczenie4;
int *adres2;


void exe_a_proces(const char *path, const char *name) {
    if (fork() == 0) {
        execl(path, name, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
}

void sigchld_handler(int sig) {
    int status;
    // Zbierz wszystkie zakończone procesy dziecko
    while (waitpid(-1, &status, WNOHANG) > 0);
}


int main() {
    signal(SIGCHLD, sigchld_handler); // Obsługa SIGCHLD
    struct Karnet karnet;
    key_t key_pd_kajser = 111;
    key_t kol_kasjer_narciarz = 9000;
    key_t key_sem_kajser_kajser = 123;

    key_t s_time_man = 201;
    key_t pd_time_man = 202;



    upd(key_pd_kajser, 12);
    upa();

    upd2(pd_time_man);
    upa2();


    int sem1 = dodaj_nowy_semafor(key_sem_kajser_kajser, 1);
    int sem_t = dodaj_nowy_semafor(s_time_man, 1);

    int msgid = msgget(kol_kasjer_narciarz, 0600);

    if (msgid == -1) {
        perror("[Kasjer] Blad msgget (otwieranie kolejki)");
        exit(EXIT_FAILURE);
    }

    int with_child[10] = {1, 1, 1, 1, 2, 1, 0, 0, 0, 0}; // ilość dzieci
    int ticket_type[5] = {4, 4, 6, 8, 24};               // czas trwania biletu
    int vip_chance[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}; // szanse na vipa

    int child, saved, id = 0;
    double sum_price = 0;

    int count=0;
    int hehe = 0;

    srand(time(NULL)^getpid());


    while (1) {
        //losowy czas przyjscia klienta
        sleep(generateRandomNumber(1,2));
        // Generowanie przykładowych danych
        int age = generateRandomNumber(9, 90);
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
        semafor_v(sem_t, 0, 1);

        sum_price = ticket_price(sum_price, type, age, vip, child);

        semafor_p(sem_t,0);
        int hours = adres2[0];
        int min = adres2[1];
        int sec = adres2[2];
        semafor_v(sem_t,0,1);
        if (hours == -1){
            break;
        }
        if (hours+type > Tk) {
            saved = 999;
        }
        else {
            saved = hours + type;
        }

        karnet.id = id;
        karnet.vip_status = vip;
        karnet.childs = child;
        karnet.hours = saved;
        karnet.min = min;
        karnet.sec = sec;

        wyslij_karnet_do_kolejki(msgid, &karnet);

        sum_price = 0;
        count++;


        exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Narciarz", "Narciarz");
        hehe++;
        // dodac plik tekstowy z danymi karnetu i cena do raportu tak to juz raczej gotowy
    }
    int status;
    for (int i=0; i<hehe; i++) {
        wait(&status);
    }
    odlacz_pamiec();
    return 0;
}


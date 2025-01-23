#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>

#include "queue.h"
#include "pamiec_dzielona.h"
#include "semafory.h"
#include "structs.h"


// Struktura do wysłania karnetu do kolejki VIP/normal
struct MsgNarciarz {
    long mtype;
    struct Karnet kar;
};

int TK = 15;
int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;
int msgid_normal, msgid_vip;


int main(){
    key_t kol_kasjer_narciarz = 9000;
    key_t kol_nrm_narciarz_prac = 9001;
    key_t kol_vip_narciarz_prac = 9002;
    key_t key_pd_narciarz_pracownik = 125;
    key_t s_narciarz_narciarz = 356;


    struct msgBuf buf;

    int msgid = odbierz_kolejke(kol_kasjer_narciarz);

    int ret = msgrcv(msgid, &buf, sizeof(buf.kar), 0, 0);
    if (ret == -1) {
        perror("[Narciarz] Blad msgrcv");
        exit(EXIT_FAILURE);
    }

    struct Karnet k = buf.kar;

    upd(key_pd_narciarz_pracownik);
    upa();



    int msgid_vip = msgget(kol_vip_narciarz_prac, 0666);
    if (msgid_vip == -1) {
        perror("[Pracownik] Blad msgget (otwieranie kolejki)");
        exit(EXIT_FAILURE);
    }

    int msgid_normal = msgget(kol_nrm_narciarz_prac, 0666);
    if (msgid_normal == -1) {
        perror("[Pracownik] Blad msgget (otwieranie kolejki)");
        exit(EXIT_FAILURE);
    }

    int s_crit_zone = dodaj_nowy_semafor(s_narciarz_narciarz, 1);
    semafor_v(s_crit_zone,0,1);
    struct MsgNarciarz msg;
    memcpy(&msg.kar, &k, sizeof(struct Karnet));

    // Ustalamy typ komunikatu (np. 1,2,3) zależnie od liczby dzieci
    if (k.childs == 2) {
        msg.mtype = 3;
    } else if (k.childs == 1) {
        msg.mtype = 2;
    } else {
        msg.mtype = 1;
    }


    // Wysyłamy do kolejki normalnej albo VIP
    semafor_p(s_crit_zone,0);
    if (k.vip_status){
        if (msgsnd(msgid_vip, &msg, sizeof(msg.kar), 0) == -1) {
            perror("[Narciarz] msgsnd vip");
        }
    } else {
        if (msgsnd(msgid_normal, &msg, sizeof(msg.kar), 0) == -1) {
            perror("[Narciarz] msgsnd normal");
        }
    }
    semafor_v(s_crit_zone, 0, 1);

    // printf("Narciarz id=%d vip=%d dziala!\n", k.id, k.vip_status);

    //Czekamy, aż ID narciarza pojawi się w tablicy adres[]
    while (k.id != adres[0] && k.id != adres[1] && k.id != adres[2]){
        usleep(10000);
    }
    printf("Narciarz id=%d vip=%d dziala!\n", k.id, k.vip_status);
    odlaczenie1 = shmdt(adres);
    if (odlaczenie1 == -1) {
        printf("Problemy z odlaczeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

//tutaj dodac semafor ze max 4 na raz wchodzi i dodac zjazdy
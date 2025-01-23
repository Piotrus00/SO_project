#include <stdio.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#include "pamiec_dzielona.h"
#include "structs.h"

int TK = 20;

struct MsgBuf {
    long mtype;
    struct Karnet k;
};

int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;

int main() {
    key_t kol_nrm_narciarz_prac = 9001;
    key_t kol_vip_narciarz_prac = 9002;
    key_t key_pd_narciarz_pracownik = 125;

    int tab[3] = {0,0,0};
    int i = 0;
    int remain = 3;

    time_t start_time, current_time;
    time(&start_time);
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

    while (1) {
        remain = 3;
        i = 0;
        // printf("remain:%d, i:%d\n", remain, i);
        struct MsgBuf buf;
        while (remain >= 3 && msgrcv(msgid_vip, &buf, sizeof(buf.k), 3, IPC_NOWAIT) > 0) {
            tab[i++] = buf.k.id;
            tab[i++] = buf.k.id;
            tab[i++] = buf.k.id;
            remain -= 3; // 1 dorosły + 2 dzieci
            // printf("[1]\n");
        }
        //    b) 1-child
        while (remain >= 2 && msgrcv(msgid_vip, &buf, sizeof(buf.k), 2, IPC_NOWAIT) > 0) {
            tab[i++] = buf.k.id;
            tab[i++] = buf.k.id;
            remain -= 2; // 1 dorosły + 1 dziecko
            // printf("[2]\n");

        }
        //    c) 0-child
        while (remain >= 1 && msgrcv(msgid_vip, &buf, sizeof(buf.k), 1, IPC_NOWAIT) > 0) {
            tab[i++] = buf.k.id;
            remain -= 1; // 1 dorosły, brak dzieci
            // printf("[3]\n");
        }

        // 2) Teraz normal, też od największych do najmniejszych
        while (remain >= 3 && msgrcv(msgid_normal, &buf, sizeof(buf.k), 3, IPC_NOWAIT) > 0) {
            tab[i] = buf.k.id;
            tab[i++] = buf.k.id;
            tab[i++] = buf.k.id;
            remain -= 3;
            // printf("[4]\n");

        }
        while (remain >= 2 && msgrcv(msgid_normal, &buf, sizeof(buf.k), 2, IPC_NOWAIT) > 0) {
            tab[i++] = buf.k.id;
            tab[i++] = buf.k.id;
            remain -= 2;
            // printf("[5]\n");

        }
        while (remain >= 1 && msgrcv(msgid_normal, &buf, sizeof(buf.k), 1, IPC_NOWAIT) > 0) {
            tab[i++] = buf.k.id;
            remain -= 1;
            // printf("[6]\n");
        }

        for (int j = 0; j < 3; j++) {
                adres[j] = tab[j];
        }
        time(&current_time);
        if (difftime(current_time, start_time) >= TK) {
            break;
        }

        usleep(50000);
    }
    odlacz_pamiec();
    return 0;
}
// tutaj walnac watek z tablica tab jakis sleep i czekamy az wyjedzie jak wyjedzie to do adresu tablice przypoisujemy +
// chyba pamiecia dzielona najlepiej odjac krzeselko z limitu a w funkcji glownej bedziemy dodawac krzeselka przed opdalenie watku i potem petla sprawdzamy czy sie limit nie dobil
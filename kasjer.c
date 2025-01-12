#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>

#define MAX_KARNETY 100
#define DZIECKO_ZNIZKA 0.75
#define SENIOR_ZNIZKA 0.75

// Typy karnetów
typedef enum {
    KARNET_CZASOWY1,
    KARNET_CZASOWY2,
    KARNET_CZASOWY3,
    KARNET_DZIENNY
} KarnetTyp;

// Struktura karnetu
typedef struct {
    int id;
    KarnetTyp typ;
    int czas_waznosci; // W minutach lub dniu
    int ilosc_uzyc;    // Liczba przejść przez bramkę
    int vip;           // 1, jeśli VIP; 0, jeśli nie
    int znizka;        // 1, jeśli zniżka; 0, jeśli brak
} Karnet;

// Struktura pamięci współdzielonej
typedef struct {
    Karnet karnety[MAX_KARNETY];
    int liczba_karnetow;
} PamiecWspoldzielona;

// Operacje na semaforach
void semafor_op(int sem_id, int sem_num, int op) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = op;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}

void Kasjer(PamiecWspoldzielona *pamiec, int sem_id) {
    while (1) {
        printf("[Kasjer] Czekam na klienta...\n");
        sleep(rand() % 5 + 1); // Symulacja czasu obsługi klienta

        // Tworzenie nowego karnetu
        semafor_op(sem_id, 0, -1); // Zablokowanie dostępu do pamięci współdzielonej

        Karnet nowy_karnet;
        nowy_karnet.id = pamiec->liczba_karnetow + 1;
        nowy_karnet.typ = rand() % 4; // Losowy typ karnetu
        nowy_karnet.czas_waznosci = (nowy_karnet.typ == KARNET_DZIENNY) ? 1440 : (nowy_karnet.typ + 1) * 60;
        nowy_karnet.ilosc_uzyc = 0;
        nowy_karnet.vip = rand() % 2;
        nowy_karnet.znizka = rand() % 2;

        pamiec->karnety[pamiec->liczba_karnetow] = nowy_karnet;
        pamiec->liczba_karnetow++;

        printf("[Kasjer] Sprzedano karnet ID: %d, Typ: %d, VIP: %d, Znizka: %d\n",
               nowy_karnet.id, nowy_karnet.typ, nowy_karnet.vip, nowy_karnet.znizka);

        semafor_op(sem_id, 0, 1); // Odblokowanie dostępu do pamięci współdzielonej
    }
}

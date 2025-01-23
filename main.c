#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "semafory.h"
#include "queue.h"

void exe_a_proces(const char *path, const char *name);


int TK=15;
int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;

int main() {

    key_t kol_kasjer_narciarz = 9000;
    key_t kol_nrm_narciarz_prac = 9001;
    key_t kol_vip_narciarz_prac = 9002;

    key_t s_narciarz_narciarz = 356;

    int msgid = utworz_kolejke(kol_kasjer_narciarz);

    int id = msgget(kol_vip_narciarz_prac, 0666 | IPC_CREAT);
    if (id == -1) {
        perror("Nie moglem utworzyc kolejki komunikatow");
        exit(EXIT_FAILURE);
    }
    int id2 = msgget(kol_nrm_narciarz_prac, 0666 | IPC_CREAT);
    if (id2 == -1) {
        perror("Nie moglem utworzyc kolejki komunikatow");
        exit(EXIT_FAILURE);
    }
    int sem1 = utworz_nowy_semafor(123);
    int sem3 = utworz_nowy_semafor(s_narciarz_narciarz);
    int sem_krzeselka = utworz_nowy_semafor(128);


    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");

    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Peron", "Peron");

    //raczej gotowy dodac tylko kolejke i ewentualnie peron dolny zalezy co wymysle

    int status;
    wait(&status);
    wait(&status);
    wait(&status);
    wait(&status);
    wait(&status);
    // wait(&status);
    // wait(&status);


    usun_semafor(sem1, 0);
    usun_semafor(sem_krzeselka,0);
    usun_semafor(sem3, 0);
    usun_kolejke(msgid);
    usun_kolejke(id);
    usun_kolejke(id2);


    return 0;
}


void exe_a_proces(const char *path, const char *name) {
    if (fork() == 0) {
        execl(path, name, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
}


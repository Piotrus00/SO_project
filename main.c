#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "semafory.h"
#include "queue.h"

void exe_a_proces(const char *path, const char *name);


int TK=10;
int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;

int main() {
    int sem1 = utworz_nowy_semafor(123);
    int msgid = utworz_kolejke(555);


    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");

    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Narciarz", "Narciarz");

    //raczej gotowy dodac tylko kolejke i ewentualnie peron dolny zalezy co wymysle

    int status;
    wait(&status);
    wait(&status);
    wait(&status);
    wait(&status);
    wait(&status);

    usun_semafor(sem1, 0);
    usun_kolejke(msgid);

    return 0;
}


void exe_a_proces(const char *path, const char *name) {
    if (fork() == 0) {
        execl(path, name, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
}
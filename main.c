#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/wait.h>

int sem_karnet_gotowy;

void exe_a_proces(const char *path, const char *name);
int utworz_nowy_semafor(int key);
static void usun_semafor(int semafor, int sem_num);
int utworz_kolejke(int key);
void usun_kolejke(int msgid);

int main() {
    int sem1 = utworz_nowy_semafor(123);
    int msgid = utworz_kolejke(555);

    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");

    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Narciarz", "Narciarz");


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


int utworz_nowy_semafor(int key)
{
    int semafor;
    semafor=semget(key,2,0777|IPC_CREAT);
    if (semafor==-1)
    {
        printf("Nie moglem utworzyc nowego semafora.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Semafor zostal utworzony : %d\n",semafor);
    }
    return semafor;
}

static void usun_semafor(int semafor, int sem_num)
{
    int sem;
    sem=semctl(semafor,sem_num,IPC_RMID);
    if (sem==-1)
    {
        printf("Nie mozna usunac semafora.\n");
        exit(EXIT_FAILURE);
    }
    printf("Semafor zostal usuniety : %d\n",sem);
}

void usun_kolejke(int msgid) {
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Nie moglem usunac kolejki komunikatow");
        exit(EXIT_FAILURE);
    }
    printf("Kolejka komunikatow (ID: %d) zostala usunieta.\n", msgid);
}

int utworz_kolejke(int key) {
    int id = msgget(key, 0666 | IPC_CREAT);
    if (id == -1) {
        perror("Nie moglem utworzyc kolejki komunikatow");
        exit(EXIT_FAILURE);
    }
    // printf("Utworzono kolejke komunikatow o ID: %d\n", id);
    return id;
}
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>

#include "pamiec_dzielona_2.h"
#include "semafory.h"
#include "queue.h"



const int Tp = 8;  // Czas otwarcia (godzina)
const int Tk = 21; // Czas zamknięcia (godzina)
const int scale_duration = 30; // Czas trwania dnia w sekundach (30 sekund)
const int update_interval = 10; // Interwał aktualizacji w ms
int current_time;

int pamiec2;
int odlaczenie3;
int odlaczenie4;
int *adres2;


void exe_a_proces(const char *path, const char *name);

void *time_simulation_thread(void *arg);


int main() {

    key_t kol_kasjer_narciarz = 9000;
    key_t kol_nrm_narciarz_prac = 9001;
    key_t kol_vip_narciarz_prac = 9002;
    key_t kol_krzeselka_narciarz = 9003;
    key_t kol_narciarz_krzeselka = 9004;



    key_t s_narciarz_narciarz = 356;
    key_t s_narciarz_pracownik = 357;
    key_t s_narciarz_krzeselka = 358;
    key_t s_krzeselka_count_key = 359;

    key_t s_time_man = 201;


    int msgid = utworz_kolejke(kol_kasjer_narciarz);

    int id = msgget(kol_vip_narciarz_prac, 0600 | IPC_CREAT);
    if (id == -1) {
        perror("Nie moglem utworzyc kolejki komunikatow");
        exit(EXIT_FAILURE);
    }
    int id2 = msgget(kol_nrm_narciarz_prac, 0600 | IPC_CREAT);
    if (id2 == -1) {
        perror("Nie moglem utworzyc kolejki komunikatow");
        exit(EXIT_FAILURE);
    }

    int id3 = msgget(kol_krzeselka_narciarz, 0600 | IPC_CREAT);
    if (id3 == -1) {
        perror("Nie moglem utworzyc kolejki komunikatow");
        exit(EXIT_FAILURE);
    }

    int id4 = msgget(kol_narciarz_krzeselka, 0600 | IPC_CREAT);
    if (id4 == -1) {
        perror("Nie moglem utworzyc kolejki komunikatow");
        exit(EXIT_FAILURE);
    }

    int sem1 = utworz_nowy_semafor(123, 1);
    int sem2 = utworz_nowy_semafor(s_narciarz_pracownik, 1);
    int sem3 = utworz_nowy_semafor(s_narciarz_narciarz, 3);
    int sem5 = utworz_nowy_semafor(s_time_man, 1);
    int sem6 = utworz_nowy_semafor(s_narciarz_krzeselka, 1);
    int sem7 = utworz_nowy_semafor(s_krzeselka_count_key, 1);

    pthread_t thread;

    if (pthread_create(&thread, NULL, time_simulation_thread, NULL) != 0) {
        perror("Nie można utworzyć wątku");
        return EXIT_FAILURE;
    }
    pthread_detach(thread);

    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");
    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Kasjer", "Kasjer");

    exe_a_proces("/home/kali/CLionProjects/untitled3/cmake-build-debug/Peron", "Peron");


    int status;

    wait(&status);
    wait(&status);
    wait(&status);
    wait(&status);
    wait(&status);

    usun_semafor(sem1, 0);
    usun_semafor(sem2, 0);
    usun_semafor(sem3, 0);
    usun_semafor(sem5, 0);
    usun_semafor(sem6, 0);
    usun_semafor(sem7, 0);

    usun_kolejke(msgid);
    usun_kolejke(id);
    usun_kolejke(id2);
    usun_kolejke(id3);
    usun_kolejke(id4);
    odlacz_pamiec2();

    return 0;
}


void exe_a_proces(const char *path, const char *name) {
    if (fork() == 0) {
        execl(path, name, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
}

void *time_simulation_thread(void *arg) {
    key_t s_time_man = 201;
    key_t pd_time_man_202 = 202;
    int sem_t = dodaj_nowy_semafor(s_time_man, 1);
    semafor_v(sem_t,0,1);
    int simulated_day_duration = (Tk - Tp) * 3600; // Czas trwania dnia w sekundach
    int scaled_second = simulated_day_duration / scale_duration; // Sekunda symulowana jako czas rzeczywisty
    upd2(pd_time_man_202);
    upa2();

    current_time = 0; // Rozpoczynamy od Tp

    while (current_time <= simulated_day_duration) {

        int hours = Tp + (current_time / 3600);
        int minutes = (current_time % 3600) / 60;
        int seconds = current_time % 60;

        semafor_p(sem_t,0);
        adres2[0] = hours;
        adres2[1] = minutes;
        adres2[2] = seconds;
        semafor_v(sem_t,0,1);

        usleep(update_interval * 1000);
        current_time += scaled_second / (1000 / update_interval);
    }

    semafor_p(sem_t,0);
    adres2[0] = -1;
    adres2[1] = -1;
    adres2[2] = -1;
    semafor_v(sem_t,0,1);

    printf("Symulacja zakończona!\n");
    sleep(30);
    printf("[LAWINA]!!!\n");
    int status;
    status = system("killall Narciarz");
    if (status == -1) {
        perror("Nie udało się zakończyć procesów Narciarz");
    }
    return NULL;
}
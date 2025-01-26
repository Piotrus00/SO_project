#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "queue.h"
#include "pamiec_dzielona.h"
#include "structs.h"



int TK = 15;
int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;
int msgid_normal, msgid_vip;


pthread_mutex_t queue_mutex;

// Tablica dynamiczna dla identyfikatorów wątków
pthread_t *threads = NULL;
size_t threads_count = 0;
size_t threads_capacity = 10;

// Struktura do wysyłania komunikatu (narciarz -> pracownik)
struct MsgNarciarz {
    long mtype;
    struct Karnet kar;
};

void* narciarz_thread(void* arg) {
    struct Karnet *k = (struct Karnet*) arg;

    key_t key_pd_narciarz_pracownik = 125;
    upd(key_pd_narciarz_pracownik);
    upa();

    struct MsgNarciarz msg;
    memcpy(&msg.kar, k, sizeof(struct Karnet));
    if (k->childs == 2)
        msg.mtype = 3;
    else if (k->childs == 1)
        msg.mtype = 2;
    else
        msg.mtype = 1;

    pthread_mutex_lock(&queue_mutex);
    if (k->vip_status) {
        if (msgsnd(msgid_vip, &msg, sizeof(msg.kar), 0) == -1) {
            perror("[Narciarz] msgsnd vip");
        }
    } else {
        if (msgsnd(msgid_normal, &msg, sizeof(msg.kar), 0) == -1) {
            perror("[Narciarz] msgsnd normal");
        }
    }
    pthread_mutex_unlock(&queue_mutex);

    while (k->id != adres[0] && k->id != adres[1] && k->id != adres[2]){
        usleep(10000);
    }

    printf("Narciarz id=%d vip=%d dziala!\n", k->id, k->vip_status);
    free(k);
    return NULL;
}

void add_thread(pthread_t tid) {
    if (threads_count == threads_capacity) {
        threads_capacity *= 2;
        threads = realloc(threads, threads_capacity * sizeof(pthread_t));
        if (!threads) {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }
    }
    threads[threads_count++] = tid;
}

int main() {

    key_t kol_nrm_narciarz_prac = 9001;
    key_t kol_vip_narciarz_prac = 9002;


    if (pthread_mutex_init(&queue_mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }

    msgid_normal = msgget(kol_nrm_narciarz_prac, IPC_CREAT | 0666);
    if (msgid_normal == -1) {
        perror("[Narciarz] msgget normal");
        exit(EXIT_FAILURE);
    }

    msgid_vip = msgget(kol_vip_narciarz_prac, IPC_CREAT | 0666);
    if (msgid_vip == -1) {
        perror("[Narciarz] msgget vip");
        exit(EXIT_FAILURE);
    }

    time_t start_time, current_time;
    time(&start_time);
    struct msgBuf buf;
    int end_of_work_kajser = 0;

    int msgid = odbierz_kolejke();

    threads = malloc(threads_capacity * sizeof(pthread_t));
    if (!threads) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        time(&current_time);
        if (difftime(current_time, start_time) >= TK) {
            break;
        }

        int ret = msgrcv(msgid, &buf, sizeof(buf.kar), 0, 0);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("[Narciarz] Blad msgrcv");
            break;
        }

        if (buf.kar.id != -1) {
            struct Karnet *nowyKarnet = malloc(sizeof(struct Karnet));
            if (!nowyKarnet) {
                perror("malloc error");
                break;
            }
            memcpy(nowyKarnet, &buf.kar, sizeof(struct Karnet));

            pthread_t tid;
            int err = pthread_create(&tid, NULL, narciarz_thread, nowyKarnet);
            if (err != 0) {
                fprintf(stderr, "Blad pthread_create: %s\n", strerror(err));
                free(nowyKarnet);
            } else {
                add_thread(tid);
            }
        } else {
            end_of_work_kajser++;
            if (end_of_work_kajser > 3) {
                break;
            }
        }
    }

    // Czekamy na zakończenie wszystkich wątków
    for (size_t i = 0; i < threads_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("[Narciarz]farjant\n");

    free(threads);

    // Kolejki na tym etapie można usunąć, jeśli wiadomo, że nikt już ich nie potrzebuje:
    // msgctl(msgid_normal, IPC_RMID, NULL);
    // msgctl(msgid_vip, IPC_RMID, NULL);

    pthread_mutex_destroy(&queue_mutex);
    odlacz_pamiec();
    return 0;
}

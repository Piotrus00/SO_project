#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>  // Dodajemy bibliotekę wątków

#include "queue.h"
#include "structs.h"

int TK = 10;

void* narciarz_thread(void* arg)
{
    // pthread_t my_tid = pthread_self();

    // Rzutujemy argument na wskaźnik do Karnet
    struct Karnet *k = (struct Karnet*) arg;

    printf("Narciarz ID=%d, VIP=%d, Childs=%d, Time=%.2f\n",
           k->id,
           k->vip_status,
           k->childs,
           k->time);

    //sprawdzic pozostaly czas karnetu odjac czas zjazdu zapisac do pliku kolejka odczytuje albo jeszcze jeden program peron dolny niapsac
    //ktory czyta dane z pliku(4 na raz) i wpuszcza na peron dolny jezeli N, po odczytaniu z pliku, thread oposcic semafor
    //tu pewnie do raportu jakies godziny wpuszczenia znowu semafor na kolejke opuszczany po wejsciu i transporcie na gore
    //dodac zjazdy trasami i to wszystko w petle

    free(k);
    return NULL;
}

int main() {
    time_t start_time, current_time;
    time(&start_time);
    struct msgBuf buf;
    int end_of_work_kajser = 0;

    // Otwieramy (lub tworzymy) kolejkę komunikatów
    int msgid=odbierz_kolejke();

    printf("[Narciarz PID=%d] Otwarta kolejka komunikatow ID=%d\n", getpid(), msgid);

    while (1) {
        // Sprawdzamy, czy minął czas (TK sekund)
        time(&current_time);
        if (difftime(current_time, start_time) >= TK) {
            // Jeśli tak, kończymy pętlę
            break;
        }

        // Odczytujemy wiadomość z kolejki
        int ret = msgrcv(msgid, &buf, sizeof(buf.kar), 0, 0);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("[Narciarz] Blad msgrcv");
            break;
        }

        // gdy kasjer sie konczy przesyla karnet id=-1 gdy program otrzyma 3 wychodzi z petli
        if (buf.kar.id != -1) {
            // Alokujemy pamięć na nowy Karnet
            struct Karnet *nowyKarnet = malloc(sizeof(struct Karnet));
            if (!nowyKarnet) {
                perror("malloc error");
                break;
            }
            // Kopiujemy dane z bufora
            memcpy(nowyKarnet, &buf.kar, sizeof(struct Karnet));

            // Tworzenie wątku
            pthread_t tid;
            int err = pthread_create(&tid, NULL, narciarz_thread, nowyKarnet);
            if (err != 0) {
                fprintf(stderr, "Blad pthread_create: %s\n", strerror(err));
                free(nowyKarnet);
            } else {
                pthread_detach(tid);
            }
        } else {
            end_of_work_kajser++;
            if (end_of_work_kajser > 3) {
                break;
            }
        }
    }

    printf("[Narciarz PID=%d] Koniec pracy.\n", getpid());
    return 0;
}
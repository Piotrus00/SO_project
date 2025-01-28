#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "structs.h"

int N = 30;
int TK = 15;


int main() {

    time_t start_time, current_time;
    time(&start_time);

    int pd_vip = open("/home/kali/CLionProjects/untitled3/cmake-build-debug/fifo_vip", O_RDONLY | O_NONBLOCK);
    if (pd_vip == -1) {
        perror("Error opening VIP FIFO");
        exit(EXIT_FAILURE);
    }

    int pd_normal = open("/home/kali/CLionProjects/untitled3/cmake-build-debug/fifo", O_RDONLY | O_NONBLOCK);
    if (pd_normal == -1) {
        perror("Error opening normal FIFO");
        close(pd_vip);
        exit(EXIT_FAILURE);
    }

    int pd_krzeselka = open("/home/kali/CLionProjects/untitled3/cmake-build-debug/fifo_krzeselka", O_WRONLY);
    if (pd_krzeselka == -1) {
        perror("[FIFO] Otwarcie zapisu");
        exit(EXIT_FAILURE);
    }


    struct Karnet k;
    int current_people = 0;

    while (1) {
        while (1) {
            if (current_people <= N - 3) {
                ssize_t bytes_read = read(pd_vip, &k, sizeof(struct Karnet));
                if (bytes_read > 0) {
                    int total_people = 1 + k.childs; // ilosc osob wchodzachych na peron
                    ssize_t bytes_written = write(pd_krzeselka, &k, sizeof(struct Karnet));
                    if (bytes_written == -1) {
                        perror("write to FIFO");
                    }
                    current_people += total_people; // obecna laczna ilosc osob na peronie
                } else break;
            }
        }
        if (current_people <= N - 3) {
            ssize_t bytes_read = read(pd_normal, &k, sizeof(struct Karnet));

            if (bytes_read > 0) {
                int total_people = 1 + k.childs;

                ssize_t bytes_written = write(pd_krzeselka, &k, sizeof(struct Karnet));

                if (bytes_written == -1) {
                    perror("write to FIFO");
                }
                current_people += total_people;
            }
        }
        current_people-=3;

        time(&current_time);
        if (difftime(current_time, start_time) >= TK) {
            break;
        }
        usleep(50000);
    }

    close(pd_vip);
    close(pd_normal);

    return 0;
}

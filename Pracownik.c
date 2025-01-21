#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "structs.h"
#include "pamiec_dzielona.h"

int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;

int main() {
    key_t key_pd_narciarz_pracownik = 125;
    upd(key_pd_narciarz_pracownik);
    upa();
    // Open the FIFO for reading
    int pd_krzeselka = open("/home/kali/CLionProjects/untitled3/cmake-build-debug/fifo_krzeselka", O_RDONLY | O_NONBLOCK);
    if (pd_krzeselka == -1) {
        perror("Error opening FIFO for reading");
        exit(EXIT_FAILURE);
    }



    struct Karnet k;
    sleep(1);
    while (1) {
        ssize_t bytes_read = read(pd_krzeselka, &k, sizeof(struct Karnet));

        if (bytes_read > 0) {
            *adres = k.id;
            usleep(20000);
        }else if (bytes_read == 0) {
            break;
        }
    }
    odlacz_pamiec();
    close(pd_krzeselka);

    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <errno.h>
#include <time.h>

// STRUKTURA KARNET
struct Karnet {
    int id;
    int vip_status; // 1 - vip, 0 - zwykly
    int childs;
    double time;
};

// STRUKTURA KOLEJKA
struct msgBuf {
    long mtype;        
    struct Karnet kar;
};

int TK = 10;
int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;


int generateRandomNumber(int min, int max);
int utworz_nowy_semafor(int key);
static void semafor_p(int semafor, int sem_num);
static void semafor_v(int semafor, int sem_num, int sem_op);
static void usun_semafor(int semafor, int sem_num);

void upd(int key);
void upa();
void odlacz_pamiec();

double ticket_price(double total_price, double time, int age, int vip, int dzieci);

int utworz_kolejke(int key);
void wyslij_karnet_do_kolejki(int msgid, struct Karnet *k);
void usun_kolejke(int msgid);

int dodaj_nowy_semafor(int key);


int main() {

    time_t start_time, current_time;
    time(&start_time);

    int sem1 = dodaj_nowy_semafor(123);
    int msgid = msgget(555, 0666);
    if (msgid == -1) {
        perror("[Kasjer] Blad msgget (otwieranie kolejki)");
        exit(EXIT_FAILURE);
    }

    int with_child[10] = {1, 1, 1, 1, 2, 1, 0, 0, 0, 0}; // ilość dzieci
    int ticket_type[5] = {2, 4, 6, 8, 24};               // czas trwania biletu
    int vip_chance[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}; // szanse na vipa

    int child, age, id = 0;
    double sum_price = 0;

    srand(time(NULL));

    upd(111);
    upa();

    while (1) {
        time(&current_time);
        if (difftime(current_time, start_time) >= TK) {
            break;
        }

        age = generateRandomNumber(9, 90);
        if (age > 18) {
            child = with_child[generateRandomNumber(0, 9)];
        } else {
            child = 0;
        }
        int type = ticket_type[generateRandomNumber(0, 4)];
        int vip  = vip_chance[generateRandomNumber(0, 9)];

        if (*adres != 0) {
            semafor_p(sem1, 0);
            int new_id = *adres + 1;
            *adres = new_id;
            id = *adres;
        } else {
            id = 1;
            *adres = id;
        }
        semafor_v(sem1, 0, 1);

        sum_price = ticket_price(sum_price, type, age, vip, child);

        struct Karnet karnet;
        karnet.id = id;
        karnet.vip_status = vip;
        karnet.childs = child;
        karnet.time = type;

        wyslij_karnet_do_kolejki(msgid, &karnet);

        sum_price = 0;

        usleep(200000); // 0.2 sekundy
    }

    odlacz_pamiec();      // to do maina trzeba przeuzcic i tworzenie ale juz nie dzis
    return 0;
}


int generateRandomNumber(int min, int max) {
    return min + rand() % (max - min + 1);
}


int utworz_nowy_semafor(int key) {
    int semafor;
    semafor = semget(key, 2, 0777 | IPC_CREAT);
    if (semafor == -1) {
        printf("Nie moglem utworzyc nowego semafora.\n");
        exit(EXIT_FAILURE);
    }
    // else {
    //     printf("Semafor zostal utworzony : %d\n", semafor);
    // }
    return semafor;
}

static void semafor_p(int semafor, int sem_num) {
    int zmien_sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num = sem_num;
    bufor_sem.sem_op  = -1;
    bufor_sem.sem_flg = 0;

    zmien_sem = semop(semafor, &bufor_sem, 1);
    if (zmien_sem == -1) {
        if (errno == EINTR) {
            semafor_p(semafor, sem_num);
        } else {
            exit(EXIT_FAILURE);
        }
    }
}


static void semafor_v(int semafor, int sem_num, int sem_op) {
    int zmien_sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num = sem_num;
    bufor_sem.sem_op  = sem_op;
    bufor_sem.sem_flg = 0;

    zmien_sem = semop(semafor, &bufor_sem, 1);
    if (zmien_sem == -1) {
        printf("Nie moglem otworzyc semafora.\n");
        exit(EXIT_FAILURE);
    }
}


static void usun_semafor(int semafor, int sem_num) {
    int sem;
    sem = semctl(semafor, sem_num, IPC_RMID);
    if (sem == -1) {
        printf("[1]Nie mozna usunac semafora.\n");
        exit(EXIT_FAILURE);
    }
    // printf("Semafor zostal usuniety : %d\n", sem);
}


void upd(int key) {
    pamiec = shmget(key, 10, 0777 | IPC_CREAT);
    if (pamiec == -1) {
        printf("Problemy z utworzeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
    // else {
        // printf("Pamiec dzielona zostala utworzona : %d\n", pamiec);
    // }
}

void upa() {
    adres = (int *)shmat(pamiec, NULL, 0);
    if (adres == (int *)(-1)) {
        printf("Problem z przydzieleniem adresu.\n");
        exit(EXIT_FAILURE);
    }
    // else {
    //     printf("Przestrzen adresowa zostala przyznana : %p\n", (void*)adres);
    // }
}

void odlacz_pamiec() {
    odlaczenie1 = shmctl(pamiec, IPC_RMID, 0);
    sleep(5);
    odlaczenie2 = shmdt(adres);
    if (odlaczenie1 == -1 || odlaczenie2 == -1) {
        printf("Problemy z odlaczeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Pamiec dzielona zostala odlaczona.\n");
    }
}


double ticket_price(double total_price, double time, int age, int vip, int dzieci) {
    int p = 50;    // stala cena za godzine = 50zl
    int vip_p = 200; // cena za godzine z vipem = 200zl
    int vip_or_not_vip = (vip) ? vip_p : p;

    if (age < 12 || age > 65) {
        total_price += vip_or_not_vip * 0.75 * time;
    } else {
        total_price += vip_or_not_vip * time;
    }
    for (int i = 1; i <= dzieci; i++) {
        total_price += vip_or_not_vip * 0.75 * time;
    }
    return total_price;
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


void wyslij_karnet_do_kolejki(int msgid, struct Karnet *k) {
    struct msgBuf msg;
    msg.mtype = 1; // dow >0
    msg.kar = *k;   // skopiuj zawartosc Karnetu

    // Wysyłamy przez msgsnd
    if (msgsnd(msgid, &msg, sizeof(msg.kar), 0) == -1) {
        perror("msgsnd blad");
        exit(EXIT_FAILURE);
    }
    // printf("Wyslano Karnet o ID: %d do kolejki.\n", k->id);
}


void usun_kolejke(int msgid) {
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Nie moglem usunac kolejki komunikatow");
        exit(EXIT_FAILURE);
    }
    printf("Kolejka komunikatow (ID: %d) zostala usunieta.\n", msgid);
}


int dodaj_nowy_semafor(int key) {
    int semafor;
    semafor = semget(key, 2, 0777);
    if (semafor == -1) {
        printf("Nie moglem dodac nowego semafora.\n");
        exit(EXIT_FAILURE);
    }
    return semafor;
}
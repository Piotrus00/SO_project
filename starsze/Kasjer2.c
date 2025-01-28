#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <time.h>

int TK=10;
int pamiec;
int odlaczenie1;
int odlaczenie2;
int *adres;  // Change to int pointer for better range handling

int generateRandomNumber(int min, int max);
int utworz_nowy_semafor(int key);
static void semafor_p(int semafor, int sem_num);
void poczatek();
void upd();
void upa();
void odlacz_pamiec();
void wstaw(char *znak);
void wez();
static void semafor_v(int semafor, int sem_num, int sem_op);
double ticket_price(double total_price, double time, int age, int vip, int dzieci);
static void usun_semafor(int semafor, int sem_num);

struct Karnet {
  int id;
  int vip_status; // 1 - vip 0 - zwykly
  int childs;
  double time;
};

int main() {
  time_t start_time, current_time;
  time(&start_time);
  int sem1=utworz_nowy_semafor(123);
  int with_child[10] = {1, 1, 1, 1, 2, 1, 0, 0, 0, 0}; // ilosc dzieci
  int ticket_type[5] = {2, 4, 6, 8, 24}; // czas trwania biletu w godzinach
  int vip_chance[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}; // czy vip
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

    age = generateRandomNumber(9, 90); // losowa liczba od 9 do 90
    if (age > 18) {
      child = with_child[generateRandomNumber(0, 9)];
    } else {
      child = 0;
    }

    int type = ticket_type[generateRandomNumber(0, 4)];
    int vip = vip_chance[generateRandomNumber(0, 9)];

    if (*adres != 0) {
      semafor_p(sem1, 0);
      int new_id = *adres + 1;
      *adres = new_id;
      id = *adres;
    } else {
      id = 1;
      *adres = id;
    }

    sum_price = ticket_price(sum_price, type, age, vip, child);
    semafor_v(sem1, 0, 1);
    printf("ID: %d, Wiek: %d, Dzieci: %d, Time: %d VIP: %d, Cena biletu: %lf\n", id, age, child, type, vip, sum_price);
    sum_price = 0;
    usleep(200000); // freeze na .2sekundy
  }

  odlacz_pamiec();
  usun_semafor(sem1, 0);
  return 0;
}


int generateRandomNumber(int min, int max) {
  return min + rand() % (max - min + 1);
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

static void semafor_p(int semafor, int sem_num)
{
  int zmien_sem;
  struct sembuf bufor_sem;
  bufor_sem.sem_num=sem_num;
  bufor_sem.sem_op=-1;
  bufor_sem.sem_flg=0;
  zmien_sem=semop(semafor,&bufor_sem,1);
  if (zmien_sem==-1)
  {
    if(errno == EINTR){
      semafor_p(semafor, sem_num);
    }
    else
    {
      exit(EXIT_FAILURE);
    }
  }
  // else
  //   {
  //     printf("Semafor zostal zamkniety.\n");
  //   }
}

void upd(int key)
{
  pamiec=shmget(key,1,0777|IPC_CREAT);
  if (pamiec==-1)
  {
    printf("Problemy z utworzeniem pamieci dzielonej.\n");
    exit(EXIT_FAILURE);
  }
  else printf("Pamiec dzielona zostala utworzona : %d\n",pamiec);
}

void upa()
{
  adres=(int *)shmat(pamiec,NULL,0);
  if (adres==(int *)(-1))
  {
    printf("Problem z przydzieleniem adresu.\n");
    exit(EXIT_FAILURE);
  }
  else printf("Przestrzen adresowa zostala przyznana : %s\n",adres);
}

void odlacz_pamiec()
{
  odlaczenie1=shmctl(pamiec,IPC_RMID,0);
  sleep(5);
  odlaczenie2=shmdt(adres);
  if (odlaczenie1==-1 || odlaczenie2==-1)
  {
    printf("Problemy z odlaczeniem pamieci dzielonej.\n");
    exit(EXIT_FAILURE);
  }
  else printf("Pamiec dzielona zostala odlaczona.\n");
}

void wez()
{
  printf("Biore z pamieci : %s\n",adres);
}

static void semafor_v(int semafor, int sem_num, int sem_op)
{
  int zmien_sem;
  struct sembuf bufor_sem;
  bufor_sem.sem_num=sem_num;
  bufor_sem.sem_op=sem_op;
  bufor_sem.sem_flg=0;
  zmien_sem=semop(semafor,&bufor_sem,1);
  if (zmien_sem==-1)
  {
    printf("P3:Nie moglem otworzyc semafora.\n");
    exit(EXIT_FAILURE);
  }
  // printf("Semafor zostal otwarty.\n");
}

double ticket_price(double total_price, double time, int age, int vip, int dzieci) {
  int p = 50; //stala cena za godzine = 50zl
  int vip_p = 200;// cena za godzine z vipem = 200zl
  int vip_or_not_vip;
  if (vip) {
    vip_or_not_vip = vip_p;
  }
  else {
    vip_or_not_vip = p;
  }
  if (age<12 || age>65) {
    total_price += vip_or_not_vip * 0.75 * time;
  }
  else {
    total_price+= vip_or_not_vip * time;
  }
  for (int i=1; i<=dzieci; i++) {
    total_price+= vip_or_not_vip * 0.75*time;
  }
  return total_price;
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

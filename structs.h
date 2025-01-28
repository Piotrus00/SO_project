

#ifndef STRUCTS_H
#define STRUCTS_H

// STRUKTURA KARNET
struct Karnet {
    int id;
    int vip_status; // 1 - vip, 0 - zwykly
    int childs;
    int hours;
    int min;
    int sec;
};

// STRUKTURA WIADOMOŚCI DLA KOLEJKI
struct msgBuf {
    long mtype;
    struct Karnet kar;
};

// Struktura wiadomości w kolejce 2 taka sama dla przejrzystosci
struct MsgBuf2 {
    long mtype;
    struct Karnet k;
};

struct MsgBufDone {
    long mtype;        // k.id
    int id;   // powtórzone k.id
};

// Struktura przekazywana do wątku
typedef struct {
    int tablica[3];
    int key;
} id_na_krzeselku;

struct MsgNarciarz {
    long mtype;
    struct Karnet k;
};


#endif //STRUCTS_H

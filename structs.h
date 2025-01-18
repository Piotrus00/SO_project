#ifndef STRUCTS_H
#define STRUCTS_H

// STRUKTURA KARNET
struct Karnet {
    int id;
    int vip_status; // 1 - vip, 0 - zwykly
    int childs;
    double time;
};

// STRUKTURA WIADOMOŚCI DLA KOLEJKI
struct msgBuf {
    long mtype;
    struct Karnet kar;
};

#endif //STRUCTS_H
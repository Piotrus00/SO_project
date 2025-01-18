#ifndef FUNCTIONS_KASJER_H
#define FUNCTIONS_KASJER_H

#include <stdlib.h>

int generateRandomNumber(int min, int max) {
    return min + rand() % (max - min + 1);
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


#endif //FUNCTIONS_KASJER_H
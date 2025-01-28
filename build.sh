#!/bin/bash


echo "Kompiluję Narciarz..."
gcc -o Narciarz Narciarz.c queue.c pamiec_dzielona.c pamiec_dzielona_2.c semafory.c -lpthread
echo "Narciarz został skompilowany."

echo "Kompiluję Main..."
gcc -o Main main.c pamiec_dzielona_2.c semafory.c queue.c -lpthread
echo "Main został skompilowany."

echo "Kompiluję Pracownik..."
gcc -o Pracownik Pracownik.c queue.c pamiec_dzielona.c pamiec_dzielona_2.c semafory.c -lpthread
echo "Pracownik został skompilowany."

echo "Kompiluję Kasjer..."
gcc -o Kasjer Kasjer.c queue.c pamiec_dzielona.c pamiec_dzielona_2.c semafory.c -lpthread
echo "Kasjer został skompilowany."


echo "Wszystkie pliki zostały pomyślnie skompilowane."

int main() {
    key_t shm_key = ftok("program", 65);
    key_t sem_key = ftok("program", 66);

    // Inicjalizacja pamięci współdzielonej
    int shm_id = shmget(shm_key, sizeof(PamiecWspoldzielona), 0666 | IPC_CREAT);
    if (shm_id == -1) {
        perror("Blad tworzenia pamieci wspoldzielonej");
        exit(1);
    }

    PamiecWspoldzielona *pamiec = (PamiecWspoldzielona *)shmat(shm_id, NULL, 0);
    if (pamiec == (void *)-1) {
        perror("Blad dolaczenia pamieci wspoldzielonej");
        exit(1);
    }

    pamiec->liczba_karnetow = 0;

    // Inicjalizacja semaforów
    int sem_id = semget(sem_key, 1, 0666 | IPC_CREAT);
    if (sem_id == -1) {
        perror("Blad tworzenia semaforow");
        exit(1);
    }

    semctl(sem_id, 0, SETVAL, 1); // Ustawienie wartości początkowej semafora na 1

    printf("[Main] Inicjalizacja zakończona. Startujemy!\n");

    // Uruchomienie kasjera
    Kasjer(pamiec, sem_id);

    // Zwalnianie zasobów przy zakończeniu
    shmdt(pamiec);
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);

    return 0;
}

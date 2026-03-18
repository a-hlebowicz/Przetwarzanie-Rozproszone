#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>


#define DLUGOSC_KODU 4
#define MAX_PROBY 12
#define LICZBA_KOLOROW 6

char* kolory[] = {"1", "2", "3", "4", "5", "6"};

// Funkcja losująca kod
void generuj_kod(char wynik[DLUGOSC_KODU][2]) {
    for (int i = 0; i < DLUGOSC_KODU; i++) {
        strcpy(wynik[i], kolory[rand() % LICZBA_KOLOROW]);
    }
}

// Funkcja sprawdzająca próbę
void sprawdz(char haslo[DLUGOSC_KODU][2], char kod[DLUGOSC_KODU][2], int* trafienia, int* prawie) {
    *trafienia = 0;
    *prawie = 0;
    int trafione[DLUGOSC_KODU] = {0};
    int sprawdzone[DLUGOSC_KODU] = {0};

    // Trafienia na miejscu
    for (int i = 0; i < DLUGOSC_KODU; i++) {
        if (strcmp(haslo[i], kod[i]) == 0) {
            (*trafienia)++;
            trafione[i] = 1;
            sprawdzone[i] = 1;
        }
    }

    // Trafienia w złym miejscu
    for (int i = 0; i < DLUGOSC_KODU; i++) {
        if (trafione[i]) continue;
        for (int j = 0; j < DLUGOSC_KODU; j++) {
            if (!sprawdzone[j] && strcmp(haslo[i], kod[j]) == 0) {
                (*prawie)++;
                sprawdzone[j] = 1;
                break;
            }
        }
    }
}

void koder(int pipe_r, int pipe_w) {
    char haslo[DLUGOSC_KODU][2];
    generuj_kod(haslo);

    printf("[KODER] Tajny kod: ");
    for (int i = 0; i < DLUGOSC_KODU; i++) printf("%s ", haslo[i]);
    printf("\n");

    for (int tura = 1; tura <= MAX_PROBY; tura++) {
        char kod[DLUGOSC_KODU][2];
        read(pipe_r, kod, sizeof(kod));

        printf("[KODER] Otrzymano próbę %d: ", tura);
        for (int i = 0; i < DLUGOSC_KODU; i++) printf("%s ", kod[i]);
        printf("\n");

        int trafienia = 0, prawie = 0;
        sprawdz(haslo, kod, &trafienia, &prawie);

        int wynik[2] = {trafienia, prawie};
        write(pipe_w, wynik, sizeof(wynik));

        if (trafienia == DLUGOSC_KODU) {
            printf("[KODER] Dekoder wygrał w %d turze!\n", tura);
            return;
        }
    }

    printf("[KODER] Koniec gry! Dekoder przegrał.\n");
}

int main() {
    srand(time(NULL));

    int d2k[2]; pipe(d2k); // dekoder → koder
    int k2d[2]; pipe(k2d); // koder → dekoder

    pid_t pid_koder = fork();
    if (pid_koder == 0) {
        close(d2k[1]);
        close(k2d[0]);
        koder(d2k[0], k2d[1]);
        exit(0);
    }

    pid_t pid_dekoder = fork();
    if (pid_dekoder == 0) {
        close(d2k[0]);
        close(k2d[1]);

        // Ładowanie biblioteki .so
        void* handle = dlopen("./libdekoder.so", RTLD_NOW);
        if (!handle) {
            fprintf(stderr, "Błąd ładowania biblioteki: %s\n", dlerror());
            exit(1);
        }

        void (*dekoder_fn)(int, int) = dlsym(handle, "dekoder");
        if (!dekoder_fn) {
            fprintf(stderr, "Nie znaleziono funkcji dekoder: %s\n", dlerror());
            exit(1);
        }

        dekoder_fn(d2k[1], k2d[0]);
        dlclose(handle);
        exit(0);
    }

    close(d2k[0]); close(d2k[1]);
    close(k2d[0]); close(k2d[1]);

    wait(NULL);
    wait(NULL);

    return 0;
}


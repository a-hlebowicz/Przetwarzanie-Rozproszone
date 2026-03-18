#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "dekoder.h"

#define DLUGOSC_KODU 4
#define MAX_PROBY 12
#define LICZBA_KOLOROW 6

char* kolory[] = {"1", "2", "3", "4", "5", "6"};

void dekoder(int pipe_w, int pipe_r) {
    char kolory_reszta[LICZBA_KOLOROW][2];
    int indeks_reszty = 0;
    srand(time(NULL) ^ getpid());

    for (int tura = 1; tura <= MAX_PROBY; tura++) {
        char kod[DLUGOSC_KODU][2];
        if (tura <= LICZBA_KOLOROW) {
            for (int i = 0; i < DLUGOSC_KODU; i++)
                strcpy(kod[i], kolory[tura - 1]);
        } else {
            for (int i = 0; i < DLUGOSC_KODU; i++)
                strcpy(kod[i], kolory_reszta[rand() % indeks_reszty]);
        }

        printf("[DEKODER] Próba %d: ", tura);
        for (int i = 0; i < DLUGOSC_KODU; i++) printf("%s ", kod[i]);
        printf("\n");

        ssize_t napisane = write(pipe_w, kod, sizeof(kod)); //write zwraza typ ssize_t, i bez tego jest warning
        if (napisane != sizeof(kod)) {
            perror("write");
            exit(1);
        }
        int wynik[2];
        ssize_t przeczytane = read(pipe_r, wynik, sizeof(wynik));
        if (przeczytane != sizeof(wynik)) {
            perror("read");
            exit(1);
        }
        if (tura <= LICZBA_KOLOROW && wynik[0] > 0) {
            strcpy(kolory_reszta[indeks_reszty++], kolory[tura - 1]);
        }
        if (tura == MAX_PROBY)return;
        printf("[DEKODER] Trafienia: %d, Nietrafione: %d\n", wynik[0], wynik[1]);

        if (wynik[0] == DLUGOSC_KODU) {
            printf("[DEKODER] Udało się w %d turze!\n", tura);
            return;
        }
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIERSZE
#define WIERSZE 22
#endif

#ifndef KOLUMNY
#define KOLUMNY 80
#endif

/* Struktura tworząca elementy listy, w ktorych zapisuje numer kolumny
 * zywej/martwej komorki, oraz ile ma sasiadow - jesli wiecej niz 10 tzn. ze w
 * aktualnej generacji jest zywa, jesli mniej niz 10 tzn. ze jest to komorka
 * ktora potencjalnie moze sie odrodzic. Ilosc sasiadiow do odpowiednio dla
 * zywych ileSasiadow - 10, a w martwych po prostu ileSasiadow */
typedef struct Kom {
  int kolumna;
  int ileSasiadow;
  struct Kom *next;
} Kom;

/* Struktura tworzaca elementy glownej listy, w ktorych jest numer wierszu,
 * informacja czy wystepuja w danym wierszu wylacznie komorki zywe, a takze
 * wskaznik to nastepnego takiego elementu i wskaznik do listy zawierajacej
 * numery kolumn komorek zywych/potencjalnie zywych */
typedef struct KomWiersz {
  int wiersz;
  int wszystkieZywe;
  struct Kom *nextKom;
  struct KomWiersz *nextKomWiersz;
} KomWiersz;

/**
 * Skanuje tablice.
 **/
void printList(Kom *head) {
  if (head == NULL)
    return;
  while (head != NULL) {
    printf("%d ", head->ileSasiadow);
    head = head->next;
  }
  printf("\n");
}

KomWiersz *skanujTablice(void) {
  KomWiersz *listaKomorek = NULL;
  KomWiersz *ostatniKomWiersz = NULL;
  Kom *ostatniKom = NULL;
  int wiersz;
  int kolumna;

  char c;
  while ((c = getchar()) != EOF) {
    if (c == '/') {
      if ((c = getchar()) == '\n') {
        // Koniec informacji o generacji wejściowej
        return listaKomorek;
      } else {
        ungetc(c, stdin);
        // Czyta numer wierszu
        scanf("%d", &wiersz);
        // Alokuje nowy element listy z wierszem
        KomWiersz *nowyKomWiersz = malloc(sizeof(KomWiersz));
        nowyKomWiersz->wiersz = wiersz;
        nowyKomWiersz->wszystkieZywe = 1;
        nowyKomWiersz->nextKom = NULL;
        nowyKomWiersz->nextKomWiersz = NULL;
        // Dodaje ten element do listy
        if (ostatniKomWiersz == NULL) {
          listaKomorek = nowyKomWiersz;
        } else {
          ostatniKomWiersz->nextKomWiersz = nowyKomWiersz;
        }
        ostatniKomWiersz = nowyKomWiersz;
        ostatniKom = NULL;
      }
    } else if (c != '\n' && c != '/' && c != '\0') {
      // Czyta numer kolumny
      ungetc(c, stdin);
      scanf("%d", &kolumna);
      // Alokuje nowy element listy z kolumna
      Kom *nowyKom = malloc(sizeof(Kom));
      nowyKom->kolumna = kolumna;
      nowyKom->ileSasiadow = 10;
      nowyKom->next = NULL;
      // Dodaje ten element do listy
      if (ostatniKom == NULL) {
        ostatniKomWiersz->nextKom = nowyKom;
      } else {
        ostatniKom->next = nowyKom;
      }
      ostatniKom = nowyKom;
    }
  }
  return listaKomorek;
}

/**
 * Drukowanie okienka
 */

void drukujOkienko(int w, int k, KomWiersz *listaKomorek) {
  int okienko[WIERSZE][KOLUMNY];
  memset(okienko, '.', sizeof(okienko));
  if (listaKomorek != NULL) {
    while (listaKomorek) {
      Kom *tempKom = listaKomorek->nextKom;
      while (tempKom != NULL) {
        if (w <= listaKomorek->wiersz && listaKomorek->wiersz < w + WIERSZE &&
            k <= tempKom->kolumna && tempKom->kolumna < k + KOLUMNY) {
          okienko[listaKomorek->wiersz - w][tempKom->kolumna - k] = '0';
        }
        tempKom = tempKom->next;
      }
      listaKomorek = listaKomorek->nextKomWiersz;
    }
  }
  for (int i = 0; i < WIERSZE; i++) {
    for (int j = 0; j < KOLUMNY; j++) {
      printf("%c", okienko[i][j]);
    }
    printf("\n");
  }
  for (int i = 0; i < KOLUMNY; i++) {
    printf("=");
  }
  printf("\n");
}

/**
 * Funkcja zwalniajaca podwojna liste,w ktorej trzymam informacje o komorkach
 */
void zwolnijPamiec(KomWiersz *listaKomorekHead) {
  if (listaKomorekHead == NULL)
    return;
  KomWiersz *tempWiersz = listaKomorekHead;
  while (tempWiersz != NULL) {
    KomWiersz *nextWiersz = tempWiersz->nextKomWiersz;

    Kom *tempKom = tempWiersz->nextKom;
    while (tempKom != NULL) {
      Kom *nextKom = tempKom->next;
      free(tempKom);
      tempKom = nextKom;
    }

    free(tempWiersz);
    tempWiersz = nextWiersz;
  }
}

/**
 * Tworzenie kolejnej generacji
 */

void stworzMartwe(KomWiersz *wierszHead, Kom *kom1, Kom *kom2) {
  if (wierszHead == NULL || kom1 == NULL || kom2 == NULL)
    return;
  Kom *przedostatni = kom2;
  Kom *ostatni = kom2->next;
  Kom *akt = kom1;
  while (akt != NULL && akt->ileSasiadow < 10) {
    akt = akt->next;
  }
  if (akt == NULL)
    return;
  // while (akt != NULL && akt->kolumna < przedostatni->kolumna + 1) {
  //   if (akt->kolumna < przedostatni->kolumna + 1) {
  //     int roznica = przedostatni->kolumna - akt->kolumna;
  //     if (roznica > 1) {
  //       Kom *nowyKom1 = malloc(sizeof(Kom));
  //       Kom *nowyKom2 = malloc(sizeof(Kom));
  //       Kom *nowyKom3 = malloc(sizeof(Kom));
  //       wierszHead->nextKom = nowyKom1;
  //       nowyKom1->next = nowyKom2;
  //       nowyKom2->next = nowyKom3;
  //       nowyKom3->next = przedostatni;
  //       nowyKom1->kolumna = akt->kolumna - 1;
  //       nowyKom2->kolumna = akt->kolumna;
  //       nowyKom3->kolumna = akt->kolumna + 1;
  //       nowyKom1->ileSasiadow = 0;
  //       nowyKom2->ileSasiadow = 0;
  //       nowyKom3->ileSasiadow = 0;
  //     } else if (roznica == 1) {
  //       Kom *nowyKom1 = malloc(sizeof(Kom));
  //       Kom *nowyKom2 = malloc(sizeof(Kom));
  //       wierszHead->nextKom = nowyKom1;
  //       nowyKom1->next = nowyKom2;
  //       nowyKom2->next = przedostatni;
  //       nowyKom1->kolumna = akt->kolumna - 1;
  //       nowyKom2->kolumna = akt->kolumna;
  //       nowyKom1->ileSasiadow = 0;
  //       nowyKom2->ileSasiadow = 0;
  //     } else {
  //       Kom *nowyKom1 = malloc(sizeof(Kom));
  //       wierszHead->nextKom = nowyKom1;
  //       nowyKom1->next = przedostatni;
  //       nowyKom1->kolumna = akt->kolumna - 1;
  //       nowyKom1->ileSasiadow = 0;
  //     }
  //   }
  //   akt = akt->next;
  // }
  // while (akt != NULL && akt->ileSasiadow < 10) {
  //   akt = akt->next;
  // }
  if (akt == NULL)
    return;
  int roznica = ostatni->kolumna - przedostatni->kolumna;
  while (akt != NULL && ostatni != NULL) {
    if (akt->ileSasiadow >= 10) {
      roznica = ostatni->kolumna - przedostatni->kolumna;
      if (roznica > 1) {
        if (akt->kolumna >= przedostatni->kolumna &&
            akt->kolumna <= ostatni->kolumna) {
          if (akt->kolumna == przedostatni->kolumna || roznica == 2) {
            Kom *nowyKom = malloc(sizeof(Kom));
            nowyKom->kolumna = akt->kolumna + 1;
            nowyKom->ileSasiadow = 0;
            nowyKom->next = ostatni;
            przedostatni->next = nowyKom;
            przedostatni = nowyKom;
          } else if (akt->kolumna == ostatni->kolumna) {
            Kom *nowyKom = malloc(sizeof(Kom));
            nowyKom->kolumna = akt->kolumna - 1;
            nowyKom->ileSasiadow = 0;
            nowyKom->next = ostatni;
            przedostatni->next = nowyKom;
            ostatni = nowyKom;
          } else if (akt->kolumna == przedostatni->kolumna + 1) {
            Kom *nowyKom1 = malloc(sizeof(Kom));
            Kom *nowyKom2 = malloc(sizeof(Kom));
            nowyKom1->kolumna = akt->kolumna;
            nowyKom2->kolumna = akt->kolumna + 1;
            nowyKom1->ileSasiadow = 0;
            nowyKom2->ileSasiadow = 0;
            nowyKom1->next = nowyKom2;
            nowyKom2->next = ostatni;
            przedostatni->next = nowyKom1;
            przedostatni = nowyKom2;
          } else if (akt->kolumna == ostatni->kolumna - 1) {
            Kom *nowyKom1 = malloc(sizeof(Kom));
            Kom *nowyKom2 = malloc(sizeof(Kom));
            nowyKom1->kolumna = akt->kolumna - 1;
            nowyKom2->kolumna = akt->kolumna;
            nowyKom1->ileSasiadow = 0;
            nowyKom2->ileSasiadow = 0;
            nowyKom1->next = nowyKom2;
            nowyKom2->next = ostatni;
            przedostatni->next = nowyKom1;
            przedostatni = nowyKom2;
          } else {
            Kom *nowyKom1 = malloc(sizeof(Kom));
            Kom *nowyKom2 = malloc(sizeof(Kom));
            Kom *nowyKom3 = malloc(sizeof(Kom));
            nowyKom1->kolumna = akt->kolumna - 1;
            nowyKom2->kolumna = akt->kolumna;
            nowyKom3->kolumna = akt->kolumna + 1;
            nowyKom1->ileSasiadow = 0;
            nowyKom2->ileSasiadow = 0;
            nowyKom3->ileSasiadow = 0;
            nowyKom1->next = nowyKom2;
            nowyKom2->next = nowyKom3;
            nowyKom3->next = ostatni;
            przedostatni->next = nowyKom1->next;
            przedostatni = nowyKom3;
          }
        } else if (akt->kolumna > ostatni->kolumna) {
          przedostatni = ostatni;
          ostatni = ostatni->next;
        } else {
          akt = akt->next;
        }
      } else {
        przedostatni = ostatni;
        ostatni = ostatni->next;
      }
    } else {
      akt = akt->next;
    }
  }
}

/* Funkcja zliczająca sąsiadow wystepujacych w tym samym
   wierszu dla wszystkich komorek w tym wierszu */
void zliczSasiadowWeWierszuAktualnym(Kom *pierwszyKom) {

  if (pierwszyKom == NULL || pierwszyKom->next == NULL)
    return;

  Kom *akt = pierwszyKom;
  Kom *next = pierwszyKom->next;

  while (next != NULL) {
    if ((akt->kolumna + 1) == next->kolumna) {
      (akt->ileSasiadow)++;
      (next->ileSasiadow)++;
    }
    if ((akt->kolumna + 2) == next->kolumna) {
      Kom *nowyKom = malloc(sizeof(Kom));
      nowyKom->ileSasiadow = 2;
      nowyKom->kolumna = (akt->kolumna) + 1;
      nowyKom->next = next;
      akt->next = nowyKom;
    }
    if (akt->kolumna + 2 < next->kolumna) {
      Kom *nowyKom1 = malloc(sizeof(Kom));
      nowyKom1->ileSasiadow = 1;
      nowyKom1->kolumna = next->kolumna - 1;
      nowyKom1->next = next;
      Kom *nowyKom2 = malloc(sizeof(Kom));
      nowyKom2->ileSasiadow = 1;
      nowyKom2->kolumna = akt->kolumna + 1;
      nowyKom2->next = nowyKom1;
      akt->next = nowyKom2;
    }
    akt = next;
    next = next->next;
  }
  Kom *nowyKom = malloc(sizeof(Kom));
  nowyKom->ileSasiadow = 1;
  nowyKom->kolumna = akt->kolumna + 1;
  nowyKom->next = NULL;
  akt->next = nowyKom;
}

/* Funkcja zliczająca sąsiadow wystepujacych w dolnym
   wierszu dla wszystkich komorek w tym wierszu */
void zliczSasiadowWeWierszuDolnym(Kom *pierwszyKom1, Kom *pierwszyKom2) {
  if (pierwszyKom1 == NULL || pierwszyKom2 == NULL)
    return;
  Kom *akt1 = pierwszyKom1;
  Kom *akt2 = pierwszyKom2;
  Kom *temp = akt2;

  while (akt1 != NULL && akt2 != NULL) {
    if (akt2->kolumna > (akt1->kolumna + 1)) {
      akt1 = akt1->next;
      akt2 = temp;
    } else if (akt2->kolumna < (akt1->kolumna - 1)) {
      akt2 = akt2->next;
      temp = akt2;
    } else if (akt1->ileSasiadow >= 10 && akt2->ileSasiadow < 10) {
      (akt2->ileSasiadow)++;
      akt2 = akt2->next;
      if (akt2 == NULL) {
        akt1 = akt1->next;
        akt2 = temp;
      }
    } else if (akt1->ileSasiadow < 10 && akt2->ileSasiadow >= 10) {
      (akt1->ileSasiadow)++;
      akt2 = akt2->next;
      if (akt2 == NULL) {
        akt1 = akt1->next;
        akt2 = temp;
      }
    } else if (akt1->ileSasiadow >= 10 && akt2->ileSasiadow >= 10) {
      (akt1->ileSasiadow)++;
      (akt2->ileSasiadow)++;
      akt2 = akt2->next;
      if (akt2 == NULL) {
        akt1 = akt1->next;
        akt2 = temp;
      }
    } else {
      akt2 = akt2->next;
      if (akt2 == NULL) {
        akt1 = akt1->next;
        akt2 = temp;
      }
    }
  }
}

/* Wstawia element listy KomWiersz, pomiedzy inne juz istniejace i
 * oddaje wskaznik do dodanego elementu */
KomWiersz *nowyKomWiersz(KomWiersz *prev, KomWiersz *next) {
  KomWiersz *nowyWiersz = malloc(sizeof(KomWiersz));

  prev->nextKomWiersz = nowyWiersz;
  nowyWiersz->nextKomWiersz = next;
  nowyWiersz->nextKom = NULL;

  return nowyWiersz;
}

/* Funkcja dodajaca martwe komorki ktore potencjalnie moga sie ozywic do
 * wierszu dolnego lub gornego od wierszu z zywymi komorkami do ktorych
 * sasiadow tworzymy. To czy tworzymy we wierszu dolnym czy gornym, zalezy od
 * wyboru przedostatniKom i ostatniKom, ktore sa odpowiednimi elementami w
 * nowym, dolnym lub gornym wierszu. aktKom jest to zywa komorka, ktorej
 * aktualnie rozpatrujemy sasiadow*/
void dodajMartweWeWierszu(Kom *aktKom, Kom *przedostatniKom, Kom *ostatniKom) {
  if (aktKom == NULL || przedostatniKom == NULL || ostatniKom == NULL)
    return;
  while (aktKom != NULL) {
    // Jesli wartosc ileSasiadow danej komorki jest wieksza, rowna niz 10, to
    // znaczy ze jest to komorka aktualnie zywa, wiec rozpatruje jej gornych
    // martywch sasiadow
    if (aktKom->ileSasiadow >= 10) {
      // Sprawdzam wartosc kolumny przedostaniej dodanej przeze mnie martwej
      // komorki, dzieki temu wiem jaki jest stosunek miedzy ta kolumna a
      // kolumna aktKom Jesli kolumna przedostatnio dodanej martwej komorki
      // jest wieksza o wiecej niz 2 od aktKom, to znaczy ze musze stworzyc
      // wszystkich mozliwych martywch sasiadow aktKom we wierszu sasiadujacym
      if (przedostatniKom->kolumna < aktKom->kolumna - 2) {
        Kom *nowyKom1 = malloc(sizeof(Kom));
        Kom *nowyKom2 = malloc(sizeof(Kom));
        Kom *nowyKom3 = malloc(sizeof(Kom));

        ostatniKom->next = nowyKom1;
        nowyKom1->next = nowyKom2;
        nowyKom2->next = nowyKom3;

        nowyKom1->kolumna = aktKom->kolumna - 1;
        nowyKom2->kolumna = aktKom->kolumna;
        nowyKom3->kolumna = aktKom->kolumna + 1;

        nowyKom1->ileSasiadow = 1;
        nowyKom2->ileSasiadow = 1;
        nowyKom3->ileSasiadow = 1;

        przedostatniKom = nowyKom2;
        ostatniKom = nowyKom3;
        przedostatniKom->next = ostatniKom;
        // Jesli kolumna przedostatnio dodanej martwej komorki jest dokladnie
        // wieksza o 2 od kolumny aktKom, to znaczy ze musze stworzyc jeszcze
        // dwoch mozliwych martywch sasiadow aktKom we wierszu sasiadujacym,
        // bo jeden juz tam wystepuje i jemu podwyzszam wartosc ileSasiadow
      } else if (przedostatniKom->kolumna == aktKom->kolumna - 2) {
        Kom *nowyKom1 = malloc(sizeof(Kom));
        Kom *nowyKom2 = malloc(sizeof(Kom));

        ostatniKom->next = nowyKom1;
        nowyKom1->next = nowyKom2;

        nowyKom1->kolumna = aktKom->kolumna;
        nowyKom2->kolumna = aktKom->kolumna + 1;

        (ostatniKom->ileSasiadow)++;
        nowyKom1->ileSasiadow = 1;
        nowyKom2->ileSasiadow = 1;

        przedostatniKom = nowyKom1;
        ostatniKom = nowyKom2;
        przedostatniKom->next = ostatniKom;
        // Jesli kolumna przedostatnio dodanej martwej komorki jest dokladnie
        // wieksza o 1 od kolumny aktKom, to znaczy ze musze stworzyc jeszcze
        // ostatniego mozliwego martwego sasiadow aktKom we wierszu
        // sasiadujacym, bo dwoch mozliwych juz tam wystepuje i im podwyzszam
        // wartosc ileSasiadow
      } else if (przedostatniKom->kolumna == aktKom->kolumna - 1) {
        Kom *nowyKom1 = malloc(sizeof(Kom));

        ostatniKom->next = nowyKom1;

        nowyKom1->kolumna = aktKom->kolumna + 1;

        nowyKom1->ileSasiadow = 1;
        (przedostatniKom->ileSasiadow)++;
        (ostatniKom->ileSasiadow)++;

        przedostatniKom = ostatniKom;
        ostatniKom = nowyKom1;
        przedostatniKom->next = ostatniKom;
      }
    }
    aktKom = aktKom->next;
  }
  ostatniKom->next = NULL;
}

/* Funkcja tworzaca nowe elementy listy dla martwych komorek, ktore
 * ewentualnie moga ozyc w przyszlej generacji w gornym wierszu od aktualnego
 */
void stworzWierszZMartwymi(KomWiersz *prevWiersz, KomWiersz *nextWiersz,
                           int i) {
  if (prevWiersz == NULL || nextWiersz == NULL || prevWiersz->nextKom == NULL ||
      nextWiersz->nextKom == NULL)
    return;
  Kom *aktKom = NULL;
  if (i < 0) {
    aktKom = nextWiersz->nextKom;
  } else {
    aktKom = prevWiersz->nextKom;
  }

  Kom *atrapaKom = malloc(sizeof(Kom));
  Kom *przedostatniKom = atrapaKom;
  Kom *ostatniKom = malloc(sizeof(Kom));

  przedostatniKom->next = ostatniKom;
  // Recznie ustawiam wartosc kolumny
  przedostatniKom->kolumna = (aktKom->kolumna) - 1;
  ostatniKom->kolumna = aktKom->kolumna;

  przedostatniKom->ileSasiadow = 0;
  ostatniKom->ileSasiadow = 0;

  KomWiersz *nowyWiersz = nowyKomWiersz(prevWiersz, nextWiersz);

  if (i < 0) {
    nowyWiersz->wiersz = (nextWiersz->wiersz) + i;
  } else {
    nowyWiersz->wiersz = (prevWiersz->wiersz) + i;
  }

  nowyWiersz->wszystkieZywe = 0;
  nowyWiersz->nextKom = przedostatniKom;

  dodajMartweWeWierszu(aktKom, przedostatniKom, ostatniKom);
}

/* Zmienia liczebnosc zywych sasiadiow dla juz */
void zmienWierszZMartwymiSasiadami(Kom *pierwszyKom1, Kom *pierwszyKom2) {
  if (pierwszyKom1 == NULL || pierwszyKom2 == NULL)
    return;
  Kom *akt1 = pierwszyKom1;
  Kom *akt2 = pierwszyKom2;
  Kom *temp = akt2;
  while (akt1 != NULL && akt2 != NULL) {
    if (akt1->ileSasiadow >= 10) {
      if (akt2->kolumna > (akt1->kolumna + 1)) {
        akt1 = akt1->next;
        akt2 = temp;
      } else if (akt2->kolumna < (akt1->kolumna - 1)) {
        akt2 = akt2->next;
        temp = akt2;
      } else {
        (akt2->ileSasiadow)++;
        akt2 = akt2->next;
        if (akt2 == NULL) {
          akt1 = akt1->next;
          akt2 = temp;
        }
      }
    } else {
      akt1 = akt1->next;
    }
  }
}

void zmienWierszZZywymiSasiadami(Kom *pierwszyKom1, Kom *pierwszyKom2) {
  if (pierwszyKom1 == NULL || pierwszyKom2 == NULL)
    return;
  Kom *akt1 = pierwszyKom1;
  Kom *akt2 = pierwszyKom2;
  Kom *temp = akt2;
  while (akt1 != NULL && akt2 != NULL) {
    if (akt1->ileSasiadow >= 10) {
      if (akt2->kolumna > (akt1->kolumna + 1)) {
        akt1 = akt1->next;
        akt2 = temp;
      } else if (akt2->kolumna < (akt1->kolumna - 1)) {
        akt2 = akt2->next;
        temp = akt2;
      } else {
        (akt2->ileSasiadow)++;
        akt2 = akt2->next;
        if (akt2 == NULL) {
          akt1 = akt1->next;
          akt2 = temp;
        }
      }
    } else {
      akt1 = akt1->next;
    }
  }
}

KomWiersz *ozywGorne(KomWiersz *listaKomorekHead) {
  if (listaKomorekHead == NULL)
    return NULL;
  KomWiersz *aktWiersz = listaKomorekHead;
  KomWiersz *prevWiersz = malloc(sizeof(KomWiersz));
  prevWiersz->nextKomWiersz = aktWiersz;
  prevWiersz->wiersz = aktWiersz->wiersz - 2;
  KomWiersz *atrapaWiersz = prevWiersz;
  while (aktWiersz != NULL) {
    if (prevWiersz->wiersz != aktWiersz->wiersz - 1) {
      stworzWierszZMartwymi(prevWiersz, aktWiersz, -1);
    }
    prevWiersz = aktWiersz;
    aktWiersz = aktWiersz->nextKomWiersz;
  }
  listaKomorekHead = atrapaWiersz->nextKomWiersz;
  free(atrapaWiersz);
  return listaKomorekHead;
}

/* Funkcja tworzaca elementy listy dla martwych komorek ponizej aktualnego
 * wierszu ktore moge ozyc w nastepnej generacji */
void ozywDolne(KomWiersz *listaKomorekHead) {
  if (listaKomorekHead == NULL)
    return;
  KomWiersz *aktWiersz = listaKomorekHead;
  KomWiersz *nextWiersz = aktWiersz->nextKomWiersz;
  while (nextWiersz != NULL) {
    if (aktWiersz->wszystkieZywe == 1) {
      if (nextWiersz->wiersz > aktWiersz->wiersz + 1) {
        stworzWierszZMartwymi(aktWiersz, nextWiersz, 1);
        aktWiersz = nextWiersz;
        nextWiersz = nextWiersz->nextKomWiersz;
      } else if (nextWiersz->wszystkieZywe == 1) {
        aktWiersz = nextWiersz;
        nextWiersz = nextWiersz->nextKomWiersz;
      } else {
        zmienWierszZMartwymiSasiadami(aktWiersz->nextKom, nextWiersz->nextKom);
        aktWiersz = nextWiersz;
        nextWiersz = nextWiersz->nextKomWiersz;
      }
    } else {
      aktWiersz = nextWiersz;
      nextWiersz = nextWiersz->nextKomWiersz;
    }
  }
  if (aktWiersz->wszystkieZywe == 1) {
    KomWiersz *atrapaWiersz = malloc(sizeof(KomWiersz));
    aktWiersz->nextKomWiersz = atrapaWiersz;
    stworzWierszZMartwymi(aktWiersz, atrapaWiersz, 1);
    (aktWiersz->nextKomWiersz)->nextKomWiersz = NULL;
    free(atrapaWiersz);
  }
}

/* Funkcja tworzaca nowe elementy listy dla martwych komorek, ktore
 * ewentualnie moga ozyc w przyszlej generacji w dolnym wierszu od aktualnego
 */

KomWiersz *stworzNastepnaGeneracja(KomWiersz *listaKomorekHead) {
  if (listaKomorekHead == NULL)
    return NULL;
  KomWiersz *listaKomorek = listaKomorekHead;
  while (listaKomorek->nextKomWiersz != NULL) {
    stworzMartwe(listaKomorek->nextKomWiersz, listaKomorek->nextKom,
                 listaKomorek->nextKomWiersz->nextKom);
    stworzMartwe(listaKomorek, listaKomorek->nextKomWiersz->nextKom,
                 listaKomorek->nextKom);
    listaKomorek = listaKomorek->nextKomWiersz;
  }

  listaKomorek = listaKomorekHead;

  // while (listaKomorek != NULL) {
  //   zliczSasiadowWeWierszuAktualnym(listaKomorek->nextKom);
  //   listaKomorek = listaKomorek->nextKomWiersz;
  // }

  listaKomorek = listaKomorekHead;
  while (listaKomorek != NULL) {
    printList(listaKomorek->nextKom);
    listaKomorek = listaKomorek->nextKomWiersz;
  }
  printf("zliczSasiadowWeWierszuAktualnym\n");
  listaKomorek = listaKomorekHead;

  // if (listaKomorek != NULL) {
  //   while (listaKomorek->nextKomWiersz != NULL) {
  //     if (listaKomorek->wiersz == (listaKomorek->nextKomWiersz)->wiersz - 1)
  //     {
  //       zliczSasiadowWeWierszuDolnym(listaKomorek->nextKom,
  //                                    (listaKomorek->nextKomWiersz)->nextKom);
  //     }
  //     listaKomorek = listaKomorek->nextKomWiersz;
  //   }
  // }

  listaKomorek = listaKomorekHead;
  while (listaKomorek != NULL) {
    printList(listaKomorek->nextKom);
    listaKomorek = listaKomorek->nextKomWiersz;
  }
  printf("zliczSasiadowWeWierszuDolnym\n");
  listaKomorek = listaKomorekHead;

  // listaKomorek = ozywGorne(listaKomorek);
  listaKomorekHead = listaKomorek;

  // while (listaKomorek != NULL) {
  //   printList(listaKomorek->nextKom);
  //   listaKomorek = listaKomorek->nextKomWiersz;
  // }
  printf("ozywGorne\n");
  listaKomorek = listaKomorekHead;

  // ozywDolne(listaKomorek);
  listaKomorek = listaKomorekHead;
  // while (listaKomorek != NULL && listaKomorek->nextKom != NULL) {
  //   printList(listaKomorek->nextKom);
  //   listaKomorek = listaKomorek->nextKomWiersz;
  // }
  printf("Dolne\n");
  listaKomorek = listaKomorekHead;

  return listaKomorek;
}

/**
 * Funkcja usuwajaca z listy komorki, ktore nie przezyja i te ktore sie nie
 * ozywią
 */
KomWiersz *usunMartweKomorki(KomWiersz *listaKomorekHead) {
  if (listaKomorekHead == NULL)
    return NULL;
  KomWiersz *aktKomWiersz = listaKomorekHead;
  KomWiersz *prevKomWiersz = NULL;
  KomWiersz *nowyHead = listaKomorekHead;
  while (aktKomWiersz != NULL) {
    // Sprawdzam czy nextKom jest NULLem
    if (aktKomWiersz->nextKom == NULL) {
      if (prevKomWiersz != NULL) {
        prevKomWiersz->nextKomWiersz = aktKomWiersz->nextKomWiersz;
      } else {
        nowyHead = aktKomWiersz->nextKomWiersz;
      }
      KomWiersz *usun_aktKomWiersz = aktKomWiersz;
      aktKomWiersz = aktKomWiersz->nextKomWiersz;
      free(usun_aktKomWiersz);
      if (aktKomWiersz == NULL)
        return nowyHead;
    } else {
      // Przechodze przez elementy listy i zwalniam odpowiednie elementy
      Kom *aktKom = aktKomWiersz->nextKom;
      Kom *prevKom = NULL;
      while (aktKom != NULL) {
        // printf("%d", aktKom->ileSasiadow);
        if (aktKom->ileSasiadow != 2 && aktKom->ileSasiadow != 3 &&
            aktKom->ileSasiadow != 13) {
          if (prevKom != NULL) {
            prevKom->next = aktKom->next;
          } else {
            aktKomWiersz->nextKom = aktKom->next;
          }
          Kom *usun = aktKom;
          aktKom = aktKom->next;
          free(usun);
        } else {
          prevKom = aktKom;
          aktKom = aktKom->next;
        }
      }
    }
    prevKomWiersz = aktKomWiersz;
    aktKomWiersz = aktKomWiersz->nextKomWiersz;
  }
  return nowyHead;
}

int main() {
  KomWiersz *listaKomorekHead = skanujTablice();
  KomWiersz *listaKomorek = listaKomorekHead;
  /* Poczatkowe wspolrzedne okienka */
  int w = 1;
  int k = 1;

  /* Wykorzystywane do odczytu i zrozumienia danych wejsciowych */
  char c1 = '0';
  char c2 = '1';
  while (1) {
    drukujOkienko(w, k, listaKomorek);
    c1 = getchar();
    if (c1 == '\n') {
      if (listaKomorek == NULL)
        continue;
      listaKomorek = stworzNastepnaGeneracja(listaKomorek);
      listaKomorek = usunMartweKomorki(listaKomorek);
    } else {
      c2 = getchar();
      if (c2 == ' ') {
        char c3 = getchar();
        w = c1;
        k = c3;
      } else if (c2 == '\n') {
        if (c1 == '.') {
          ;
          if (listaKomorek != NULL) {
            zwolnijPamiec(listaKomorek);
            return 0;
          }
        } else if (c1 == '0') {
          continue;
        } else if (listaKomorek != NULL) {
          int n = c1 - '0';
          for (int i = 0; i < n; i++) {
            listaKomorek = stworzNastepnaGeneracja(listaKomorek);
            listaKomorek = usunMartweKomorki(listaKomorek);
          }
        }
      }
    }
  }
  return 0;
}

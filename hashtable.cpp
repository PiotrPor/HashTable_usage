//Program ma korzystac z tablicy haszujacej
//Dane wejsciowe dla programu sa odczytywane z pliku, ale nie za pomoca std::ifstream. Plik *.exe programu jest uruchamiany we wierszu  poleceń i dane z pliku są tam dostarczone poprzez przekierowanie strumienia.
#include <iostream>
#include <sstream> //potrzebne do std::stringstream (przy okazji mozna korzystac z std::string)

struct komorka
{
    bool wypelniony; //czy cos jest wpisane do tej komorki
    long klucz;
    char lancuch[9];
};

class hashtable
{
 private:
     int rozmiar; //rozmiar tablicy, ile jest w niej komorek
     komorka* tablica; //to bedzie poczatek tablicy dynamicznej
     int ilosc_zapelnionych_pol; //ile jest zapelnionych komorek w tabeli
 public:
     hashtable();
     hashtable(int r); //konstruktor parametrowy (argument to rozmiar tablicy dynamicznej)
     ~hashtable();
     void wypisztablice(); //wypisze zawartosc tabeli (efekt komendy 'print')
     void dodaj_komorke(int k, std::string chain); //probuje dodac do tablicy komorke o podanym kluczu i lancuchu
     void usun_komorke(int k); //usunie z tablicy komorke o podanym kluczu
     int haszowanie(long key); //zwraca hasz dla podanego klucza
     void uporzadkuj(); //na nowo uklada zawartosc tablicy, porzadkuje
     void przesun_komorke(int z, int cel); //przesunie komorke w tablicy spod jednego indeksu do drugiego

     //parser jest funkcja spoza klasy, ale jest jako 'friend' bo wywoluje metody z klasy
     friend void parser(hashtable* HH, std::string linia);
};

int odczytaj_rozmiar(std::string LLL); //z linijki "size ... " odczytuje liczbe
komorka stworz_komorke(long kk, std::string sss); //tworzy komorke o podanym kluczu i przepisuje string do lancucha

/*
=============================
        MAIN
=============================
*/

int main()
{
    int ilosc_badanych_przypadkow = 1;
    std::cin >> ilosc_badanych_przypadkow; //na samym poczatku pliku jest liczba przypadkow testowych
    std::string linijka;
    //nastepna linijka w pliku to 'size ...' z rozmiarem tabeli.
    //pojedynczy std::getline() nie odczyta jej, dopiero drugi to zrobi
    std::getline(std::cin, linijka); //to ten pierwszy getline()
    while (ilosc_badanych_przypadkow > 0)
    {
            std::getline(std::cin,linijka); //ten getline() odczyta 'size ....'
            int rozmiar_tablicy = odczytaj_rozmiar(linijka); //nasze zabezpieczenie da tu 20 jesli nie ma rozmiaru
            
            hashtable* mam_tablice = new hashtable(rozmiar_tablicy); //obiekt dla tego przypadku testowego
            do {
                std::getline(std::cin, linijka);
                parser(mam_tablice, linijka);
                //   po zakonczeniu cyklu petli wywoluje sie destruktor hashtable::~hashtable()
                //   nie powinnien sie wywolywac
                //problem rozwiazany ale musialem uzywac 'new' i wskaznikow do obiektu
            } while (linijka != "stop");

            ilosc_badanych_przypadkow -= 1;
            delete mam_tablice; //w kolejnym przebiegu petli bedzie nowy obiekt, tego juz nie potrzebujemy
    }
    return 0;
}

/*
=============================
    FUNKCJE I METODY
=============================
*/

//konstruktor bezparametrowy
hashtable::hashtable()
{
    rozmiar = 10; //domyslnie rozmiar tablicy to 10
    tablica = new komorka[10];
    int a;
    komorka pusty = stworz_komorke(0,"");
    pusty.wypelniony = false;
    //wypelnienie tablicy "pustymi" komorkami
    for (a = 0; a < rozmiar; a++)
    {
        tablica[a] = pusty;
    }
    ilosc_zapelnionych_pol = 0;
}

//konstruktor parametrowy (jako argument przyjmuje rozmiar tablicy)
hashtable::hashtable(int r)
{
    rozmiar = r;
    tablica = new komorka[r];
    int a;
    komorka pusty = stworz_komorke(0,"");
    pusty.wypelniony = false;
    //wypelnienie tablicy "pustymi" komorkami
    for (a = 0; a < rozmiar; a++)
    {
        tablica[a] = pusty;
    }
    ilosc_zapelnionych_pol = 0;
}

hashtable::~hashtable()
{
    delete [] tablica; //trzeba zwolnic pamiec, bo tablice stworzono poprzez 'new'
    tablica = NULL;
}

//efekt komendy "print"
void hashtable::wypisztablice()
{
    int a;
    for (a = 0; a < rozmiar; a++)
    {
        if (tablica[a].wypelniony)
        {
            std::cout<< a << " " << tablica[a].klucz << " " << tablica[a].lancuch << std::endl;
        }
    }
    std::cout << std::endl;
    return;
}

//sprobuje dodac do tablicy komorke o podanym kluczu i stringu (przerobi go na char[])
//efekt dzialania komendy "add"
void hashtable::dodaj_komorke(int k, std::string chain)
{
    if (ilosc_zapelnionych_pol < rozmiar) //w tabeli jest miejsce na dodanie
    {
        int indeks, pierwotny_indeks;
        int numer_okrazenia = 1; //do zabezpieczenia przed nieskonczona petla
        indeks = haszowanie(k);
        pierwotny_indeks = indeks; //do zabezpieczenia przed nieskonczona petla
        komorka nowy = stworz_komorke(k,chain);
        //szukanie wolnego miejsca i tak dalej
        while (tablica[indeks].wypelniony && numer_okrazenia<=2)
        {
            indeks += 1;
            //jesli ostatnia komorka tablicy jest zapelniona, to (indeks + 1 = rozmiar)
            if (indeks == rozmiar)
            {
                indeks = 0;
            }
            if (indeks == pierwotny_indeks) //wrocilismy do indeks spod ktorego wyruszylismy
            {
                numer_okrazenia += 1;
            }
        }
        //jesli bylo wolne miejsce, to wlozy tam komorke
        if (!(numer_okrazenia > 2 || tablica[indeks].wypelniony))
        {
            tablica[indeks] = nowy;
        }
        ilosc_zapelnionych_pol += 1;
    }
    return;
}

//usunie komorke o podanym kluczu
void hashtable::usun_komorke(int k)
{
    komorka pusty = stworz_komorke(0,"");
    pusty.wypelniony = false;
    int a;
    bool przerwanie = false; //posluzy do przerwania petli for()
    for (a = 0; a < rozmiar && !przerwanie; a++)
    {
        //musi byc warunek o wypelnieniu bo "pusta" ma klucz 0, a wypelniona komorka tez moglaby miec 0
        if (tablica[a].klucz == k && tablica[a].wypelniony)
        {
            tablica[a] = pusty; //usuwana komorka jest zastapiona "pusta" komorka
            ilosc_zapelnionych_pol -= 1;
            przerwanie = true; //przerwanie petli
        }
    }
    return;
}

int hashtable::haszowanie(long key)
{
    //najprostsza metoda wyliczania haszu na podstawie klucza
    return key % rozmiar;
}

//porzadkuje, sprzata w tabeli
void hashtable::uporzadkuj()
{
    int a;
    bool wrzucony_do_poprawnego = false;
    for (a = 1; a < rozmiar; a++) //na razie nie robimy nic z komorka pod indeksem 0
    {
        if (tablica[a].wypelniony)
        {
            int poprawny = haszowanie(tablica[a].klucz);
            if (poprawny != a) //komorka nie jest tam gdzie wskazywalby hasz
            {
                wrzucony_do_poprawnego = false;
                //sprawdza czy jest wolne miejsce przy jej wlasciwym indeksie
                if (!(tablica[poprawny].wypelniony))
                {
                    przesun_komorke(a, poprawny);
                    wrzucony_do_poprawnego = true;
                }
                //sprawdza czy przed ta komorka jest dziura
                if (!(tablica[a - 1].wypelniony) && !wrzucony_do_poprawnego) //przy ta komorka jest dziura
                {
                    przesun_komorke(a, a - 1);
                }
            } //koniec 'if' dla komorki pod niewlasciwym indeksem
        } //koniec 'if' dla niepustej komorki
    }
    //czas zrobic cos z elementem w tablica[0]
    wrzucony_do_poprawnego = false;
    if (tablica[0].wypelniony && haszowanie(tablica[0].klucz) != 0)
    {
        bool przerwana_petla = false;
        int popr = haszowanie(tablica[0].klucz);
        for (a = 0; a < rozmiar && !przerwana_petla; a++)
        {
            //jest pusto tam gdzie powinna byc ta komorka
            if (popr == a && !tablica[a].wypelniony)
            {
                przesun_komorke(0,a);
                przerwana_petla = true;
                wrzucony_do_poprawnego = true;
            }
        }
    }
    return;
}

//przesuwanie komorki z jednego indeksu do drugiego
void hashtable::przesun_komorke(int z, int cel)
{
    komorka pusty = stworz_komorke(0,"");
    pusty.wypelniony = false;
    //na zwolniona przestrzen wlozy sie "pusta" komorke
    if (!tablica[cel].wypelniony)
    {
        tablica[cel] = tablica[z];
        tablica[z] = pusty;
    }
    return;
}

//jako argument przyjmuje wskaznik do obiektu aby wywolywac jego metody
//drugi argument to linijka z pliku
void parser(hashtable* HH, std::string linia)
{
    std::stringstream strumien(linia);
    //czesci skladowe komendy z pliku
    std::string pierwsza, trzecia;
    int druga;
    //wlasciwy parser
    if (strumien >> pierwsza)
    {
        if (strumien >> druga)
        {
            if (strumien >> trzecia)
            {
                //trzywyrazowe
                if (pierwsza == "add") 
                {
                    HH->dodaj_komorke(druga, trzecia); //klucz i lancuch to argumenty
                }
            }
            else
            {
                //dwuwyrazowe
                if (pierwsza == "delete")
                {
                    //usuwanie komorki (klucz jest argumentem funkcji do usuwania)
                    HH->usun_komorke(druga);
                    HH->uporzadkuj();
                }
            }
        }
        else
        {
            //jednowyrazowe
            if (pierwsza == "print")
            {
                HH->wypisztablice();
            }
            else if (pierwsza == "stop")
            {
                //nic nie robimy, parser zaraz zakonczy prace
                //linijka "stop" zostanie pozniej sprawdzona przez do{}while() wewnatrz main() 
            }
        }
    }
    return;
}

//z linijki "size ... " odczyta liczbe
int odczytaj_rozmiar(std::string LLL)
{
    int oczekiwany_rozmiar=20;
    std::stringstream strumien(LLL);
    std::string pierwsze_slowo;
    if (strumien >> pierwsze_slowo)
    {
        if (pierwsze_slowo == "size")
        {
            if (strumien >> oczekiwany_rozmiar)
            {
                //nic nie robi, bo odczytal
            }
            else
            {
                oczekiwany_rozmiar = 20;
            }
        } //koniec if
    } //koniec if
    return oczekiwany_rozmiar;
}

//tworzy komorke o podanym kluczu, zmieni string na char[]
komorka stworz_komorke(long kk, std::string sss)
{
    komorka nowa;
    nowa.wypelniony = true;
    nowa.klucz = kk;
    unsigned int a;
    if (sss.length() <= 9)
    {
        for (a = 0; a < sss.length(); a++)
        {
            nowa.lancuch[a] = sss[a];
        }
        //uzupelnienie reszty spacjami, bo inaczej bylyby losowe dziwne znaczki
        while (a<9)
        {
            nowa.lancuch[a] = ' ';
            a++;
        }
    }
    else
    {
        for (a = 0; a < 9; a++)
        {
            nowa.lancuch[a] = sss[a];
        }
    }
    return nowa;
}

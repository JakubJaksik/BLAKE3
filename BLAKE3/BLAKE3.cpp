/////////////////////////////////////////
// Implementacja funkcji skrótu BLAKE3 //
// Jakub Jaksik                        //
// WCY19KS1S1                          //
// jakub.jaksik@student.wat.edu.pl     //
// //////////////////////////////////////
// 
// Projekt zawiera implementację funkcji skrótu BLAKE3
// zgodną ze specyfikacją dostępną na stronie:
// https://github.com/BLAKE3-team/BLAKE3-specs/blob/master/blake3.pdf
// 
// BLAKE3.cpp : Ten plik zawiera test użycia zaimplementowanej funkcji skrótu

#include "Hasher.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>

#define OUT_LEN_BYTES 32
#define IN_LEN_BYTES 8192

using namespace std;

int main()
{
    // Przygotowuje plik testowy zgodny z wektorem tesotwym w dokumentacji
    ofstream test("C:/wszystko/Studia/jfs/BLAKE3/test.txt", ios_base::binary | ios_base::out);
    for (int j = 0; j < 10; j++) {
        for (unsigned __int8 i = 0; i < 251; i++) {
            test << i;
        }
    }
    test.close();

    // Otwarcie pliku do odczytu
    ifstream input("C:/wszystko/Studia/jfs/BLAKE3/test.mkv", ios_base::binary | ios_base::in );
    // Przygotowanie potrzebnych zmiennych
    Hasher* hasher = new Hasher();
    __int8 data[IN_LEN_BYTES];
    const void* buf = data;
    // Hashowanie
    auto start = chrono::high_resolution_clock::now();
    while (input) {
        input.read(data, sizeof(data));
        hasher->update(buf, sizeof(data));
    }
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    input.close();
    // Zakończenie Hashowania
    unsigned __int8 hash[OUT_LEN_BYTES];
    void* out = hash;
    hasher->finalize(out, sizeof(hash));

    // Zapis uzyskanej wartości do pliku
    ofstream output("C:/wszystko/Studia/jfs/BLAKE3/hash.txt", ios_base::binary | ios_base::out);
    for (int i = 0; i < sizeof(hash); i++) {
        output << hash[i];
    }
    output.close();
    
    // Zapis skrótu w czytelnej formie do pliku
    unsigned int *hash32 = (unsigned int*)hash;
    ofstream outputReadable("C:/wszystko/Studia/jfs/BLAKE3/hash_readable.txt", ios_base::binary | ios_base::out);
    stringstream ss;
    
    for (int i(0); i < sizeof(hash)/4; ++i) {
        ss << hex << setw(8) << setfill('0') << hash32[i] <<' ';
        if (i % 4 == 3)
            ss << endl;
    }

    cout << ss.str() << endl;
    cout << "czas obliczania skrotu: " << elapsed.count() << "s" << endl;
    outputReadable << ss.str();
    outputReadable.close();

    return 0;
}
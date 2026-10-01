// =====================================================================
// Tema 6.9  |  Taquilla de cine con candado: comprobar y actuar dentro de la misma sección crítica
// En este programa he metido la comprobación y la resta de disponibles bajo el mismo candado mtx, soltándolo en los dos caminos del if.
// Requisitos previos: 06/08, 06/05
// Qué observar al ejecutar: se venden exactamente 100 entradas y quedan 0.
// Salida determinista: SI (el total; el reparto entre taquillas cambia)
// Compilar con: -O0 (igual que 06/08, para comparar)
// =====================================================================
// Mismo programa que 06/08. Cambios: el candado se toma ANTES de comprobar
// y se suelta DESPUÉS de restar, en los dos caminos del if.
// Error típico: proteger solo "disponibles--". Sigue vendiendo de más,
// porque la comprobación quedó fuera.
#include <iostream>     // cout
#include <thread>       // thread y this_thread::sleep_for
#include <mutex>        // mutex
#include <vector>       // vector
#include <functional>   // ref
#include <chrono>       // microseconds

using namespace std;

const int NUM_TAQUILLAS = 8;
const int QUIERE_VENDER = 30;
int disponibles = 100;   // global: todas las taquillas venden de aquí
mutex mtx;               // candado global que protege disponibles

// int& vendidas_aqui: es la casilla del vector de main que le toca a esta taquilla.
void vender(int& vendidas_aqui) {
    bool agotado = false;   // bandera para salir del while cuando no quedan entradas
    // Sigo vendiendo mientras no haya llegado a mis 30 Y no se hayan agotado.
    while (vendidas_aqui < QUIERE_VENDER && !agotado) {
        mtx.lock();                                                      // ---- entra
        if (disponibles > 0) {                                           // comprobar
            this_thread::sleep_for(chrono::microseconds(50));  // imprimir entrada (los demás esperan)
            disponibles--;                                               // actuar
            vendidas_aqui++;   // no es compartida: la casilla es solo de esta taquilla
            mtx.unlock();                                                // ---- sale
        } else {
            agotado = true;
            mtx.unlock();                                                // ---- sale
        }
    }
}

int main() {
    vector<int> vendidas(NUM_TAQUILLAS, 0);   // 8 casillas, todas empiezan a 0
    vector<thread> taquillas;                 // vector de hilos, empieza vacío
    for (int i = 0; i < NUM_TAQUILLAS; i++) {
        // ref(vendidas[i]): el hilo recibe la casilla i de verdad, no una copia.
        // push_back añade el hilo, ya en marcha, al final del vector.
        taquillas.push_back(thread(vender, ref(vendidas[i])));
    }
    int total = 0;
    for (int i = 0; i < NUM_TAQUILLAS; i++) {
        taquillas[i].join();   // join: espero a que la taquilla i termine...
        total = total + vendidas[i];   // ...y solo entonces leo lo que ha vendido
    }
    cout << "Entradas vendidas: " << total << " (la sala tiene 100)\n";
    cout << "Disponibles al final: " << disponibles << "\n";
    return 0;   // 0 = el programa ha terminado bien
}

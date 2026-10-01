// =====================================================================
// Tema 6.8  |  Taquilla de cine sin candado: se venden más entradas de las que hay
// En este programa he puesto 8 taquillas a vender de las mismas 100 entradas sin candado, para ver que se venden más de las que hay.
// Requisitos previos: 06/02
// Qué observar al ejecutar: se venden más de 100 entradas y el contador de disponibles acaba en negativo.
// Salida determinista: NO (cambia cuántas de más se venden, pero siempre sobran ventas)
// Compilar con: -O0 (target sin optimizar, como todos los ejemplos de carreras)
// =====================================================================
// Ejercicio 4 del Tema 2. 8 taquillas (hilos) intentan vender 30 entradas
// cada una; en la sala solo hay 100.
// El sleep de 50 microsegundos simula "imprimir la entrada" entre comprobar
// y restar. Es lo que hace que el fallo aparezca SIEMPRE.
// Cada hilo apunta sus ventas en SU posición del vector: eso no es carrera
// (nadie más escribe en esa posición).
// Calibración (16 núcleos, GCC 13.3, -O0): en 6 ejecuciones, entre 107 y
// 112 entradas vendidas y disponibles = -7 siempre. Más de 107 significa que
// además se han perdido restas (disponibles-- tampoco es atómico).
// Con "taskset -c 0" (un núcleo) también falla: el sleep cede la CPU.
#include <iostream>     // cout
#include <thread>       // thread y this_thread::sleep_for
#include <vector>       // vector
#include <functional>   // ref
#include <chrono>       // microseconds

using namespace std;

const int NUM_TAQUILLAS = 8;
const int QUIERE_VENDER = 30;
int disponibles = 100;   // global: todas las taquillas venden de aquí

// int& vendidas_aqui: es la casilla del vector de main que le toca a esta taquilla.
void vender(int& vendidas_aqui) {
    bool agotado = false;   // bandera para salir del while cuando no quedan entradas
    // Sigo vendiendo mientras no haya llegado a mis 30 Y no se hayan agotado.
    while (vendidas_aqui < QUIERE_VENDER && !agotado) {
        if (disponibles > 0) {                                           // comprobar
            this_thread::sleep_for(chrono::microseconds(50));  // imprimir entrada (otro hilo comprueba mientras)
            disponibles--;                                               // actuar
            vendidas_aqui++;
        } else {
            agotado = true;
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

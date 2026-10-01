// =====================================================================
// Tema 3.8  |  Carrera de tortugas con un hilo por tortuga
// En este programa he hecho una carrera con un hilo por tortuga, en la que cada una avanza al azar de 1 a 5 metros hasta llegar a los 100.
// Requisitos previos: 06_muchos_hilos_con_vector, 01-repaso-cpp (random y sleep_for)
// Qué observar al ejecutar: las tortugas avanzan intercaladas y el orden de llegada cambia.
// Salida determinista: NO (avances aleatorios y planificación de hilos distinta cada vez)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Origen: Tema 1 presencial, "Carrera de las tortugas" (Python con threading).
// Cambios del port: los tres hilos sueltos pasan a un vector (ejercicio 2 del
// Tema 1: para añadir tortugas basta cambiar NUM_TORTUGAS). Cada hilo tiene su
// PROPIO generador aleatorio: compartir uno entre hilos sería una carrera.
// Igual que en el original, todas corren hasta la meta; que solo gane una
// (ejercicio 1 del Tema 2) necesita un candado y se hace en la carpeta 06.
#include <chrono>     // milliseconds
#include <iostream>   // cout
#include <random>     // random_device, mt19937, uniform_int_distribution
#include <string>     // string y to_string
#include <thread>     // thread y this_thread
#include <vector>     // vector

using namespace std;

const int NUM_TORTUGAS = 3;
const int META = 100;

// const string&: recibo el nombre sin copiarlo y sin poder cambiarlo.
void mover_tortuga(const string& nombre) {
    random_device semilla;   // pide al sistema un número de arranque impredecible
    // mt19937: generador de números aleatorios, arrancado con la semilla.
    mt19937 generador(semilla());                     // generador local del hilo
    uniform_int_distribution<int> avance_aleatorio(1, 5);   // enteros de 1 a 5, ambos incluidos

    int distancia = 0;
    while (distancia < META) {   // la tortuga para al llegar a 100 o pasarse
        distancia = distancia + avance_aleatorio(generador);   // saca un número entre 1 y 5
        cout << nombre + " ha recorrido " + to_string(distancia) + " metros\n";
        this_thread::sleep_for(chrono::milliseconds(100));   // duerme este hilo 100 ms
    }
    cout << nombre + " ha llegado a la linea de meta!\n";
}

int main() {
    vector<thread> tortugas;   // un hilo por tortuga
    for (int i = 1; i <= NUM_TORTUGAS; i++) {
        // "Tortuga " + to_string(i) monta el nombre "Tortuga 1", "Tortuga 2"...
        tortugas.push_back(thread(mover_tortuga, "Tortuga " + to_string(i)));
    }

    // Las tortugas se numeran desde 1, pero las posiciones del vector empiezan en 0.
    for (int i = 0; i < NUM_TORTUGAS; i++) {
        tortugas[i].join();   // esperar a que todas lleguen a la meta
    }
    cout << "Carrera de tortugas finalizada!\n";
    return 0;   // 0 = el programa ha terminado bien
}

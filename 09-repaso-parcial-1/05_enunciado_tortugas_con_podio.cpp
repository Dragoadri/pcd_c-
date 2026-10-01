// =====================================================================
// Tema 9.5  |  Ejercicio tipo examen: carrera de tortugas con podio (ENUNCIADO)
// En este programa he dejado preparada la carrera de 5 tortugas con los TODO sin resolver, para que lancéis vosotros los hilos y montéis el podio.
// Requisitos previos: 03-hilos-creacion/08_carrera_tortugas_con_hilos.cpp, 06-mutex
// Qué observar al ejecutar: tal como está no corre ninguna tortuga y el podio sale vacío (faltan los TODO).
// Salida determinista: SI (mientras no se completen los TODO siempre imprime lo mismo)
// Compilar con: cualquier nivel
// =====================================================================
// ENUNCIADO (material original: Tema 1, carrera de tortugas, portada de Python)
// 5 tortugas avanzan de 1 a 5 metros cada 10 ms hasta llegar a 100 metros.
//   TODO 1: crea un hilo por tortuga que ejecute mover_tortuga(id) y espera a todas.
//   TODO 2: cuando una tortuga llega, apunta su id en el vector podio. Varias pueden
//           llegar casi a la vez: el podio no puede perder ni desordenar llegadas.
//   TODO 3: la tortuga que llega PRIMERA imprime "Gana la tortuga X", una sola vez.
//   TODO 4: la línea "llega a la meta" usa << encadenado. ¿Qué problema tiene con
//           varios hilos? Arréglalo.
// Norma: sin break, sin clases propias, lock() y unlock() a mano, todos los hilos con join.
#include <chrono>     // milliseconds, para indicar cuánto tiempo dormir
#include <iostream>   // cout
#include <random>     // random_device, mt19937 y uniform_int_distribution (números aleatorios)
#include <thread>     // thread y this_thread
#include <vector>     // vector

using namespace std;

const int TORTUGAS = 5;
const int META = 100;

vector<int> podio;      // ids en orden de llegada (compartido); empieza vacío

void mover_tortuga(int id) {
    random_device semilla;                          // da una semilla distinta en cada ejecución
    mt19937 generador(semilla());                   // generador de números aleatorios arrancado con esa semilla
    uniform_int_distribution<int> avance(1, 5);     // enteros entre 1 y 5, los dos incluidos
    int distancia = 0;
    while (distancia < META) {                      // sigue hasta llegar a 100 metros o pasarse
        distancia += avance(generador);             // avanza entre 1 y 5 metros al azar
        this_thread::sleep_for(chrono::milliseconds(10));   // cada paso dura 10 ms
    }
    cout << "Tortuga " << id << " llega a la meta\n";   // TODO 4
    // TODO 2 y TODO 3
}

int main() {
    // TODO 1
    cout << "Carrera de " << TORTUGAS << " tortugas hasta " << META << " metros\n";

    cout << "Podio:\n";
    if (podio.empty()) {          // empty(): true si el vector no tiene ningún elemento
        cout << "  (vacio: faltan los TODO)\n";
    }
    // size() da cuántos elementos tiene el vector; (int) lo convierte a int para compararlo con i
    for (int i = 0; i < (int)podio.size(); i++) {
        // i + 1: la casilla 0 del vector es el 1er puesto, la 1 el 2o, etc.
        cout << "  " << i + 1 << "o: tortuga " << podio[i] << "\n";
    }
    return 0;   // 0 = el programa ha terminado bien
}

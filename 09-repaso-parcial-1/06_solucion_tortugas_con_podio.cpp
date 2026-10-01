// =====================================================================
// Tema 9.6  |  Ejercicio tipo examen: carrera de tortugas con podio (SOLUCIÓN)
// En este programa he lanzado las 5 tortugas en hilos y he anotado cada llegada en podio con mtx_podio, leyendo la posición dentro del mismo mutex.
// Requisitos previos: 05_enunciado_tortugas_con_podio.cpp
// Qué observar al ejecutar: 5 llegadas, un único "Gana la tortuga X" y un podio de 5 puestos que coincide con el orden de llegada.
// Salida determinista: NO (la ganadora cambia; siempre hay exactamente una y 5 puestos)
// Compilar con: cualquier nivel
// =====================================================================
// SOLUCIÓN. Los cambios respecto al enunciado van marcados con "CAMBIO".
// TODO 2 y 3: push_back y "¿soy la primera?" van en la MISMA sección crítica.
//   Si se mira podio.size() fuera del mutex, dos tortugas podrían ver tamaño 1 y
//   anunciarse ganadoras las dos.
// TODO 4: el << encadenado son varias escrituras; otro hilo puede colarse entre
//   ellas. Se usa imprimir() con su propio mutex.
#include <chrono>     // milliseconds, para indicar cuánto tiempo dormir
#include <iostream>   // cout
#include <mutex>                                            // CAMBIO (mutex)
#include <random>     // random_device, mt19937 y uniform_int_distribution (números aleatorios)
#include <string>                                           // CAMBIO (string y to_string)
#include <thread>     // thread y this_thread
#include <vector>     // vector

using namespace std;

const int TORTUGAS = 5;
const int META = 100;

vector<int> podio;                                     // ids en orden de llegada (compartido); empieza vacío
mutex mtx_podio;                                       // CAMBIO: protege podio
mutex mtx_consola;                                     // CAMBIO: protege la consola

// const string&: recibe el texto por referencia (sin copiarlo) y sin poder modificarlo.
void imprimir(const string& texto) {                   // CAMBIO
    mtx_consola.lock();       // empieza la sección crítica: la línea entera sale de una vez
    cout << texto << "\n";
    mtx_consola.unlock();     // acaba la sección crítica
}

void mover_tortuga(int id) {
    random_device semilla;                          // da una semilla distinta en cada ejecución
    mt19937 generador(semilla());                   // generador de números aleatorios arrancado con esa semilla
    uniform_int_distribution<int> avance(1, 5);     // enteros entre 1 y 5, los dos incluidos
    int distancia = 0;
    while (distancia < META) {                      // sigue hasta llegar a 100 metros o pasarse
        distancia += avance(generador);             // avanza entre 1 y 5 metros al azar
        this_thread::sleep_for(chrono::milliseconds(10));   // cada paso dura 10 ms
    }

    mtx_podio.lock();                                       // CAMBIO: TODO 2 y 3 (empieza la sección crítica)
    podio.push_back(id);                                    // push_back: apunta el id al final del podio
    int posicion = (int)podio.size();                       // se lee DENTRO del mutex; size() da cuántos han llegado ya
    mtx_podio.unlock();                                     // acaba la sección crítica

    // to_string convierte el número en texto para poder unirlo con +
    imprimir("Tortuga " + to_string(id) + " llega a la meta en posicion " + to_string(posicion));
    if (posicion == 1) {                // posicion es una copia local: nadie más la cambia, se puede mirar sin mutex
        imprimir("Gana la tortuga " + to_string(id));
    }
}

int main() {
    vector<thread> tortugas;                      // CAMBIO: TODO 1
    for (int i = 0; i < TORTUGAS; i++) {
        // thread(mover_tortuga, i) crea el hilo (arranca ya) con id = i; push_back lo añade al final del vector
        tortugas.push_back(thread(mover_tortuga, i));
    }
    for (int i = 0; i < TORTUGAS; i++) {
        tortugas[i].join();                       // espero a que la tortuga i llegue
    }

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

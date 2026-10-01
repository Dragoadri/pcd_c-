// =====================================================================
// Tema 7.7  |  Productor-consumidor con dos semáforos y un mutex
// En este programa he resuelto el búfer acotado con dos counting_semaphore (huecos y elementos) y un mutex, con 2 productores y 2 consumidores.
// Requisitos previos: 06_productor_consumidor_sin_sincronizar.cpp
// Qué observar al ejecutar: perdidos 0, repetidos 0, huecos vacíos 0, aunque haya 2 productores y 2 consumidores.
// Salida determinista: SI (el resumen siempre es el mismo; el orden interno varía pero no se ve)
// Compilar con: -O0 (igual que el 06, para comparar en igualdad)
// =====================================================================
// Solución clásica con tres piezas:
//   huecos    (counting_semaphore, empieza a TAM): cuántos sitios libres quedan
//   elementos (counting_semaphore, empieza a 0)  : cuántos datos hay para leer
//   mtx_bufer (mutex): protege bufer, entrada y salida (hay 2 productores y 2 consumidores)
// ORDEN IMPORTANTE: primero el semáforo, después el mutex. Al revés, un productor
// podría dormirse en huecos.acquire() con el mutex cogido y nadie podría consumir:
// interbloqueo.
#include <functional>   // ref, para pasar variables por referencia a un hilo
#include <iostream>     // cout
#include <mutex>        // mutex
#include <semaphore>    // counting_semaphore (C++20)
#include <thread>       // thread
#include <vector>       // vector

using namespace std;

const int TAM = 5;
const int POR_PRODUCTOR = 50000;
const int TOTAL = 2 * POR_PRODUCTOR;    // 2 productores

vector<int> bufer(TAM, -1);   // TAM casillas, todas a -1 al empezar (-1 = hueco vacío)
int entrada = 0;              // dónde escriben los productores
int salida = 0;               // dónde leen los consumidores

counting_semaphore<TAM> huecos(TAM);    // máximo TAM permisos y empieza con TAM: todo el búfer libre
counting_semaphore<TAM> elementos(0);   // empieza a 0: todavía no hay nada que leer
mutex mtx_bufer;

void productor(int id) {
    for (int i = 0; i < POR_PRODUCTOR; i++) {
        // cada productor, sus propios números: el 0 produce de 0 a 49999 y el 1 de 50000 a 99999
        int valor = id * POR_PRODUCTOR + i;
        huecos.acquire();                      // espero sitio (dormido si está lleno)
        mtx_bufer.lock();                      // empieza la sección crítica (siempre DESPUÉS del semáforo)
        bufer[entrada] = valor;
        entrada = (entrada + 1) % TAM;         // % TAM: después de la casilla 4 vuelve a la 0 (búfer circular)
        mtx_bufer.unlock();                    // acaba la sección crítica
        elementos.release();                   // hay un dato más
    }
}

// vector<int>&: cada consumidor llena el vector de main que le toca, no una copia.
void consumidor(vector<int>& recibidos) {
    for (int i = 0; i < POR_PRODUCTOR; i++) {  // cada consumidor retira la mitad del total
        elementos.acquire();                   // espero dato (dormido si está vacío)
        mtx_bufer.lock();                      // empieza la sección crítica
        recibidos.push_back(bufer[salida]);    // push_back añade el dato al final de recibidos
        bufer[salida] = -1;                    // la casilla vuelve a quedar vacía
        salida = (salida + 1) % TAM;
        mtx_bufer.unlock();                    // acaba la sección crítica
        huecos.release();                      // queda un sitio libre más
    }
}

int main() {
    vector<int> recibidos_a;
    vector<int> recibidos_b;
    thread p0(productor, 0);
    thread p1(productor, 1);
    thread c0(consumidor, ref(recibidos_a));   // ref(): el hilo llena recibidos_a de main, no una copia
    thread c1(consumidor, ref(recibidos_b));
    p0.join();                                 // espero a los cuatro hilos
    p1.join();
    c0.join();
    c1.join();

    // Misma comprobación que en el 06, juntando lo de los dos consumidores
    vector<int> veces(TOTAL, 0);               // veces[n] = cuántas veces ha llegado el número n
    int huecos_vacios = 0;
    // En cada línea: si el valor es un dato (>= 0) lo cuento en veces; si es -1, era un hueco vacío
    for (int i = 0; i < POR_PRODUCTOR; i++) {
        if (recibidos_a[i] >= 0) { veces[recibidos_a[i]]++; } else { huecos_vacios++; }
        if (recibidos_b[i] >= 0) { veces[recibidos_b[i]]++; } else { huecos_vacios++; }
    }
    int perdidos = 0;
    int repetidos = 0;
    for (int i = 0; i < TOTAL; i++) {
        if (veces[i] == 0) {                   // el número i no llegó nunca
            perdidos++;
        } else if (veces[i] > 1) {             // el número i llegó más de una vez
            repetidos++;
        }
    }
    // size() da cuántos elementos tiene cada vector
    cout << "Recibidos: " << recibidos_a.size() + recibidos_b.size() << " de " << TOTAL << "\n";
    cout << "Perdidos: " << perdidos << "  Repetidos: " << repetidos
              << "  Huecos vacios leidos: " << huecos_vacios << "\n";
    return 0;   // 0 = el programa ha terminado bien
}

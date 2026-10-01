// =====================================================================
// Tema 6.3  |  Sección crítica mínima frente a demasiado grande
// En este programa he medido 4 hilos con el sleep fuera del candado (seccion_minima) y dentro (seccion_demasiado_grande), para comparar tiempos.
// Requisitos previos: 06/02
// Qué observar al ejecutar: los dos resultados son correctos, pero la versión grande tarda unas 4 veces más (tantas como hilos).
// Salida determinista: SI (los totales; los tiempos varían un poco)
// Compilar con: cualquier nivel (el trabajo se simula con sleep)
// =====================================================================
// Cada pedido: 2 ms de "preparar" (trabajo LOCAL, no toca nada compartido)
// y después apuntarlo en el total (COMPARTIDO).
#include <iostream>   // cout
#include <thread>     // thread y this_thread::sleep_for
#include <mutex>      // mutex
#include <vector>     // vector, para guardar los hilos
#include <chrono>     // milliseconds y steady_clock

using namespace std;

const int NUM_HILOS = 4;
const int PEDIDOS = 25;
int total_pedidos = 0;   // global y compartida por todos los hilos
mutex mtx;               // global: todos los hilos usan este mismo candado

void seccion_minima() {
    for (int i = 0; i < PEDIDOS; i++) {
        // sleep_for duerme este hilo 2 ms; los demás pueden seguir a la vez.
        this_thread::sleep_for(chrono::milliseconds(2));  // preparar: fuera
        mtx.lock();                                                 // ---- empieza la sección crítica
        total_pedidos++;                                            // solo esto dentro
        mtx.unlock();                                               // ---- acaba la sección crítica
    }
}

void seccion_demasiado_grande() {
    for (int i = 0; i < PEDIDOS; i++) {
        mtx.lock();                                                 // ---- empieza (demasiado pronto)
        // Mientras este hilo duerme con el candado, los otros 3 esperan parados.
        this_thread::sleep_for(chrono::milliseconds(2));  // ERROR: dentro
        total_pedidos++;
        mtx.unlock();                                               // ---- acaba
    }
}

void medir(bool grande) {
    total_pedidos = 0;
    auto inicio = chrono::steady_clock::now();   // steady_clock::now(): la hora de ahora, para medir
    vector<thread> hilos;   // vector de hilos: empieza vacío y vamos añadiendo
    for (int i = 0; i < NUM_HILOS; i++) {
        if (grande) {
            // push_back añade al final del vector un hilo que ya está en marcha.
            hilos.push_back(thread(seccion_demasiado_grande));
        } else {
            hilos.push_back(thread(seccion_minima));
        }
    }
    for (int i = 0; i < NUM_HILOS; i++) {
        hilos[i].join();   // join: espero a cada hilo del vector
    }
    // duration_cast pasa el tiempo transcurrido a milisegundos y .count() saca el número.
    auto ms = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - inicio).count();
    // Operador ternario (condición ? si_cierto : si_falso): elige la etiqueta.
    cout << (grande ? "Seccion demasiado grande: " : "Seccion minima:          ")
              << total_pedidos << " pedidos en " << ms << " ms\n";
}

int main() {
    medir(false);   // primero la versión buena
    medir(true);    // después la versión con todo dentro del candado
    return 0;   // 0 = el programa ha terminado bien
}

// =====================================================================
// Tema 6.2  |  Contador compartido con candado (mutex)
// En este programa he protegido contador++ entre mtx.lock() y mtx.unlock(), para ver que el total sale siempre 2000000 aunque tarde más.
// Requisitos previos: 06/01
// Qué observar al ejecutar: el total es SIEMPRE 2000000, pero tarda bastante más que la versión sin candado.
// Salida determinista: SI (el total; el tiempo varía)
// Compilar con: -O0 (igual que 06/01, para comparar tiempos en igualdad de condiciones)
// =====================================================================
// mutex es el "cartel de ocupado" del baño del avión (carpeta 05):
//   lock()   = preprotocolo: si está ocupado, el hilo se BLOQUEA (no gasta
//              CPU, a diferencia de Peterson) hasta que quede libre.
//   unlock() = postprotocolo: libera y despierta a uno de los que esperan.
// El candado tiene que ser el MISMO objeto para los dos hilos: por eso se
// pasa con ref igual que el contador.
// Calibración (16 núcleos, GCC 13.3, -O0): total exacto en 6 de 6
// ejecuciones, entre 156 y 258 ms (casi 100 veces más lento que 06/01:
// cada lock/unlock cuesta, y los dos hilos se pelean por el candado).
#include <iostream>     // cout
#include <thread>       // thread
#include <mutex>        // mutex
#include <functional>   // ref
#include <chrono>       // steady_clock y milliseconds, para medir tiempos

using namespace std;

const int VECES = 1000000;

// int& y mutex&: los dos por referencia, para usar el contador y el candado de main.
void incrementar(int& contador, mutex& mtx) {
    for (int i = 0; i < VECES; i++) {
        mtx.lock();                     // ---- empieza la sección crítica
        contador++;
        mtx.unlock();                   // ---- acaba la sección crítica
    }
}

int main() {
    int contador = 0;
    mutex mtx;   // el candado nace libre
    auto inicio = chrono::steady_clock::now();   // steady_clock::now(): la hora de ahora, para medir

    // ref(...) también para el mutex: un mutex no se puede copiar, y además
    // tiene que ser el mismo para los dos hilos.
    thread a(incrementar, ref(contador), ref(mtx));
    thread b(incrementar, ref(contador), ref(mtx));
    a.join();   // join: main espera a que termine cada hilo
    b.join();

    auto fin = chrono::steady_clock::now();
    // duration_cast pasa el intervalo a milisegundos y .count() saca el número.
    auto ms = chrono::duration_cast<chrono::milliseconds>(fin - inicio).count();
    cout << "Total: " << contador << " (esperado " << 2 * VECES << ")\n";
    cout << "Tiempo: " << ms << " ms\n";
    return 0;   // 0 = el programa ha terminado bien
}

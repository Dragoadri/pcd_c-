// =====================================================================
// Tema 6.1  |  Contador compartido sin candado (el problema, otra vez)
// En este programa he sumado 1 000 000 de veces desde dos hilos al mismo contador sin candado y he medido el tiempo, para compararlo con 06/02.
// Requisitos previos: 04-hilos-usados-mal, 05-conceptos-y-exclusion-mutua
// Qué observar al ejecutar: el total no llega a 2000000 y el tiempo es pequeño.
// Salida determinista: NO (el resultado depende del entrelazado de los hilos)
// Compilar con: -O0 (target sin optimizar; con -O2 la carrera desaparece)
// =====================================================================
// Calibración (16 núcleos, GCC 13.3, -O0, 2 hilos x 1000000): en 6
// ejecuciones, totales entre 1017704 y 1154303, en 2 o 3 ms.
#include <iostream>     // cout
#include <thread>       // thread
#include <functional>   // ref, para pasar una variable por referencia a un hilo
#include <chrono>       // steady_clock y milliseconds, para medir tiempos

using namespace std;

const int VECES = 1000000;

// int& contador: trabaja sobre la MISMA variable de main, no sobre una copia.
void incrementar(int& contador) {
    for (int i = 0; i < VECES; i++) {
        contador++;                     // sección crítica SIN proteger
    }
}

int main() {
    int contador = 0;
    // steady_clock::now() apunta la hora actual; auto deja que el compilador ponga el tipo.
    auto inicio = chrono::steady_clock::now();

    // ref(contador): los dos hilos reciben la variable de main; sin ref, cada uno sumaría en una copia.
    thread a(incrementar, ref(contador));
    thread b(incrementar, ref(contador));
    a.join();   // join: main espera aquí a que termine el hilo a
    b.join();

    auto fin = chrono::steady_clock::now();
    // fin - inicio es un intervalo de tiempo; duration_cast lo pasa a
    // milisegundos y .count() saca el número para poder imprimirlo.
    auto ms = chrono::duration_cast<chrono::milliseconds>(fin - inicio).count();
    cout << "Total: " << contador << " (esperado " << 2 * VECES << ")\n";
    cout << "Tiempo: " << ms << " ms\n";
    return 0;   // 0 = el programa ha terminado bien
}

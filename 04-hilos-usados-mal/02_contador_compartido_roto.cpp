// =====================================================================
// Tema 4.2  |  Contador compartido sin proteger
// En este programa he lanzado dos hilos que suman 1 000 000 de veces al mismo contador sin protegerlo, para ver que se pierden sumas.
// Requisitos previos: 03-hilos-creacion (05_trampa_1_solucion_std_ref)
// Qué observar al ejecutar: ningún total llega a 2000000 y cada repetición da un número distinto.
// Salida determinista: NO (el resultado depende de cómo se intercalen los dos hilos)
// Compilar con: -O0 (target sin optimizar; con -O2 la carrera casi no se ve, ver 09)
// =====================================================================
// Calibración (16 núcleos, GCC 13.3 y Clang 18, -O0): 2 hilos x 1 000 000
// incrementos. 10 ejecuciones x 5 repeticiones = 50 experimentos: los 50
// fallan, totales entre 1 000 710 y 1 380 151.
// Con un solo núcleo (taskset -c 0 ./02_contador_compartido_roto) fallan
// solo 14 de 100, casi siempre con 1 000 000 exacto: sigue estando mal, pero
// el fallo es mucho menos probable (típico de una máquina virtual con 1 CPU).
#include <functional>   // ref
#include <iostream>     // cout
#include <thread>       // thread

using namespace std;

const int INCREMENTOS = 1000000;
const int REPETICIONES = 5;

// int&: los dos hilos reciben la MISMA variable contador, no una copia cada uno.
void incrementar(int& contador) {
    for (int i = 0; i < INCREMENTOS; i++) {
        contador++;   // parece una sola operación... ¿lo es?
        // No: son tres pasos (leer, sumar 1, escribir) y el otro hilo puede colarse entre ellos.
    }
}

int main() {
    // Repetimos el mismo experimento varias veces dentro del programa
    for (int r = 1; r <= REPETICIONES; r++) {
        int contador = 0;   // contador nuevo, a 0, en cada repetición
        thread hilo_1(incrementar, ref(contador));   // ref: compartida de verdad
        thread hilo_2(incrementar, ref(contador));
        hilo_1.join();   // espero a los dos hilos antes de leer el resultado
        hilo_2.join();

        cout << "Repeticion " << r << ": contador = " << contador
                  << "  (esperado " << 2 * INCREMENTOS << ", perdidos "
                  << 2 * INCREMENTOS - contador << ")\n";   // perdidos = lo esperado menos lo obtenido
    }
    return 0;   // 0 = el programa ha terminado bien
}

// =====================================================================
// Tema 1.14  |  Números aleatorios (<random>) y tiempo (<chrono>, sleep_for)
// En este programa he sacado 5 avances aleatorios de 1 a 5 con mt19937, he dormido 100 ms entre pasos y he medido el tiempo total con steady_clock.
// Requisitos previos: 12_mismo_ejercicio_con_for.cpp
// Qué observar al ejecutar: avances distintos en cada ejecución y unos 500 ms en total (nunca exactos).
// Salida determinista: NO (semilla aleatoria y tiempo medido variable)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <chrono>     // steady_clock, milliseconds y duration_cast
#include <iostream>   // cout
#include <random>     // random_device, mt19937 y uniform_int_distribution
#include <thread>     // this_thread::sleep_for

using namespace std;

int main() {
    // Aleatoriedad en tres piezas:
    random_device semilla;                        // semilla impredecible del sistema
    // semilla() con paréntesis pide un número a random_device y con él arranca el generador.
    mt19937 generador(semilla());                 // generador de la secuencia
    uniform_int_distribution<int> avance(1, 5);   // enteros de 1 a 5, ambos incluidos

    // steady_clock: el reloj para medir duraciones (nunca va hacia atrás).
    // now() devuelve el instante actual; time_point es el tipo de ese instante.
    chrono::steady_clock::time_point inicio = chrono::steady_clock::now();

    for (int paso = 1; paso <= 5; paso++) {
        // avance(generador): la distribución usa el generador para dar un número de 1 a 5.
        cout << "Paso " << paso << ": avanzo " << avance(generador) << " metros\n";
        // Suspende el hilo actual AL MENOS 100 ms.
        // chrono::milliseconds(100) es una duración de 100 milisegundos.
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    chrono::steady_clock::time_point fin = chrono::steady_clock::now();
    // fin - inicio es la duración; duration_cast la pasa a milisegundos enteros.
    chrono::milliseconds duracion =
        chrono::duration_cast<chrono::milliseconds>(fin - inicio);
    // .count() devuelve el número de milisegundos para poder imprimirlo.
    cout << "Tiempo total: " << duracion.count() << " ms\n";

    // Por qué no rand(): peor calidad, rango según implementación y estado global.
    // Reglas para hilos: cada hilo tendrá SU PROPIO generador (un generador
    // compartido es otra variable compartida), y sleep_for no sirve para
    // "esperar a que otro hilo acabe": el tiempo no está garantizado.
    return 0;   // 0 = el programa ha terminado bien
}

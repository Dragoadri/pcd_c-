// =====================================================================
// Tema 8.5  |  barrier: la barrera ya hecha (AVISO: no es estándar)
// En este programa he repetido la ruta del 04 cambiando la barrera artesanal por barrier refugio(4) y su arrive_and_wait(), que solo existe en libstdc++.
// Requisitos previos: 04_barrera_artesanal.cpp
// Qué observar al ejecutar: lo mismo que en el 04, con la barrera reducida a una línea.
// Salida determinista: NO (los tiempos de marcha son aleatorios; la salida conjunta se repite siempre)
// Compilar con: GCC o Clang en Linux (libstdc++). NO compila en macOS (libc++) ni en MSVC
// =====================================================================
// AVISO IMPORTANTE, verificado con GCC 13 y Clang 18:
//   barrier se propuso para C++20 (propuesta P1135) pero se RETIRÓ antes de
//   aprobar el estándar. C++20 solo trae <latch> y <semaphore>.
//   libstdc++ (la biblioteca de GCC, que también usa Clang en Linux) incluye <barrier>
//   como extensión propia. En macOS (libc++) y en Visual Studio da "file not found".
//   Si tu entorno no la tiene: usa barrera_esperar() del 04, que es portable.
// Uso:
//   barrier refugio(N);   // se abre cada vez que llegan N hilos
//   refugio.arrive_and_wait(); // llego y espero a los demás
#include <barrier>    // barrier (extensión de libstdc++, no es estándar)
#include <chrono>     // steady_clock, duration_cast y milliseconds
#include <iostream>   // cout
#include <mutex>      // mutex
#include <random>     // random_device, mt19937 y uniform_int_distribution (números aleatorios)
#include <string>     // string y to_string
#include <thread>     // thread y this_thread
#include <vector>     // vector

using namespace std;

const int EXCURSIONISTAS = 4;
const int ETAPAS = 2;

barrier refugio(EXCURSIONISTAS);     // sustituye a las 3 variables y la función del 04

// time_point: un instante concreto del reloj steady_clock (un reloj que nunca va hacia atrás)
chrono::steady_clock::time_point inicio;
mutex mtx_consola;    // protege cout

// const string&: recibe el texto por referencia (sin copiarlo) y sin poder modificarlo.
void imprimir(const string& texto) {
    mtx_consola.lock();       // solo un hilo escribe a la vez
    cout << texto << "\n";
    mtx_consola.unlock();
}

// Devuelve como texto los milisegundos que han pasado desde inicio, por ejemplo "350 ms".
string reloj() {
    auto transcurrido = chrono::steady_clock::now() - inicio;   // now() menos inicio: una duración
    // duration_cast la pasa a milisegundos, .count() saca el número y to_string lo convierte a texto
    return to_string(chrono::duration_cast<chrono::milliseconds>(transcurrido).count()) + " ms";
}

void excursionista(int id) {
    random_device semilla;                               // da una semilla distinta en cada ejecución
    mt19937 generador(semilla());                        // generador de números aleatorios arrancado con esa semilla
    uniform_int_distribution<int> marcha_ms(100, 700);   // enteros entre 100 y 700, los dos incluidos
    for (int etapa = 1; etapa <= ETAPAS; etapa++) {
        this_thread::sleep_for(chrono::milliseconds(marcha_ms(generador)));   // camina hasta el refugio
        imprimir("[" + reloj() + "] Excursionista " + to_string(id) + " llega al refugio " + to_string(etapa));
        refugio.arrive_and_wait();                       // llego y espero a que estén los 4; entonces salimos todos
        imprimir("[" + reloj() + "] Excursionista " + to_string(id) + " sale del refugio " + to_string(etapa));
    }
}

int main() {
    inicio = chrono::steady_clock::now();   // pongo el cronómetro a cero
    vector<thread> grupo;                   // vector de hilos: un hilo por excursionista
    for (int i = 0; i < EXCURSIONISTAS; i++) {
        // thread(excursionista, i) crea el hilo (arranca ya) con id = i; push_back lo añade al final
        grupo.push_back(thread(excursionista, i));
    }
    for (int i = 0; i < EXCURSIONISTAS; i++) {
        grupo[i].join();                    // espero a que el excursionista i termine
    }
    cout << "Todo el grupo ha terminado la ruta\n";
    return 0;   // 0 = el programa ha terminado bien
}

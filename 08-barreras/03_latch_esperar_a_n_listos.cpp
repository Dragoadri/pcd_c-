// =====================================================================
// Tema 8.3  |  latch(N): esperar a que N hilos estén listos
// En este programa he usado un latch(N) para que main espere a que los 4 corredores estén listos y otro latch(1) para darles la salida a la vez.
// Requisitos previos: 02_latch_pistoletazo_de_salida.cpp
// Qué observar al ejecutar: "Juez: todos listos" aparece después del último "listo", aunque alguien tarde 1,5 s.
// Salida determinista: NO (el orden de "listo" cambia; el juez siempre dispara tras el último)
// Compilar con: cualquier nivel
// =====================================================================
// Dos latch con papeles distintos:
//   listos(N):      lo bajan los CORREDORES, uno cada uno. Main espera en él.
//   pistoletazo(1): lo baja MAIN. Los corredores esperan en él.
// Ya no hay ningún sleep "a ojo" en main (recuerda 04-hilos-usados-mal: coordinar con sleep falla).
#include <chrono>     // steady_clock, duration_cast y milliseconds
#include <iostream>   // cout
#include <latch>      // latch (C++20)
#include <mutex>      // mutex
#include <random>     // random_device, mt19937 y uniform_int_distribution (números aleatorios)
#include <string>     // string y to_string
#include <thread>     // thread y this_thread
#include <vector>     // vector

using namespace std;

const int CORREDORES = 4;

latch listos(CORREDORES);   // empieza en 4: cada corredor lo baja 1 cuando está listo
latch pistoletazo(1);       // empieza en 1: main lo baja a 0 para dar la salida
// time_point: un instante concreto del reloj steady_clock (un reloj que nunca va hacia atrás)
chrono::steady_clock::time_point inicio;
vector<int> salida_ms(CORREDORES, 0);   // cada hilo escribe SOLO su casilla
mutex mtx_consola;                      // protege cout

// const string&: recibe el texto por referencia (sin copiarlo) y sin poder modificarlo.
void imprimir(const string& texto) {
    mtx_consola.lock();       // solo un hilo escribe a la vez
    cout << texto << "\n";
    mtx_consola.unlock();
}

int milisegundos_desde_inicio() {
    // now() da el instante actual; restar dos instantes da una duración (auto deduce el tipo solo)
    auto transcurrido = chrono::steady_clock::now() - inicio;
    // duration_cast pasa la duración a milisegundos, .count() saca el número y (int) lo convierte a int
    return (int)chrono::duration_cast<chrono::milliseconds>(transcurrido).count();
}

void corredor(int id) {
    random_device semilla;                                   // da una semilla distinta en cada ejecución
    mt19937 generador(semilla());                            // generador de números aleatorios arrancado con esa semilla
    uniform_int_distribution<int> preparacion_ms(100, 1500);   // puede pasar de 1 s
    this_thread::sleep_for(chrono::milliseconds(preparacion_ms(generador)));   // se prepara
    // El texto sigue en la línea de abajo: el + une los trozos; to_string pasa los números a texto
    imprimir("Corredor " + to_string(id) + " listo en t = "
             + to_string(milisegundos_desde_inicio()) + " ms");

    listos.count_down();      // aviso: uno menos por llegar
    pistoletazo.wait();       // y espero la salida
    salida_ms[id] = milisegundos_desde_inicio();
}

int main() {
    inicio = chrono::steady_clock::now();     // pongo el cronómetro a cero
    vector<thread> corredores;                // vector de hilos: guardo los 4 para hacerles join después
    for (int i = 0; i < CORREDORES; i++) {
        // thread(corredor, i) crea el hilo (arranca ya) con id = i; push_back lo añade al final del vector
        corredores.push_back(thread(corredor, i));
    }

    listos.wait();                           // bloqueado hasta que los N hayan hecho count_down
    imprimir("Juez: todos listos. PUM!");
    pistoletazo.count_down();                // 1 -> 0: salen todos a la vez

    for (int i = 0; i < CORREDORES; i++) {
        corredores[i].join();                // espero a que el corredor i termine
    }
    // Ya han terminado todos: main puede leer salida_ms sin miedo
    for (int i = 0; i < CORREDORES; i++) {
        cout << "Corredor " << i << " sale en t = " << salida_ms[i] << " ms\n";
    }
    return 0;   // 0 = el programa ha terminado bien
}

// =====================================================================
// Tema 8.2  |  latch(1): el pistoletazo de salida
// En este programa he parado a 4 corredores en un latch(1) y los he soltado todos a la vez cuando el juez hace count_down tras esperar 1 segundo.
// Requisitos previos: 01_salida_desigual_sin_barrera.cpp
// Qué observar al ejecutar: todos salen en el mismo milisegundo (diferencia 0 o 1 ms).
// Salida determinista: NO (el orden de "listo" cambia; la diferencia de salida es siempre casi 0)
// Compilar con: cualquier nivel
// =====================================================================
// latch es estándar de C++20 (<latch>). Es un contador que solo baja:
//   wait():       el hilo se bloquea mientras el contador sea mayor que 0.
//   count_down(): resta 1. Al llegar a 0 despierta a todos y ya NO se vuelve a cerrar.
// Punto débil (para la pregunta de clase): el juez espera 1 segundo "a ojo".
// Si un corredor tarda más de 1 s en prepararse, sale tarde. Lo arregla el 03.
#include <chrono>     // steady_clock, duration_cast, milliseconds y seconds
#include <iostream>   // cout
#include <latch>      // latch (C++20)
#include <mutex>      // mutex
#include <random>     // random_device, mt19937 y uniform_int_distribution (números aleatorios)
#include <string>     // string y to_string
#include <thread>     // thread y this_thread
#include <vector>     // vector

using namespace std;

const int CORREDORES = 4;

latch pistoletazo(1);   // contador que empieza en 1: cerrado hasta que alguien haga count_down
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
    uniform_int_distribution<int> preparacion_ms(100, 800);  // enteros entre 100 y 800, los dos incluidos
    this_thread::sleep_for(chrono::milliseconds(preparacion_ms(generador)));   // se prepara
    imprimir("Corredor " + to_string(id) + " listo en la linea");   // to_string: número a texto

    pistoletazo.wait();                            // bloqueado hasta el disparo (hasta que el latch llegue a 0)
    salida_ms[id] = milisegundos_desde_inicio();
}

int main() {
    inicio = chrono::steady_clock::now();     // pongo el cronómetro a cero
    vector<thread> corredores;                // vector de hilos: guardo los 4 para hacerles join después
    for (int i = 0; i < CORREDORES; i++) {
        // thread(corredor, i) crea el hilo (arranca ya) con id = i; push_back lo añade al final del vector
        corredores.push_back(thread(corredor, i));
    }

    this_thread::sleep_for(chrono::seconds(1));   // el juez espera "un rato"
    imprimir("Juez: PUM!");
    pistoletazo.count_down();                                 // 1 -> 0: salen todos

    for (int i = 0; i < CORREDORES; i++) {
        corredores[i].join();                 // espero a que el corredor i termine
    }
    // Ya han terminado todos: main puede leer salida_ms sin miedo
    for (int i = 0; i < CORREDORES; i++) {
        cout << "Corredor " << i << " sale en t = " << salida_ms[i] << " ms\n";
    }
    return 0;   // 0 = el programa ha terminado bien
}

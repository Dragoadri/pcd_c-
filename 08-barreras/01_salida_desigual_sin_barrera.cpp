// =====================================================================
// Tema 8.1  |  El problema: cada hilo arranca cuando le da la gana
// En este programa he lanzado 4 corredores que se preparan un tiempo aleatorio y salen sin esperarse, y he medido en qué milisegundo sale cada uno.
// Requisitos previos: 07-semaforos, 03-hilos-creacion (carrera de tortugas)
// Qué observar al ejecutar: la tabla final: los corredores salen con cientos de milisegundos de diferencia.
// Salida determinista: NO (los tiempos de preparación son aleatorios)
// Compilar con: cualquier nivel
// =====================================================================
// Cada corredor tarda un tiempo aleatorio en atarse las zapatillas y sale en cuanto
// termina. Ninguno espera a los demás: la carrera es injusta.
#include <chrono>     // steady_clock, duration_cast y milliseconds (medir y dormir)
#include <iostream>   // cout
#include <random>     // random_device, mt19937 y uniform_int_distribution (números aleatorios)
#include <thread>     // thread y this_thread
#include <vector>     // vector

using namespace std;

const int CORREDORES = 4;

// time_point: un instante concreto del reloj steady_clock (un reloj que nunca va hacia atrás)
chrono::steady_clock::time_point inicio;     // se fija antes de crear los hilos
vector<int> salida_ms(CORREDORES, 0);        // cada hilo escribe SOLO su casilla (por eso no hace falta mutex)

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
    // se ata las zapatillas: duerme un tiempo al azar entre 100 y 800 ms
    this_thread::sleep_for(chrono::milliseconds(preparacion_ms(generador)));

    salida_ms[id] = milisegundos_desde_inicio();  // ¡sale!
}

int main() {
    inicio = chrono::steady_clock::now();     // pongo el cronómetro a cero
    vector<thread> corredores;                // vector de hilos: guardo los 4 para hacerles join después
    for (int i = 0; i < CORREDORES; i++) {
        // thread(corredor, i) crea el hilo (arranca ya) con id = i; push_back lo añade al final del vector
        corredores.push_back(thread(corredor, i));
    }
    for (int i = 0; i < CORREDORES; i++) {
        corredores[i].join();                 // espero a que el corredor i termine
    }

    int primera = salida_ms[0];               // empiezo suponiendo que el 0 es el primero y el último
    int ultima = salida_ms[0];
    for (int i = 0; i < CORREDORES; i++) {
        cout << "Corredor " << i << " sale en t = " << salida_ms[i] << " ms\n";
        if (salida_ms[i] < primera) { primera = salida_ms[i]; }   // me quedo con la salida más temprana
        if (salida_ms[i] > ultima) { ultima = salida_ms[i]; }     // y con la más tardía
    }
    cout << "Diferencia entre el primero y el ultimo: " << ultima - primera << " ms\n";
    return 0;   // 0 = el programa ha terminado bien
}

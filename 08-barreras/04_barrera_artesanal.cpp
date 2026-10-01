// =====================================================================
// Tema 8.4  |  Barrera artesanal con mutex y condition_variable
// En este programa he construido una barrera reutilizable, barrera_esperar(), con mutex y condition_variable, y 4 excursionistas se esperan en cada refugio.
// Requisitos previos: 03_latch_esperar_a_n_listos.cpp, 07-semaforos/05_semaforo_artesanal.cpp
// Qué observar al ejecutar: en cada refugio los 4 salen en el mismo milisegundo, aunque llegaron escalonados.
// Salida determinista: NO (los tiempos de marcha son aleatorios; la salida conjunta se repite siempre)
// Compilar con: cualquier nivel
// =====================================================================
// Diferencia con latch: un latch, una vez abierto, se queda abierto para siempre.
// Una BARRERA se abre cuando llega el N-ésimo hilo y se vuelve a cerrar para la
// siguiente ronda. Aquí: 4 excursionistas, 2 etapas, se esperan en cada refugio.
// Es portable: solo usa piezas estándar (a diferencia de barrier, ver 05).
#include <chrono>               // steady_clock, duration_cast y milliseconds
#include <condition_variable>   // condition_variable
#include <iostream>             // cout
#include <mutex>                // mutex y unique_lock
#include <random>               // random_device, mt19937 y uniform_int_distribution (números aleatorios)
#include <string>               // string y to_string
#include <thread>               // thread y this_thread
#include <vector>               // vector

using namespace std;

const int EXCURSIONISTAS = 4;
const int ETAPAS = 2;

// ---- La barrera: tres variables y una función ----
int barrera_llegados = 0;        // cuántos esperan en esta ronda
int barrera_aperturas = 0;       // cuántas veces se ha abierto
mutex barrera_mtx;               // protege barrera_llegados y barrera_aperturas
condition_variable barrera_cv;   // aquí duermen los que esperan a que se abra

void barrera_esperar() {
    // unique_lock: excepción de la norma, cv.wait() lo exige
    unique_lock<mutex> candado(barrera_mtx);     // coge el mutex: empieza la sección crítica
    int apertura_que_espero = barrera_aperturas; // apunto cuántas aperturas había al llegar: saldré cuando cambie
    barrera_llegados++;
    if (barrera_llegados == EXCURSIONISTAS) {    // soy el último: abro
        barrera_llegados = 0;                      // y queda vacía para la ronda siguiente
        barrera_aperturas++;
        barrera_cv.notify_all();                   // despierta a TODOS, no a uno
    }
    // WHILE: sigo durmiendo mientras no haya habido una apertura nueva (también protege de
    // despertares espurios). El último no entra en el bucle: acaba de cambiar barrera_aperturas.
    while (barrera_aperturas == apertura_que_espero) {
        barrera_cv.wait(candado);                  // suelta el mutex y duerme; al despertar lo recupera
    }
    candado.unlock();                              // acaba la sección crítica
}
// ---- Fin de la barrera ----

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
        barrera_esperar();                               // aquí espera hasta que lleguen los 4
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

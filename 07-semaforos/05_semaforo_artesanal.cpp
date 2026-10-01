// =====================================================================
// Tema 7.5  |  Semáforo artesanal: un contador protegido
// En este programa he construido un semáforo a mano con un int, un mutex y una condition_variable, y lo he usado para el aforo de 4 mesas.
// Requisitos previos: 02_restaurante_con_counting_semaphore.cpp, 04_condition_variable_basica.cpp
// Qué observar al ejecutar: la misma salida que el 02: nunca más de 4 mesas ocupadas.
// Salida determinista: NO (el orden de los clientes cambia; el máximo nunca pasa de 4)
// Compilar con: cualquier nivel
// =====================================================================
// Lo que hace counting_semaphore por dentro, escrito con funciones libres.
// Pon este fichero al lado del 02: el main y el cliente son iguales,
// solo cambia mesas_libres.acquire() por adquirir() y release() por liberar().
#include <condition_variable>   // condition_variable
#include <iostream>             // cout
#include <mutex>                // mutex y unique_lock
#include <string>               // string y to_string
#include <thread>               // thread y this_thread
#include <vector>               // vector
#include <chrono>               // milliseconds, para indicar cuánto tiempo dormir

using namespace std;

const int MESAS = 4;
const int CLIENTES = 10;

// ---- El semáforo: tres variables y dos funciones ----
int semaforo_permisos = MESAS;          // el contador
mutex semaforo_mtx;                // lo protege
condition_variable semaforo_cv;    // duerme a quien no encuentra permiso

void adquirir() {
    // unique_lock: excepción de la norma, cv.wait() lo exige (ver 04)
    unique_lock<mutex> candado(semaforo_mtx);   // coge el mutex: empieza la sección crítica
    while (semaforo_permisos == 0) {            // WHILE: al despertar puede que otro ya se haya llevado el permiso
        semaforo_cv.wait(candado);              // suelta el mutex y duerme; al despertar lo vuelve a coger
    }
    semaforo_permisos--;                        // cojo un permiso (lo mismo que hace acquire)
    candado.unlock();                           // acaba la sección crítica
}

void liberar() {
    semaforo_mtx.lock();          // empieza la sección crítica
    semaforo_permisos++;          // devuelvo el permiso (lo mismo que hace release)
    semaforo_mtx.unlock();        // acaba la sección crítica
    semaforo_cv.notify_one();     // despierta a UN hilo dormido en adquirir(), si hay alguno
}
// ---- Fin del semáforo ----

mutex mtx_consola;      // protege cout
mutex mtx_ocupacion;    // protege los dos contadores de abajo (solo sirven para medir)
int mesas_ocupadas = 0;
int maximo_observado = 0;   // el valor más alto que ha llegado a tener mesas_ocupadas

// const string&: recibe el texto por referencia (sin copiarlo) y sin poder modificarlo.
void imprimir(const string& texto) {
    mtx_consola.lock();       // solo un hilo escribe a la vez
    cout << texto << "\n";
    mtx_consola.unlock();
}

void cliente(int id) {
    adquirir();                                 // como mesas_libres.acquire() del 02: aquí espera mesa
    mtx_ocupacion.lock();                       // empieza la sección crítica de los contadores
    mesas_ocupadas++;
    if (mesas_ocupadas > maximo_observado) {    // ¿es la ocupación más alta vista hasta ahora?
        maximo_observado = mesas_ocupadas;
    }
    mtx_ocupacion.unlock();                     // acaba la sección crítica

    // to_string convierte el número en texto para poder unirlo con +
    imprimir("Cliente " + to_string(id) + " se sienta");
    this_thread::sleep_for(chrono::milliseconds(200));   // come: duerme ESTE hilo 200 ms

    mtx_ocupacion.lock();
    mesas_ocupadas--;
    mtx_ocupacion.unlock();
    imprimir("Cliente " + to_string(id) + " se va");
    liberar();                                  // como mesas_libres.release() del 02: deja la mesa
}

int main() {
    vector<thread> clientes;    // vector de hilos: guardo los 10 para hacerles join después
    for (int i = 0; i < CLIENTES; i++) {
        // thread(cliente, i) crea el hilo (arranca ya) con id = i; push_back lo añade al final del vector
        clientes.push_back(thread(cliente, i));
    }
    for (int i = 0; i < CLIENTES; i++) {
        clientes[i].join();     // espero a que el cliente i termine
    }
    cout << "Maximo ocupadas a la vez: " << maximo_observado << " de " << MESAS << "\n";
    return 0;   // 0 = el programa ha terminado bien
}

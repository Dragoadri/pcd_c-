// =====================================================================
// Tema 7.8  |  Filósofos comensales: un semáforo que evita el interbloqueo
// En este programa he sentado a 5 filósofos con 5 tenedores (mutex) y un camarero counting_semaphore de 4 permisos que evita el interbloqueo.
// Requisitos previos: 02_restaurante_con_counting_semaphore.cpp, 06-mutex (interbloqueo con dos mutex)
// Qué observar al ejecutar: los 5 filósofos comen 3 veces cada uno y el programa termina.
// Salida determinista: NO (el orden de las comidas cambia; siempre terminan los 5)
// Compilar con: cualquier nivel
// =====================================================================
// 5 filósofos en una mesa redonda, un tenedor entre cada dos. Para comer hacen falta
// el tenedor de la izquierda y el de la derecha.
// Peligro: si los 5 cogen a la vez el izquierdo, los 5 esperan el derecho para siempre
// (interbloqueo, deadlock: espera circular). La pausa tras coger el izquierdo lo provoca.
// Solución del camarero: solo deja sentarse a 4 a la vez. Con 4 sentados y 5 tenedores,
// al menos uno consigue los dos y la cadena se rompe.
// PRUEBA EN CLASE: comenta camarero.acquire() y camarero.release(), ejecuta y verás que
// se queda bloqueado para siempre (páralo con el botón Stop de CLion). Luego descoméntalas.
#include <iostream>    // cout
#include <mutex>       // mutex
#include <random>      // random_device, mt19937 y uniform_int_distribution (números aleatorios)
#include <semaphore>   // counting_semaphore (C++20)
#include <string>      // string y to_string
#include <thread>      // thread y this_thread
#include <vector>      // vector
#include <chrono>      // milliseconds, para indicar cuánto tiempo dormir

using namespace std;

const int FILOSOFOS = 5;
const int COMIDAS = 3;

vector<mutex> tenedores(FILOSOFOS);   // 5 mutex, uno por tenedor: de tenedores[0] a tenedores[4]
counting_semaphore<FILOSOFOS> camarero(FILOSOFOS - 1);   // máximo 5, empieza con FILOSOFOS - 1 = 4 permisos

mutex mtx_consola;    // protege cout

// const string&: recibe el texto por referencia (sin copiarlo) y sin poder modificarlo.
void imprimir(const string& texto) {
    mtx_consola.lock();       // solo un hilo escribe a la vez
    cout << texto << "\n";
    mtx_consola.unlock();
}

void filosofo(int id) {
    random_device semilla;                            // da una semilla distinta en cada ejecución
    mt19937 generador(semilla());                     // generador de números aleatorios arrancado con esa semilla
    uniform_int_distribution<int> pensar_ms(50, 150); // enteros entre 50 y 150, los dos incluidos
    int izquierdo = id;
    int derecho = (id + 1) % FILOSOFOS;   // % : el filósofo 4 usa como derecho el tenedor 0 (mesa redonda)

    for (int comida = 1; comida <= COMIDAS; comida++) {
        // pensar_ms(generador) saca un número al azar entre 50 y 150: piensa ese tiempo
        this_thread::sleep_for(chrono::milliseconds(pensar_ms(generador)));

        camarero.acquire();                              // pido sitio en la mesa
        tenedores[izquierdo].lock();                     // cojo el izquierdo (si lo tiene otro, espero)
        this_thread::sleep_for(chrono::milliseconds(50));   // provoca el peligro
        tenedores[derecho].lock();                       // y el derecho: aquí empieza a comer con los dos

        imprimir("Filosofo " + to_string(id) + " come (" + to_string(comida) + "/3)");
        this_thread::sleep_for(chrono::milliseconds(100));  // come durante 100 ms

        tenedores[derecho].unlock();                     // suelto los tenedores en orden inverso
        tenedores[izquierdo].unlock();
        camarero.release();                              // dejo el sitio
    }
    imprimir("Filosofo " + to_string(id) + " ha terminado");
}

int main() {
    vector<thread> mesa;      // vector de hilos: un hilo por filósofo
    for (int i = 0; i < FILOSOFOS; i++) {
        // thread(filosofo, i) crea el hilo (arranca ya) con id = i; push_back lo añade al final del vector
        mesa.push_back(thread(filosofo, i));
    }
    for (int i = 0; i < FILOSOFOS; i++) {
        mesa[i].join();       // espero a que el filósofo i termine
    }
    cout << "Todos han comido. Sin interbloqueo.\n";
    return 0;   // 0 = el programa ha terminado bien
}

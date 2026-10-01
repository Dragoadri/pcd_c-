// =====================================================================
// Tema 7.4  |  condition_variable: esperar a que algo sea cierto
// En este programa he hecho que el camarero duerma en una condition_variable hasta que el cocinero ponga plato_listo a true y le avise.
// Requisitos previos: 03_binary_semaphore_como_candado.cpp, 06-mutex
// Qué observar al ejecutar: el camarero sirve siempre DESPUÉS de que el cocinero diga que está listo.
// Salida determinista: SI (el orden de los mensajes lo impone la condición, no la suerte)
// Compilar con: cualquier nivel
// =====================================================================
// Tres piezas que van SIEMPRE juntas:
//   1. una condición guardada en una variable (plato_listo)
//   2. un mutex que protege esa variable
//   3. la condition_variable, que duerme y despierta hilos
// EXCEPCIÓN A LA NORMA: cv.wait() necesita soltar el mutex mientras duerme y volver a
// cogerlo al despertar, y para eso exige un unique_lock (un mutex suelto no vale).
// Es el único sitio de la asignatura donde se usa. Lo soltamos a mano con unlock().
#include <condition_variable>   // condition_variable
#include <iostream>             // cout
#include <mutex>                // mutex y unique_lock
#include <thread>               // thread y this_thread
#include <chrono>               // milliseconds, para indicar cuánto tiempo dormir

using namespace std;

mutex mtx_cocina;
condition_variable cv_cocina;      // donde duermen los hilos que esperan a que plato_listo cambie
bool plato_listo = false;          // la condición: protegida por mtx_cocina

void camarero() {
    unique_lock<mutex> candado(mtx_cocina);   // lock: al crear el candado ya coge el mutex (empieza la sección crítica)
    // WHILE y no IF: el hilo puede despertar sin que nadie avise (despertar espurio)
    // o despertar cuando otro ya se ha llevado el plato. Siempre se vuelve a comprobar.
    while (!plato_listo) {         // !plato_listo: "mientras el plato NO esté listo"
        cout << "Camarero: espero el plato (duermo)\n";
        cv_cocina.wait(candado);   // suelta el mutex, duerme, al despertar lo recupera
    }
    cout << "Camarero: sirvo el plato\n";
    candado.unlock();                                    // fin de la sección crítica
}

void cocinero() {
    this_thread::sleep_for(chrono::milliseconds(500));   // cocinar
    mtx_cocina.lock();             // empieza la sección crítica
    plato_listo = true;            // cambiar la condición SIEMPRE con el mutex cogido
    cout << "Cocinero: plato listo, aviso\n";
    mtx_cocina.unlock();           // acaba la sección crítica
    cv_cocina.notify_one();        // despierta a UN hilo que esté en wait
}

int main() {
    thread h_camarero(camarero);   // arranca el hilo del camarero
    thread h_cocinero(cocinero);   // y el del cocinero
    h_camarero.join();             // espero a que los dos terminen
    h_cocinero.join();
    // Sin la variable plato_listo el aviso se podría PERDER: si el cocinero avisa
    // antes de que el camarero llegue a wait(), el camarero dormiría para siempre.
    return 0;   // 0 = el programa ha terminado bien
}

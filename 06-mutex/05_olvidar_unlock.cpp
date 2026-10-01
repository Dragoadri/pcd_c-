// =====================================================================
// Tema 6.5  |  Olvidar el unlock en un camino del if
// En este programa he olvidado el unlock en la rama else de retirar, para ver que después el hilo de ingresar ya no consigue el candado.
// Requisitos previos: 06/02, 06/04
// Qué observar al ejecutar: tras el retiro fallido, el hilo del ingreso espera 1 s y se rinde: el candado sigue tomado.
// Salida determinista: SI (la ventana de espera es fija y el orden lo fija main)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Es el cartel del baño que alguien olvida girar al salir (carpeta 05).
// Con mutex el hilo del ingreso se quedaría bloqueado PARA SIEMPRE.
// Para que el programa termine usamos timed_mutex, que añade
// try_lock_for(tiempo): intenta entrar durante ese tiempo y devuelve false
// si no lo consigue.
// SOLUCIÓN: añadir mtx.unlock() también en la rama else (ver comentario).
// Por eso existe lock_guard (RAII) fuera de esta asignatura: suelta
// solo. Aquí lo hacemos a mano para ver dónde acaba la sección crítica.
#include <iostream>   // cout
#include <thread>     // thread
#include <mutex>      // timed_mutex (un mutex que admite try_lock_for)
#include <chrono>     // seconds, para indicar cuánto tiempo intentarlo

using namespace std;

int saldo = 100;   // dato compartido
timed_mutex mtx;   // candado global que protege saldo

void retirar(int cantidad) {
    mtx.lock();                   // ---- empieza la sección crítica
    if (saldo >= cantidad) {
        saldo = saldo - cantidad;
        cout << "Retiro de " << cantidad << " hecho. Saldo: " << saldo << "\n";
        mtx.unlock();             // ---- acaba (en este camino sí)
    } else {
        cout << "Retiro de " << cantidad << " rechazado: saldo insuficiente\n";
        // mtx.unlock();   <- OLVIDADO. Descomenta esta línea para arreglarlo.
    }
    // Si hemos ido por el else, salimos de la función con el candado aún tomado.
}

void ingresar(int cantidad) {
    // try_lock_for intenta coger el candado durante 1 segundo como mucho:
    // devuelve true si lo consigue (y ya lo tenemos) o false si se acaba el tiempo.
    if (mtx.try_lock_for(chrono::seconds(1))) {
        saldo = saldo + cantidad;   // sección crítica
        cout << "Ingreso de " << cantidad << " hecho. Saldo: " << saldo << "\n";
        mtx.unlock();               // ---- acaba
    } else {
        // Aquí NO tenemos el candado, así que no hay que soltarlo.
        cout << "Ingreso de " << cantidad << ": 1 s esperando y el candado sigue tomado\n";
        cout << "Sin try_lock_for este hilo se quedaria bloqueado para siempre\n";
    }
}

int main() {
    retirar(30);                     // camino del if: suelta el candado
    retirar(500);                    // camino del else: NO lo suelta
    thread cajero(ingresar, 50);     // hilo que ejecuta ingresar(50)
    cajero.join();                   // join: main espera a que el cajero termine

    // Solo para terminar limpio: el dueño del candado (main) lo suelta.
    // Destruir un mutex tomado es comportamiento indefinido. Si arreglas
    // retirar(), borra también este unlock (soltar un candado libre también
    // es comportamiento indefinido).
    mtx.unlock();
    cout << "Saldo final: " << saldo << "\n";
    return 0;   // 0 = el programa ha terminado bien
}

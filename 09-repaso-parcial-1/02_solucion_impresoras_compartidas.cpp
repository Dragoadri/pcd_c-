// =====================================================================
// Tema 9.2  |  Ejercicio tipo examen: impresoras compartidas (SOLUCIÓN)
// En este programa he corregido las impresoras: el semáforo limita a 3 los que imprimen y mtx_impresoras protege elegir y devolver cada impresora.
// Requisitos previos: 01_enunciado_impresoras_compartidas.cpp
// Qué observar al ejecutar: ya no aparece ninguna línea "ERROR"; los tres documentos de cada empleado salen.
// Salida determinista: NO (el orden de impresión cambia; nunca hay errores)
// Compilar con: cualquier nivel
// =====================================================================
// SOLUCIÓN. Los cambios respecto al enunciado van marcados con "CAMBIO".
// a) Interfoliación: E1 ve la impresora 0 libre y empieza a reservarla; antes de que
//    la marque, E2 también la ve libre. Las dos la marcan y las dos imprimen en la 0.
//    Comprobar y marcar tiene que ser atómico: es una sección crítica.
// b) Un mutex alrededor de "elegir" y alrededor de "devolver" la impresora.
// c) Sin semáforo, con las 3 impresoras ocupadas, un empleado se quedaría girando
//    en elegir_impresora() CON EL MUTEX COGIDO, y los que imprimen no podrían
//    devolver la suya porque necesitan ese mutex: interbloqueo. El semáforo va
//    PRIMERO y garantiza que, cuando entras a elegir, hay alguna libre.
#include <chrono>      // milliseconds, para indicar cuánto tiempo dormir
#include <iostream>    // cout
#include <mutex>       // mutex
#include <semaphore>   // counting_semaphore (C++20)
#include <string>      // string y to_string
#include <thread>      // thread y this_thread
#include <vector>      // vector

using namespace std;

const int IMPRESORAS = 3;
const int EMPLEADOS = 8;
const int DOCUMENTOS = 3;

counting_semaphore<IMPRESORAS> permisos(IMPRESORAS);   // máximo 3 permisos, y empieza con los 3 disponibles
vector<int> libre(IMPRESORAS, 1);        // 3 casillas, todas a 1 al empezar (1 = libre, 0 = ocupada)
mutex mtx_impresoras;                    // CAMBIO: protege el vector libre

// Código de medida (correcto, no hay que tocarlo)
mutex mtx_medida;                        // protege usuarios_en
vector<int> usuarios_en(IMPRESORAS, 0);  // cuántos empleados están usando cada impresora ahora mismo
mutex mtx_consola;                       // protege cout

// const string&: recibe el texto por referencia (sin copiarlo) y sin poder modificarlo.
void imprimir(const string& texto) {
    mtx_consola.lock();       // solo un hilo escribe a la vez
    cout << texto << "\n";
    mtx_consola.unlock();
}

// Recorre las impresoras en círculo hasta encontrar una libre.
int elegir_impresora() {
    int elegida = -1;                          // -1 = todavía no he encontrado ninguna
    int i = 0;
    mtx_impresoras.lock();                     // CAMBIO: empieza la sección crítica; comprobar y marcar, sin interrupciones
    while (elegida == -1) {                    // da vueltas hasta encontrar una (el semáforo asegura que hay alguna)
        if (libre[i] == 1) {                   // ¿está libre la impresora i?
            this_thread::sleep_for(chrono::milliseconds(1));   // reserva por red
            libre[i] = 0;                      // la marca como ocupada, sin que nadie se cuele entre el if y esto
            elegida = i;                       // se queda con ella: el while termina
        }
        i = (i + 1) % IMPRESORAS;              // siguiente impresora; % hace que después de la 2 vuelva a la 0
    }
    mtx_impresoras.unlock();                   // CAMBIO: acaba la sección crítica
    return elegida;
}

void empleado(int id) {
    for (int doc = 0; doc < DOCUMENTOS; doc++) {
        permisos.acquire();                     // PRIMERO el semáforo: si los 3 permisos están cogidos, espera dormido
        int impresora = elegir_impresora();     // DESPUÉS el mutex, dentro de elegir_impresora

        mtx_medida.lock();                      // medida: cuántos hay en esta impresora
        usuarios_en[impresora]++;
        if (usuarios_en[impresora] > 1) {       // más de un empleado a la vez en la misma impresora
            // to_string convierte el número en texto para poder unirlo con +
            imprimir("ERROR: impresora " + to_string(impresora) + " usada por 2 empleados a la vez");
        }
        mtx_medida.unlock();

        imprimir("Empleado " + to_string(id) + " imprime en la " + to_string(impresora));
        this_thread::sleep_for(chrono::milliseconds(20));   // imprime durante 20 ms

        mtx_medida.lock();
        usuarios_en[impresora]--;
        mtx_medida.unlock();

        mtx_impresoras.lock();                 // CAMBIO: devolver también es sección crítica
        libre[impresora] = 1;                  // la vuelve a marcar como libre
        mtx_impresoras.unlock();               // CAMBIO
        permisos.release();                    // devuelve el permiso para otro empleado
    }
}

int main() {
    vector<thread> oficina;       // vector de hilos: un hilo por empleado
    for (int i = 0; i < EMPLEADOS; i++) {
        // thread(empleado, i) crea el hilo (arranca ya) con id = i; push_back lo añade al final del vector
        oficina.push_back(thread(empleado, i));
    }
    for (int i = 0; i < EMPLEADOS; i++) {
        oficina[i].join();        // espero a que el empleado i termine
    }
    cout << "Fin de la jornada\n";
    return 0;   // 0 = el programa ha terminado bien
}

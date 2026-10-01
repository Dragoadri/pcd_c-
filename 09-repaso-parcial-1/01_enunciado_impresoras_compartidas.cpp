// =====================================================================
// Tema 9.1  |  Ejercicio tipo examen: impresoras compartidas (ENUNCIADO)
// En este programa he repartido 3 impresoras entre 8 empleados con un counting_semaphore de 3 permisos, y a veces dos empleados usan la misma impresora.
// Requisitos previos: 06-mutex, 07-semaforos/02_restaurante_con_counting_semaphore.cpp
// Qué observar al ejecutar: aparecen líneas "ERROR: impresora X usada por 2 empleados a la vez".
// Salida determinista: NO (qué impresora se pisa cambia; el error aparece en todas las ejecuciones)
// Compilar con: cualquier nivel
// =====================================================================
// ENUNCIADO
// Una oficina tiene 3 impresoras y 8 empleados que imprimen 3 documentos cada uno.
// El semáforo ya garantiza que nunca hay más de 3 empleados imprimiendo.
// Pero cada empleado tiene que ELEGIR una impresora libre, y dos pueden elegir la misma.
//   a) Ejecuta el programa y explica con una interfoliación por qué dos empleados
//      acaban en la misma impresora aunque el semáforo funciona.
//   b) Corrígelo con el mínimo cambio posible. No toques el semáforo ni el código de medida.
//   c) ¿Podrías quitar el semáforo y dejar solo el mutex? ¿Qué cambiaría?
// Reservar una impresora tarda 1 ms (va por red): ese sleep NO se quita.
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
    int elegida = -1;                 // -1 = todavía no he encontrado ninguna
    int i = 0;
    while (elegida == -1) {           // da vueltas hasta encontrar una
        if (libre[i] == 1) {          // ¿está libre la impresora i?
            this_thread::sleep_for(chrono::milliseconds(1));   // reserva por red
            libre[i] = 0;             // la marca como ocupada
            elegida = i;              // se queda con ella: el while termina
        }
        i = (i + 1) % IMPRESORAS;     // siguiente impresora; % hace que después de la 2 vuelva a la 0
    }
    return elegida;
}

void empleado(int id) {
    for (int doc = 0; doc < DOCUMENTOS; doc++) {
        permisos.acquire();                     // pide un permiso; si los 3 están cogidos, espera dormido
        int impresora = elegir_impresora();

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

        libre[impresora] = 1;                   // la vuelve a marcar como libre
        permisos.release();                     // devuelve el permiso para otro empleado
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

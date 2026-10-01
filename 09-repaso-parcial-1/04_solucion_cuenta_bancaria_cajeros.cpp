// =====================================================================
// Tema 9.4  |  Ejercicio tipo examen: cuenta bancaria con varios cajeros (SOLUCIÓN)
// En este programa he protegido saldo con mtx_saldo en cada ingreso y en cada retirada, comprobación incluida, y ahora el saldo cuadra siempre.
// Requisitos previos: 03_enunciado_cuenta_bancaria_cajeros.cpp
// Qué observar al ejecutar: "Diferencia: 0" en todas las ejecuciones.
// Salida determinista: SI (el saldo final siempre cuadra con lo ingresado y retirado)
// Compilar con: -O0 (mismo nivel que el enunciado, para comparar en igualdad)
// =====================================================================
// SOLUCIÓN. Los cambios respecto al enunciado van marcados con "CAMBIO".
// a) saldo += 10 y saldo -= 10 son leer, operar y escribir: dos cajeros leen el
//    mismo valor y uno de los dos cambios se pierde (condición de carrera).
// b) Mutex alrededor de cada operación sobre saldo.
// c) NO basta con proteger solo "saldo -= 10": entre el if (saldo >= 10) y la resta
//    otro cajero puede dejar el saldo a 0, y retiraríamos sin fondos. Comprobar y
//    modificar forman UNA sección crítica.
// total_ingresado y total_retirado no necesitan mutex: cada cajero tiene los suyos.
#include <functional>   // ref, para pasar variables por referencia a un hilo
#include <iostream>     // cout
#include <mutex>                                   // CAMBIO (mutex)
#include <thread>       // thread
#include <vector>       // vector

using namespace std;

const int CAJEROS = 4;
const int OPERACIONES = 1000000;
const int SALDO_INICIAL = 1000;

int saldo = SALDO_INICIAL;   // la cuenta: variable global, la misma para los 4 cajeros
mutex mtx_saldo;                              // CAMBIO (protege saldo)

// int&: los dos totales llegan por referencia, así lo que sume aquí queda guardado en los vectores de main.
void cajero(int& total_ingresado, int& total_retirado) {
    for (int i = 0; i < OPERACIONES; i++) {
        if (i % 4 != 3) {                         // 3 de cada 4: ingreso (i % 4 es el resto de dividir i entre 4)
            mtx_saldo.lock();                      // CAMBIO (empieza la sección crítica)
            saldo += 10;                           // leer, sumar y escribir, ahora sin que nadie se cuele
            mtx_saldo.unlock();                    // CAMBIO (acaba la sección crítica)
            total_ingresado += 10;                 // fuera del mutex: este total es solo de este cajero
        } else {                                  // la cuarta: retirada
            mtx_saldo.lock();                      // CAMBIO: antes de COMPROBAR
            if (saldo >= 10) {                     // la comprobación queda dentro de la sección crítica
                saldo -= 10;
                total_retirado += 10;
            }
            mtx_saldo.unlock();                    // CAMBIO: fuera del if, se suelta siempre
        }
    }
}

int main() {
    vector<int> ingresado(CAJEROS, 0);   // cada cajero escribe SOLO su casilla
    vector<int> retirado(CAJEROS, 0);
    vector<thread> cajeros;              // vector de hilos: un hilo por cajero
    for (int i = 0; i < CAJEROS; i++) {
        // ref(): el hilo recibe por referencia la casilla i de cada vector, no una copia
        cajeros.push_back(thread(cajero, ref(ingresado[i]), ref(retirado[i])));
    }
    for (int i = 0; i < CAJEROS; i++) {
        cajeros[i].join();               // espero a que el cajero i termine
    }

    // Saldo que debería quedar: el inicial más lo ingresado menos lo retirado por los 4 cajeros
    int esperado = SALDO_INICIAL;
    for (int i = 0; i < CAJEROS; i++) {
        esperado = esperado + ingresado[i] - retirado[i];
    }
    cout << "Saldo real:     " << saldo << "\n";
    cout << "Saldo esperado: " << esperado << "\n";
    cout << "Diferencia:     " << saldo - esperado << "\n";
    return 0;   // 0 = el programa ha terminado bien
}

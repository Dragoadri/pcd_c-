// =====================================================================
// Tema 9.3  |  Ejercicio tipo examen: cuenta bancaria con varios cajeros (ENUNCIADO)
// En este programa he puesto a 4 cajeros a ingresar y retirar dinero de la misma cuenta a la vez, y al final comparo el saldo real con el esperado.
// Requisitos previos: 04-hilos-usados-mal, 06-mutex
// Qué observar al ejecutar: "Saldo real" y "Saldo esperado" no coinciden.
// Salida determinista: NO (la diferencia cambia en cada ejecución; nunca es 0)
// Compilar con: -O0 (target sin optimizar; con -O2 la carrera se esconde)
// =====================================================================
// ENUNCIADO
// 4 cajeros automáticos trabajan sobre la misma cuenta. Cada uno hace 1 000 000 de
// operaciones: en tres de cada cuatro ingresa 10 euros; en la cuarta retira 10 si hay saldo.
// Cada cajero lleva la cuenta de lo que ha ingresado y retirado de verdad, así que
// al final se puede calcular el saldo que DEBERÍA quedar.
//   a) Ejecuta varias veces. ¿Por qué no cuadra? Señala las líneas del problema.
//   b) Corrige el programa con un mutex (lock y unlock a mano).
//   c) Un compañero propone proteger solo "saldo -= 10". ¿Es suficiente? Razónalo.
// Calibración (16 núcleos, GCC 13, -O0, 10 ejecuciones): la diferencia sale siempre
// entre -14 000 000 y -14 600 000 (se pierde casi la mitad de lo ingresado). Con mitad
// de ingresos y mitad de retiradas las pérdidas se compensaban y salían diferencias
// de decenas de euros, incluso cercanas a 0: el error quedaba escondido.
#include <functional>   // ref, para pasar variables por referencia a un hilo
#include <iostream>     // cout
#include <thread>       // thread
#include <vector>       // vector

using namespace std;

const int CAJEROS = 4;
const int OPERACIONES = 1000000;
const int SALDO_INICIAL = 1000;

int saldo = SALDO_INICIAL;   // la cuenta: variable global, la misma para los 4 cajeros

// int&: los dos totales llegan por referencia, así lo que sume aquí queda guardado en los vectores de main.
void cajero(int& total_ingresado, int& total_retirado) {
    for (int i = 0; i < OPERACIONES; i++) {
        if (i % 4 != 3) {                         // 3 de cada 4: ingreso (i % 4 es el resto de dividir i entre 4)
            saldo += 10;
            total_ingresado += 10;
        } else {                                  // la cuarta: retirada
            if (saldo >= 10) {                    // solo retira si hay saldo suficiente
                saldo -= 10;
                total_retirado += 10;
            }
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

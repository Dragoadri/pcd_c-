// =====================================================================
// Tema 4.4  |  Transferencias bancarias concurrentes sin proteger
// En este programa he puesto 4 hilos a mover dinero al azar entre 5 cuentas sin proteger el vector saldos, y el total del banco deja de cuadrar.
// Requisitos previos: 02_contador_compartido_roto, 01-repaso-cpp (vector y random)
// Qué observar al ejecutar: el total final no es 5000; se ha creado o destruido dinero.
// Salida determinista: NO (cuánto dinero aparece o desaparece cambia en cada ejecución)
// Compilar con: -O0 (target sin optimizar; en este ejemplo también falla a -O2)
// =====================================================================
// Cada transferencia resta en una cuenta y suma en otra: el total NUNCA
// debería cambiar. Si dos hilos tocan la misma cuenta a la vez, una de las
// dos escrituras se pierde y el total deja de cuadrar.
// Para simplificar no se comprueba si hay saldo: una cuenta puede quedar en negativo.
// Calibración (16 núcleos, GCC 13.3, -O0): 5 cuentas, 4 hilos x 10 000
// transferencias. Descuadre en 20 de 20 ejecuciones; totales observados
// entre 4 099 y 6 647 euros. Compilado a -O2 también descuadra en 10 de 10
// (entre 3 807 y 5 804): aquí el optimizador no puede fusionar el bucle.
// Con 200 000 transferencias el descuadre llega a decenas de miles, pero
// los números dejan de ser creíbles en clase.
#include <functional>   // ref
#include <iostream>     // cout
#include <random>       // random_device, mt19937, uniform_int_distribution
#include <thread>       // thread
#include <vector>       // vector

using namespace std;

const int NUM_CUENTAS = 5;
const int SALDO_INICIAL = 1000;
const int NUM_HILOS = 4;
const int TRANSFERENCIAS = 10000;

// vector<int>&: referencia; todos los hilos trabajan sobre el MISMO vector de saldos.
void hacer_transferencias(vector<int>& saldos) {
    random_device semilla;   // número de arranque impredecible que da el sistema
    // mt19937: generador de números aleatorios, arrancado con la semilla.
    mt19937 generador(semilla());                           // generador local del hilo
    uniform_int_distribution<int> cuenta_aleatoria(0, NUM_CUENTAS - 1);   // posición de cuenta: 0 a 4
    uniform_int_distribution<int> cantidad_aleatoria(1, 10);              // euros: 1 a 10

    for (int i = 0; i < TRANSFERENCIAS; i++) {
        int origen = cuenta_aleatoria(generador);   // cada llamada saca un número nuevo
        int destino = cuenta_aleatoria(generador);  // puede coincidir con origen: no pasa nada
        int cantidad = cantidad_aleatoria(generador);
        // Sin protección: entre la lectura y la escritura otro hilo puede cambiar la misma cuenta.
        saldos[origen] = saldos[origen] - cantidad;     // leer, restar, escribir
        saldos[destino] = saldos[destino] + cantidad;   // leer, sumar, escribir
    }
}

int main() {
    vector<int> saldos(NUM_CUENTAS, SALDO_INICIAL);   // 5 cuentas que empiezan con 1000 euros cada una
    vector<thread> hilos;
    for (int i = 0; i < NUM_HILOS; i++) {
        // ref(saldos): sin ref cada hilo trabajaría sobre su propia copia del vector.
        hilos.push_back(thread(hacer_transferencias, ref(saldos)));
    }
    for (int i = 0; i < NUM_HILOS; i++) {
        hilos[i].join();   // espero a todos antes de contar el dinero
    }

    int total = 0;
    for (int i = 0; i < NUM_CUENTAS; i++) {
        cout << "Cuenta " << i << ": " << saldos[i] << " euros\n";
        total = total + saldos[i];
    }
    cout << "Dinero total: " << total << " (deberia ser " << NUM_CUENTAS * SALDO_INICIAL << ")\n";
    return 0;   // 0 = el programa ha terminado bien
}

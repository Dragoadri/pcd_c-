// =====================================================================
// Tema 1.15  |  Carrera de tortugas lineal (sin hilos) y bucle con bandera
// En este programa he hecho avanzar por turnos 3 tortugas hasta la META de 100 metros, parando los dos while con la bandera hay_ganadora.
// Requisitos previos: 13_mismo_ejercicio_con_while.cpp, 14_aleatoriedad_y_tiempo.cpp
// Qué observar al ejecutar: las tortugas mueven SIEMPRE en orden 1, 2, 3, nunca a la vez.
// Salida determinista: NO (avances aleatorios; el orden de los turnos sí es siempre el mismo)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Material original: "Ejemplo Python lineal" del Tema 1 presencial.
// NORMA: sin break. Para salir de un bucle en cuanto hay ganadora se mete
// una bandera (hay_ganadora) en la condición. Sin struct: vector de distancias.
#include <chrono>     // milliseconds
#include <iostream>   // cout
#include <random>     // random_device, mt19937 y uniform_int_distribution
#include <thread>     // this_thread::sleep_for
#include <vector>     // vector

using namespace std;

// Constantes globales: fuera de main, se pueden usar en todo el fichero.
const int NUM_TORTUGAS = 3;
const int META = 100;

int main() {
    random_device semilla;                        // semilla impredecible del sistema
    mt19937 generador(semilla());                 // generador arrancado con esa semilla
    uniform_int_distribution<int> avance(1, 5);   // cada avance será de 1 a 5 metros

    // Una casilla por tortuga, todas a 0: distancias[0] es la tortuga 1, distancias[2] la tortuga 3.
    vector<int> distancias(NUM_TORTUGAS, 0);
    bool hay_ganadora = false;  // la bandera
    int ganadora = -1;          // -1 = todavía no hay ganadora (ninguna tortuga es la -1)

    // Bucle de fuera: una vuelta = una ronda en la que mueve cada tortuga.
    while (!hay_ganadora) {
        int t = 0;   // índice de la tortuga que mueve: 0, 1, 2
        // Con break sería: for (...) { ...; if (distancias[t] >= META) break; }
        // Bucle de dentro: sigue mientras queden tortugas en la ronda Y nadie haya ganado.
        while (t < NUM_TORTUGAS && !hay_ganadora) {
            distancias[t] = distancias[t] + avance(generador);
            // t + 1 porque las posiciones empiezan en 0 pero las tortugas se numeran desde 1.
            cout << "Tortuga " << t + 1 << " ha recorrido " << distancias[t] << " metros\n";
            if (distancias[t] >= META) {
                hay_ganadora = true;  // en la próxima comprobación ambos bucles terminan
                ganadora = t + 1;
            }
            t++;
        }
        // Pausa de 10 ms entre rondas para que la carrera se vea avanzar.
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    cout << "La tortuga " << ganadora << " ha llegado a la meta!\n";
    // Pregunta para el tema 03: ¿y si cada tortuga corriera por su cuenta, a la vez?
    return 0;   // 0 = el programa ha terminado bien
}

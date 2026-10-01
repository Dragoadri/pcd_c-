// =====================================================================
// Tema 16.3  |  Maestro: reparte un cálculo entre procesos trabajadores conectados por red (AMPLIACIÓN)
// En este programa he hecho un maestro que acepta hasta 3 trabajadores, les reparte rangos con CONTAR_PRIMOS y suma los primos que devuelve cada uno.
// Requisitos previos: 01_servidor_rpc_calculadora, 15-ampliacion-paralelismo-real (01_suma_paralela_speedup)
// Qué observar al ejecutar: qué rango recibe cada trabajador, cuánto devuelve cada uno y que el total es 216816 primos.
// Salida determinista: SI en el resultado (216816); NO en el tiempo ni en el orden en que se conectan los trabajadores
// Compilar con: cualquier nivel (solo Linux)
// =====================================================================
// CÓMO LANZARLO (cuatro terminales, desde build/bin):
//   Terminal 1:        ./03_maestro_reparte_trabajo
//   Terminales 2 a 4:  ./04_trabajador_remoto
// O en una sola:  ./03_maestro_reparte_trabajo & for i in 1 2 3; do ./04_trabajador_remoto & done; wait
// En el aula, los trabajadores pueden estar en los ordenadores de los alumnos
// cambiando IP_MAESTRO en 04 por la IP del ordenador del profesor.
//
// El maestro espera 20 s a que se conecten hasta 3 trabajadores y reparte
// entre los que haya. Si no llega ninguno, termina sin calcular.
#include <iostream>               // cout
#include <string>                 // string, to_string, stoi
#include <vector>                 // vector
#include <chrono>                 // steady_clock, duration
#include "red_utilidades.hpp"     // red_crear_servidor, red_enviar_linea, red_recibir_linea... (y los includes de sockets)

using namespace std;

const int PUERTO = 5603;
const int NUM_TRABAJADORES = 3;
const int LIMITE = 3000000;        // contar primos en [0, LIMITE): son 216816

int main() {
    int servidor = red_crear_servidor(PUERTO, 20);   // accept dará -1 tras 20 s sin conexiones nuevas
    vector<int> trabajadores;                        // un socket (un int) por cada trabajador conectado
    bool esperando = (servidor >= 0);                // si el puerto no se abrió, no se espera a nadie
    cout << "Maestro en el puerto " << PUERTO << ", esperando trabajadores...\n" << flush;   // flush: que se vea ya

    // 1. Reunir trabajadores (hasta NUM_TRABAJADORES o hasta que venza la espera).
    // Cast (int): size() devuelve un número sin signo y lo comparamos con un int.
    while (esperando && (int) trabajadores.size() < NUM_TRABAJADORES) {
        // accept: espera a un trabajador y devuelve un socket solo para él. nullptr: no nos interesa su dirección.
        int trabajador = accept(servidor, nullptr, nullptr);
        if (trabajador < 0) {                        // -1: pasaron 20 s sin que llegara nadie más
            esperando = false;
        } else {
            red_poner_espera_maxima(trabajador, 60); // si un trabajador tarda más de 60 s en contestar, no le esperamos
            trabajadores.push_back(trabajador);      // push_back: añade el socket al final del vector
            cout << "Trabajador conectado (" << trabajadores.size() << " de " << NUM_TRABAJADORES << ")\n" << flush;
        }
    }

    int cuantos = (int) trabajadores.size();
    if (cuantos == 0) {
        cout << "No se ha conectado ningun trabajador: no hay reparto\n";
    } else {
        chrono::steady_clock::time_point inicio = chrono::steady_clock::now();   // instante en que empieza el reparto

        // 2. Enviar TODOS los encargos antes de leer ninguna respuesta: así trabajan a la vez.
        int tamanio = LIMITE / cuantos;              // división entera: el resto se lo lleva el último
        for (int i = 0; i < cuantos; i++) {
            int desde = i * tamanio;
            // Operador ternario (condición ? si_cierto : si_falso): el último trabajador llega hasta LIMITE.
            int hasta = (i == cuantos - 1) ? LIMITE : desde + tamanio;
            red_enviar_linea(trabajadores[i], "CONTAR_PRIMOS " + to_string(desde) + " " + to_string(hasta));
            cout << "  encargo al trabajador " << i << ": [" << desde << ", " << hasta << ")\n";
        }

        // 3. Recoger resultados. Cada respuesta es "RESULTADO n".
        long long total = 0;
        for (int i = 0; i < cuantos; i++) {
            string respuesta;
            // Se lee la línea Y además tiene que empezar por "RESULTADO " (eso comprueba rfind(..., 0) == 0).
            if (red_recibir_linea(trabajadores[i], respuesta) && respuesta.rfind("RESULTADO ", 0) == 0) {
                int parcial = stoi(respuesta.substr(10));   // substr(10): salta los 10 caracteres de "RESULTADO "; stoi: texto a int
                cout << "  trabajador " << i << " devuelve " << parcial << "\n";
                total += parcial;
            } else {
                cout << "  trabajador " << i << " no ha respondido: su parte se pierde\n";
            }
            red_enviar_linea(trabajadores[i], "FIN");   // le dice al trabajador que ya puede terminar
            close(trabajadores[i]);                     // cierra la conexión con ese trabajador
        }
        // Restar dos instantes da una duración en segundos; count() da ese número.
        chrono::duration<double> segundos = chrono::steady_clock::now() - inicio;
        cout << "Primos menores que " << LIMITE << ": " << total << " (en " << segundos.count() << " s)\n";
    }
    if (servidor >= 0) {
        close(servidor);                             // cierra el socket de escucha
    }
    return 0;   // 0 = el programa ha terminado bien
}

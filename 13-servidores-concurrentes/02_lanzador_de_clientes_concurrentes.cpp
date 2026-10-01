// =====================================================================
// Tema 13.2  |  Lanzador de clientes concurrentes (herramienta de pruebas)
// En este programa he lanzado N hilos cliente que un latch hace salir a la vez contra un servidor, y he medido cuántos segundos espera cada uno.
// Requisitos previos: 01_servidor_un_hilo_por_conexion.cpp, 08-barreras (latch)
// Qué observar al ejecutar: la columna de segundos: contra el secuencial (5202) crece 2, 4, 6...; contra el concurrente (5300) todos rondan 2.
// Salida determinista: NO (tiempos y orden de atención dependen del servidor y de la carga)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: arranca antes un servidor y ejecuta este programa. Pregunta puerto y número
// de clientes; Enter (o stdin vacío) usa los valores por defecto. Desde terminal sin preguntas:
//   printf "5202\n5\n" | ./build/bin/02_lanzador_de_clientes_concurrentes
// PUERTOS: 5202 = 12/05 secuencial | 5300 = 13/01 un hilo por conexion
//          5301 = 13/03 visitas sin mutex | 5302 = 13/04 visitas con mutex | 5303 = 13/05 semaforo
#include <iostream>        // cout, cin
#include <string>          // string, to_string
#include <vector>          // vector
#include <thread>          // thread
#include <latch>           // latch: todos esperan hasta que alguien da la salida
#include <chrono>          // steady_clock, duration
#include <functional>      // ref
#include "red_comun.hpp"   // fichero nuestro: conectar_con_servidor, enviar_todo, recibir_linea

using namespace std;

const char* IP_SERVIDOR = "127.0.0.1";   // localhost: el servidor está en este mismo equipo
const int PUERTO_POR_DEFECTO = 5300;
const int CLIENTES_POR_DEFECTO = 5;

// Cada hilo escribe SOLO en su posición 'numero' de los vectores: no comparten
// ninguna variable escrita, así que no hace falta candado (condiciones de Bernstein).
// latch& y vector<...>& llevan &: el hilo trabaja con los objetos de main, no con copias.
void cliente(int puerto, int numero, latch& salida,
             vector<double>& segundos, vector<string>& respuestas) {
    salida.wait();   // todos esperan aquí y arrancan a la vez
    // steady_clock::now(): el instante actual, en un reloj que nunca va hacia atrás
    chrono::steady_clock::time_point inicio = chrono::steady_clock::now();

    // (red_comun.hpp) socket + connect con recv limitado a 30 s. Devuelve el socket o -1
    int sock = conectar_con_servidor(IP_SERVIDOR, puerto, 30);
    if (sock == -1) {
        respuestas[numero] = "NO CONECTA (esta arrancado el servidor?)";
    } else {
        string respuesta;
        enviar_todo(sock, "cliente_" + to_string(numero) + "\n");   // primera línea: mi nombre
        if (recibir_linea(sock, respuesta)) {   // espera la línea de respuesta del servidor
            respuestas[numero] = respuesta;
        } else {
            respuestas[numero] = "SIN RESPUESTA";
        }
        close(sock);   // colgar la conexión
    }
    // Restar dos instantes da una duración en segundos con decimales; count() da ese número
    chrono::duration<double> t = chrono::steady_clock::now() - inicio;
    segundos[numero] = t.count();
}

int main() {
    int puerto = PUERTO_POR_DEFECTO;
    int num_clientes = CLIENTES_POR_DEFECTO;
    cout << "Puerto del servidor [" << PUERTO_POR_DEFECTO << "]: ";
    // cin >> puerto es "falso" si no se pudo leer un número; el ! lo niega para entrar al if
    if (!(cin >> puerto)) {
        puerto = PUERTO_POR_DEFECTO;      // Enter vacío, texto o stdin cerrado
    }
    cout << "Numero de clientes [" << CLIENTES_POR_DEFECTO << "]: ";
    // Se usa el valor por defecto si no se pudo leer O si el número es menor que 1
    if (!(cin >> num_clientes) || num_clientes < 1) {
        num_clientes = CLIENTES_POR_DEFECTO;
    }
    cout << "\nLanzando " << num_clientes << " clientes a la vez contra el puerto " << puerto << "\n";

    vector<double> segundos(num_clientes, 0.0);   // num_clientes casillas, todas a 0.0
    vector<string> respuestas(num_clientes);      // num_clientes textos vacíos
    latch salida(1);   // latch con contador 1: los wait esperan hasta que count_down lo deje en 0
    vector<thread> hilos;
    for (int i = 0; i < num_clientes; i++) {
        // ref(...): pasa el objeto de main por referencia al hilo (sin ref, thread haría una copia)
        // push_back: añade el hilo recién creado al final del vector
        hilos.push_back(thread(cliente, puerto, i, ref(salida), ref(segundos), ref(respuestas)));
    }
    salida.count_down();   // pistoletazo de salida
    for (int i = 0; i < num_clientes; i++) {
        hilos[i].join();   // join: esperar a que el hilo i termine antes de leer sus resultados
    }

    double peor = 0.0;
    for (int i = 0; i < num_clientes; i++) {
        cout << "cliente_" << i << "  " << segundos[i] << " s  " << respuestas[i] << "\n";
        if (segundos[i] > peor) {
            peor = segundos[i];
        }
    }
    cout << "El cliente que mas espero tardo " << peor << " s\n";
    return 0;   // 0 = el programa ha terminado bien
}

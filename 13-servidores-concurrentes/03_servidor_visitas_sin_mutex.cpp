// =====================================================================
// Tema 13.3  |  Contador de visitas compartido SIN candado
// En este programa he dejado que los hilos lean, esperen 1 ms y escriban el contador visitas sin candado, para ver la condición de carrera en un servidor.
// Requisitos previos: 01_servidor_un_hilo_por_conexion.cpp, 02_lanzador_de_clientes_concurrentes.cpp, 04-hilos-usados-mal
// Qué observar al ejecutar: "visitas contadas" muy por debajo de "conexiones aceptadas", y líneas del libro sin firma.
// Salida determinista: NO (cuántas visitas se pierden depende del entrelazado de los hilos)
// Compilar con: -O0 (target sin optimizar; aquí la carrera la provoca la pausa, pero se mantiene el criterio de la carpeta 04)
// =====================================================================
// CÓMO LANZARLO: terminal 1 -> ./build/bin/03_servidor_visitas_sin_mutex
//                terminal 2 -> printf "5301\n50\n" | ./build/bin/02_lanzador_de_clientes_concurrentes
// Medido en el equipo de desarrollo con 50 clientes a la vez (5 ejecuciones):
// visitas contadas = 1, 1, 1, 1 y 2 de 50 aceptadas. Casi todos los hilos leen el 0 a la vez.
// SIN la pausa de 1 ms: 50 de 50 en 5 pruebas. La carrera sigue ahí, pero la ventana es tan
// pequeña que no se ve; la pausa simula una consulta a disco o a una base de datos.
#include <iostream>        // cout
#include <string>          // string, to_string
#include <vector>          // vector
#include <thread>          // thread, this_thread::sleep_for
#include <chrono>          // milliseconds
#include "red_comun.hpp"   // fichero nuestro: abrir_escucha, configurar_timeout, enviar_todo, recibir_linea

using namespace std;

const char* IP_ESCUCHA = "127.0.0.1";   // localhost: solo clientes de este mismo equipo
const int PUERTO = 5301;
const int MAX_CONEXIONES = 50;

// ---- Estado COMPARTIDO por todos los hilos ----
int visitas = 0;                                       // cuántas visitas llevamos
// vector<int> con MAX_CONEXIONES casillas, todas a 0 (0 = línea sin firmar)
vector<int> libro_de_visitas(MAX_CONEXIONES, 0);  // posición i = número de conexión de la visita i+1

void atender_cliente(int conexion, int numero_conexion) {
    configurar_timeout(conexion, 30);   // recv no espera más de 30 s
    string nombre;
    if (recibir_linea(conexion, nombre)) {   // lee la primera línea del cliente y la deja en nombre
        // ---- SECCIÓN CRÍTICA SIN PROTEGER ----
        int leido = visitas;                                        // 1) leer
        this_thread::sleep_for(chrono::milliseconds(1));  // 2) trabajar (consultar un fichero, una BD...)
        // Mientras este hilo dormía, otros han leído el MISMO valor: todos escriben leido + 1 y se pierden visitas
        visitas = leido + 1;                                        // 3) escribir
        if (leido < MAX_CONEXIONES) {                               // no salirse del vector
            libro_de_visitas[leido] = numero_conexion;              // varios hilos firman en la MISMA línea
        }
        // ---------------------------------------
        enviar_todo(conexion, "HOLA " + nombre + ", eres la visita " + to_string(leido + 1) + "\n");
    }
    close(conexion);   // colgar a este cliente
}

int main() {
    // (red_comun.hpp) socket + bind + listen, con accept limitado a 30 s. -1 si falla
    int escucha = abrir_escucha(IP_ESCUCHA, PUERTO, 30);
    if (escucha == -1) {
        return 1;
    }
    cout << "Servidor de visitas SIN mutex en el puerto " << PUERTO << "\n";

    vector<thread> hilos;   // un hilo por conexión aceptada
    bool seguir = true;
    // (int) convierte size(), que no tiene signo, para compararlo con MAX_CONEXIONES
    while (seguir && (int) hilos.size() < MAX_CONEXIONES) {
        // accept: espera a un cliente y devuelve un socket nuevo para hablar con él
        int conexion = accept(escucha, nullptr, nullptr);
        if (conexion == -1) {
            seguir = false;                      // 30 s sin conexiones nuevas
        } else {
            // Hilo nuevo que ejecuta atender_cliente(conexion, número de conexión); push_back lo guarda en el vector
            hilos.push_back(thread(atender_cliente, conexion, (int) hilos.size() + 1));
        }
    }
    for (size_t i = 0; i < hilos.size(); i++) {
        hilos[i].join();   // esperar a que termine cada hilo antes de leer los resultados
    }

    // Solo main cuenta las conexiones aceptadas: ese número sí es fiable
    int sin_firma = 0;
    for (size_t i = 0; i < hilos.size(); i++) {
        if (libro_de_visitas[i] == 0) {   // nadie escribió en esta línea
            sin_firma++;
        }
    }
    cout << "Conexiones aceptadas: " << hilos.size() << "\n";
    cout << "Visitas contadas:     " << visitas << "\n";
    cout << "Lineas del libro sin firma: " << sin_firma << "\n";
    close(escucha);   // liberar el puerto
    return 0;         // 0 = el programa ha terminado bien
}

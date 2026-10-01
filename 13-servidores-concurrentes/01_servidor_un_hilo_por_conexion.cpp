// =====================================================================
// Tema 13.1  |  Servidor concurrente: un hilo por conexión
// En este programa he lanzado un thread con atender_cliente por cada accept para atender a varios clientes a la vez, y al final los he unido con join.
// Requisitos previos: 12-sockets-tcp (05_servidor_tcp_secuencial.cpp), 03-hilos-creacion, 06-mutex
// Qué observar al ejecutar: con 5 clientes a la vez, todos terminan hacia t=2s (el secuencial de la carpeta 12 tardaba 10 s en el último).
// Salida determinista: NO (el orden de llegada y de los mensajes de cada hilo cambia en cada ejecución)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: terminal 1 -> ./build/bin/01_servidor_un_hilo_por_conexion
//                terminal 2 -> ./build/bin/02_lanzador_de_clientes_concurrentes   (Enter, Enter)
// Termina tras MAX_CLIENTES conexiones o 30 s sin conexiones nuevas, y SIEMPRE une sus hilos.
// Las funciones de red (abrir_escucha, enviar_todo...) están en red_comun.hpp: son las de la carpeta 12.
#include <iostream>        // cout
#include <string>          // string, to_string
#include <vector>          // vector
#include <thread>          // thread, this_thread::sleep_for
#include <mutex>           // mutex
#include <chrono>          // steady_clock, duration, seconds
#include "red_comun.hpp"   // entre comillas: fichero nuestro de esta carpeta (abrir_escucha, enviar_todo, recibir_linea...)

using namespace std;

const char* IP_ESCUCHA = "127.0.0.1";   // localhost: solo clientes de este mismo equipo
const int PUERTO = 5300;
const int MAX_CLIENTES = 5;
// steady_clock::now(): instante en que arranca el programa, para imprimir tiempos relativos a él
const chrono::steady_clock::time_point INICIO = chrono::steady_clock::now();

mutex mtx_consola;   // candado para que las líneas de dos hilos no se mezclen en pantalla
void imprimir(const string& texto) {   // const string&: el texto sin copiarlo y sin poder cambiarlo
    mtx_consola.lock();     // ---- empieza la sección crítica: solo un hilo escribe a la vez ----
    // Restar dos instantes da una duración; duration<double> la guarda en segundos con decimales
    chrono::duration<double> t = chrono::steady_clock::now() - INICIO;
    // count() da los segundos; el cast (int) quita los decimales (2.37 -> 2)
    cout << "t=" << (int) t.count() << "s " << texto;
    mtx_consola.unlock();   // ---- termina la sección crítica ----
}

// Lo que el servidor secuencial hacía en main con cada cliente, ahora lo hace un hilo.
// 'conexion' llega POR VALOR: cada hilo tiene su propia copia del descriptor.
void atender_cliente(int conexion, int numero) {
    configurar_timeout(conexion, 30);   // (red_comun.hpp) recv no espera más de 30 s
    string nombre;
    if (recibir_linea(conexion, nombre)) {   // (red_comun.hpp) lee la primera línea y la deja en nombre
        imprimir("hilo " + to_string(numero) + " atiende a " + nombre + "\n");   // to_string: número a texto
        this_thread::sleep_for(chrono::seconds(2));   // simula el trabajo
        enviar_todo(conexion, "HOLA " + nombre + ", te ha atendido el hilo " + to_string(numero) + "\n");
        imprimir("hilo " + to_string(numero) + " termina con " + nombre + "\n");
    }
    close(conexion);   // cada hilo cierra SU conexión
}

int main() {
    // (red_comun.hpp) socket + bind + listen en 127.0.0.1:5300, con accept limitado a 30 s. -1 si falla
    int escucha = abrir_escucha(IP_ESCUCHA, PUERTO, 30);
    if (escucha == -1) {
        return 1;   // 1 = terminar con error
    }
    imprimir("Servidor CONCURRENTE (un hilo por conexion) en el puerto " + to_string(PUERTO) + "\n");

    vector<thread> hilos;   // vector<thread>: lista que crece, con un hilo por cliente aceptado
    bool seguir = true;     // bandera: a false cuando pasan 30 s sin conexiones nuevas
    // hilos.size() devuelve un entero sin signo; (int) lo convierte para compararlo con MAX_CLIENTES
    while (seguir && (int) hilos.size() < MAX_CLIENTES) {
        // accept: espera a un cliente y devuelve un socket nuevo para hablar con él (nullptr = no quiero su IP ni puerto)
        int conexion = accept(escucha, nullptr, nullptr);
        if (conexion == -1) {
            imprimir("30 s sin conexiones nuevas: dejo de aceptar\n");
            seguir = false;
        } else {
            int numero = hilos.size() + 1;
            imprimir("conexion " + to_string(numero) + " aceptada, lanzo su hilo\n");
            // NO usar ref(conexion): la variable cambia en la siguiente vuelta del bucle
            // thread(atender_cliente, conexion, numero): hilo nuevo que ejecuta atender_cliente(conexion, numero).
            // push_back lo guarda al final del vector. main vuelve enseguida a accept sin esperar al hilo.
            hilos.push_back(thread(atender_cliente, conexion, numero));
        }
    }

    for (size_t i = 0; i < hilos.size(); i++) {
        hilos[i].join();   // join: main espera a que termine el hilo i. Nunca se sale sin unir a todos los hilos
    }
    imprimir("Todos los hilos unidos. Fin del servidor\n");
    close(escucha);   // liberar el puerto
    return 0;         // 0 = el programa ha terminado bien
}

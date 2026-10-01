// =====================================================================
// Tema 13.7  |  Cliente de chat: un hilo escucha y otro habla
// En este programa he usado dos hilos sobre el mismo socket: receptor imprime lo que llega con recibir_mensajes y main envía los mensajes y SALIR.
// Requisitos previos: 06_servidor_chat_difusion.cpp
// Qué observar al ejecutar: los mensajes recibidos aparecen intercalados con los propios, sin esperar a que este cliente termine de enviar.
// Salida determinista: NO (depende de cuándo hablen los demás clientes)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: con 06_servidor_chat_difusion arrancado, varios seguidos (en terminales distintos
// o con & en uno solo). Cada uno envía MENSAJES predefinidos cada 2 s, luego SALIR, y termina solo.
// recv y send a la vez sobre el mismo socket desde dos hilos es correcto: TCP va en los dos sentidos.
#include <iostream>        // cout
#include <string>          // string, to_string
#include <thread>          // thread, this_thread::sleep_for
#include <mutex>           // mutex
#include <chrono>          // seconds
#include "red_comun.hpp"   // fichero nuestro: conectar_con_servidor, enviar_todo, recibir_linea

using namespace std;

const char* IP_SERVIDOR = "127.0.0.1";   // localhost: el servidor está en este mismo equipo
const int PUERTO = 5304;                 // el puerto de 06_servidor_chat_difusion
const int NUM_MENSAJES = 3;
const string MENSAJES[NUM_MENSAJES] = {"hola a todos", "alguien por aqui?", "me voy, adios"};   // array fijo de 3 textos

mutex mtx_consola;   // los dos hilos imprimen: el candado evita que se mezclen sus líneas
void imprimir(const string& texto) {   // const string&: el texto sin copiarlo y sin poder cambiarlo
    mtx_consola.lock();     // empieza la sección crítica
    cout << texto;
    mtx_consola.unlock();   // termina la sección crítica
}

// Hilo receptor: imprime todo lo que llegue hasta que el servidor cierre la conexión
void recibir_mensajes(int sock, const string& yo) {
    string linea;
    // recibir_linea devuelve false cuando el servidor cierra, hay error o pasan 15 s sin nada: ahí acaba el bucle
    while (recibir_linea(sock, linea)) {
        imprimir("   (" + yo + " recibe) " + linea + "\n");
    }
    imprimir("   (" + yo + ") conexion cerrada por el servidor\n");
}

int main() {
    // getpid(): número de proceso que el sistema da a este programa; to_string lo pasa a texto
    string nombre = "usuario_" + to_string(getpid());   // el PID distingue a cada cliente

    // 15 s de timeout: si el servidor desaparece sin cerrar, el receptor no espera para siempre
    // (red_comun.hpp) socket + connect. Devuelve el socket conectado o -1
    int sock = conectar_con_servidor(IP_SERVIDOR, PUERTO, 15);
    if (sock == -1) {
        perror("connect");   // imprime el motivo, p. ej. "Connection refused" si no hay servidor
        cout << "Arranca primero 06_servidor_chat_difusion\n";
        return 1;
    }

    enviar_todo(sock, nombre + "\n");                       // primera línea: quién soy
    // Hilo nuevo que ejecuta recibir_mensajes(sock, nombre). Arranca ya, en paralelo con main
    thread receptor(recibir_mensajes, sock, nombre);   // desde aquí, recibir va aparte

    // Hilo principal: hablar
    for (int i = 0; i < NUM_MENSAJES; i++) {
        this_thread::sleep_for(chrono::seconds(2));   // dormir 2 s entre mensaje y mensaje
        imprimir(nombre + " envia: " + MENSAJES[i] + "\n");
        enviar_todo(sock, MENSAJES[i] + "\n");
    }
    enviar_todo(sock, "SALIR\n");   // el servidor nos quita de la lista y cierra: el receptor termina

    receptor.join();   // join: esperar a que el hilo receptor acabe antes de cerrar el socket que usa
    close(sock);       // colgar la conexión
    return 0;          // 0 = el programa ha terminado bien
}

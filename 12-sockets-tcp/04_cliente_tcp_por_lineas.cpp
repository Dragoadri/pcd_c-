// =====================================================================
// Tema 12.4  |  Cliente que trocea sus envíos a propósito
// En este programa he enviado las líneas uno, dos, tres y FIN en 4 send con cortes raros, para ver que el servidor las reconstruye bien.
// Requisitos previos: 03_servidor_tcp_por_lineas.cpp
// Qué observar al ejecutar: se hacen 4 send con cortes raros, pero las respuestas llegan como UNO, DOS, TRES.
// Salida determinista: SI en localhost
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: primero 03_servidor_tcp_por_lineas en otro terminal.
#include <iostream>       // cout
#include <string>         // string
#include <thread>         // this_thread::sleep_for
#include <chrono>         // milliseconds
#include <cstdio>         // perror
#include <sys/socket.h>   // socket, connect, send, recv, setsockopt
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // htons, inet_pton
#include <sys/time.h>     // timeval
#include <unistd.h>       // close

using namespace std;

const char* IP_SERVIDOR = "127.0.0.1";   // localhost: el servidor está en este mismo equipo
const int PUERTO = 5201;                 // el puerto de 03_servidor_tcp_por_lineas

// send puede enviar MENOS bytes de los pedidos (si el búfer del sistema está lleno).
// enviar_todo repite hasta que sale todo. Devuelve false si la conexión falla.
// int fd: el número del socket. const string& texto: el texto sin copiarlo (&) y sin poder cambiarlo (const)
bool enviar_todo(int fd, const string& texto) {
    size_t enviados = 0;   // cuántos bytes han salido ya (size_t: entero sin signo para tamaños)
    bool ok = true;
    while (ok && enviados < texto.size()) {   // seguir mientras no haya error y quede algo por enviar
        // texto.c_str() + enviados: puntero al primer byte que AÚN no ha salido.
        // texto.size() - enviados: cuántos bytes quedan. MSG_NOSIGNAL: si el otro cerró, -1 en vez de morir.
        ssize_t n = send(fd, texto.c_str() + enviados, texto.size() - enviados, MSG_NOSIGNAL);
        if (n <= 0) {
            ok = false;   // error: la conexión se ha roto
        } else {
            enviados += n;   // han salido n bytes más
        }
    }
    return ok;
}

int main() {
    // socket TCP: AF_INET = IPv4, SOCK_STREAM = flujo de bytes. Devuelve su número o -1
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    timeval espera{};   // tiempo en segundos (tv_sec) y microsegundos (tv_usec)
    espera.tv_sec = 5;
    // SO_RCVTIMEO: recv esperará como mucho 5 s
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));
    sockaddr_in servidor{};                                // dirección del servidor
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);                     // htons: puerto en orden de red
    inet_pton(AF_INET, IP_SERVIDOR, &servidor.sin_addr);   // IP de texto a 4 bytes
    // Con ||, si socket falló (-1) ya no se llama a connect.
    // connect: "llamar" al servidor; (sockaddr*) &servidor es la dirección vista como el tipo genérico que pide.
    if (sock == -1 || connect(sock, (sockaddr*) &servidor, sizeof(servidor)) == -1) {
        perror("connect");   // sin servidor: "Connection refused"
        cout << "Arranca primero 03_servidor_tcp_por_lineas\n";
        return 1;
    }

    // Tres send seguidos: el servidor los recibirá pegados en un solo recv
    enviar_todo(sock, "uno\n");
    enviar_todo(sock, "dos\n");
    enviar_todo(sock, "tr");            // la palabra "tres" partida en dos envíos
    this_thread::sleep_for(chrono::milliseconds(800));   // pausa de 800 ms: así "tr" y "es" llegan en recv distintos
    enviar_todo(sock, "es\nFIN\n");     // el final de "tres" y la orden FIN, juntos

    // Leer todo lo que conteste hasta que el servidor cierre
    bool seguir = true;
    while (seguir) {
        char buffer[1024];
        // recv: devuelve cuántos bytes llegaron, 0 si el servidor cerró, -1 si error o timeout
        ssize_t n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            seguir = false;
        } else {
            cout << "Respuesta recibida:\n" << string(buffer, n);   // string(buffer, n): los n bytes como texto
        }
    }
    close(sock);   // colgar la conexión
    return 0;      // 0 = el programa ha terminado bien
}

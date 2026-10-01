// =====================================================================
// Tema 12.2  |  Cliente TCP de eco
// En este programa he conectado con el servidor TCP usando connect y le he enviado tres mensajes con send, leyendo cada eco con recv.
// Requisitos previos: 01_servidor_tcp_eco.cpp
// Qué observar al ejecutar: con servidor, el eco de cada mensaje; sin servidor, "Connection refused" inmediato (en UDP no había error).
// Salida determinista: SI
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: primero 01_servidor_tcp_eco en otro terminal. Pruébalo también SIN servidor.
#include <iostream>       // cout
#include <string>         // string
#include <cstdio>         // perror
#include <sys/socket.h>   // socket, connect, send, recv, setsockopt
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // htons, inet_pton
#include <sys/time.h>     // timeval
#include <unistd.h>       // close

using namespace std;

const char* IP_SERVIDOR = "127.0.0.1";   // localhost: el servidor está en este mismo equipo
const int PUERTO = 5200;                 // el puerto donde escucha 01_servidor_tcp_eco

int main() {
    // socket TCP: AF_INET = IPv4, SOCK_STREAM = flujo de bytes. Devuelve su número o -1 si falla
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");   // imprime el motivo del error
        return 1;
    }
    timeval espera{};   // tiempo en segundos (tv_sec) y microsegundos (tv_usec)
    espera.tv_sec = 5;
    // SO_RCVTIMEO: recv no esperará más de 5 s la respuesta
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));

    sockaddr_in servidor{};   // dirección del servidor: familia, puerto e IP
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);                     // htons: puerto en orden de red
    inet_pton(AF_INET, IP_SERVIDOR, &servidor.sin_addr);   // IP de texto a 4 bytes

    // connect: "marcar el número". Si nadie escucha en ese puerto, falla YA.
    // (sockaddr*) &servidor: nuestra sockaddr_in vista como el tipo genérico que pide connect
    if (connect(sock, (sockaddr*) &servidor, sizeof(servidor)) == -1) {
        perror("connect");   // sin servidor: "connect: Connection refused"
        cout << "No hay servidor en " << IP_SERVIDOR << ":" << PUERTO << ". Arrancalo primero.\n";
        close(sock);
        return 1;
    }
    cout << "Conectado a " << IP_SERVIDOR << ":" << PUERTO << "\n";

    string mensajes[3] = {"hola", "que tal", "adios"};   // array de 3 textos
    bool seguir = true;
    int i = 0;
    while (i < 3 && seguir) {
        // send(socket, datos, cuántos bytes, opciones). c_str() = texto como char*, size() = su longitud.
        // MSG_NOSIGNAL: si el servidor ya ha cerrado, send devuelve -1 en vez de matar el programa.
        send(sock, mensajes[i].c_str(), mensajes[i].size(), MSG_NOSIGNAL);
        char buffer[1024];
        // recv: espera bytes del servidor. Devuelve cuántos llegaron, 0 si cerró o -1 si error o timeout
        ssize_t n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            cout << "El servidor ha cerrado la conexion\n";
            seguir = false;
        } else {
            cout << "Eco: " << string(buffer, n) << "\n";   // string(buffer, n): los n bytes recibidos como texto
        }
        i++;
    }

    close(sock);   // colgar: el servidor verá que recv devuelve 0
    return 0;      // 0 = el programa ha terminado bien
}

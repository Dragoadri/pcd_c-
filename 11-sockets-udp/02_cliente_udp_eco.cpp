// =====================================================================
// Tema 11.2  |  Cliente UDP de eco
// En este programa he enviado tres mensajes al servidor UDP con sendto y he esperado cada respuesta con recvfrom durante 3 s como máximo.
// Requisitos previos: 01_servidor_udp_eco.cpp
// Qué observar al ejecutar: con el servidor arrancado vuelve cada mensaje; sin servidor, sendto NO da error y solo nos enteramos por el timeout.
// Salida determinista: SI con el servidor arrancado (sin servidor, siempre "sin respuesta")
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: primero 01_servidor_udp_eco en otro terminal. Pruébalo también SIN servidor.
#include <iostream>       // cout
#include <string>         // string
#include <cstdio>         // perror
#include <sys/socket.h>   // socket, sendto, recvfrom, setsockopt
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // htons, inet_pton
#include <sys/time.h>     // timeval
#include <unistd.h>       // close

using namespace std;

const char* IP_SERVIDOR = "127.0.0.1";  // cámbiala por la IP del equipo del servidor
const int PUERTO = 5100;                // el puerto donde escucha 01_servidor_udp_eco
const int SEGUNDOS_RESPUESTA = 3;

int main() {
    // socket: AF_INET = IPv4, SOCK_DGRAM = UDP. Devuelve un número que identifica el socket, o -1
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");   // imprime el motivo del error
        return 1;
    }
    timeval espera{};   // tiempo: tv_sec segundos (y tv_usec microsegundos, aquí 0)
    espera.tv_sec = SEGUNDOS_RESPUESTA;
    // SO_RCVTIMEO: recvfrom no esperará más de 3 s; pasado ese tiempo devuelve -1
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));

    // El cliente NO hace bind: el sistema le asigna un puerto libre al enviar
    sockaddr_in servidor{};   // la dirección (IP y puerto) a la que vamos a enviar
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);   // htons: puerto en orden de red
    inet_pton(AF_INET, IP_SERVIDOR, &servidor.sin_addr);   // IP de texto a 4 bytes

    string mensajes[3] = {"hola", "que tal", "adios"};   // array de 3 textos
    bool servidor_responde = true;   // bandera: a false si una respuesta no llega
    int i = 0;
    while (i < 3 && servidor_responde) {
        // sendto(socket, datos, cuántos bytes, opciones, a quién, tamaño de esa dirección).
        // c_str() da el texto como char*; size() su número de caracteres.
        // (sockaddr*) &servidor: la dirección de nuestra sockaddr_in vista como el tipo genérico que pide sendto.
        ssize_t enviados = sendto(sock, mensajes[i].c_str(), mensajes[i].size(), 0,
                                  (sockaddr*) &servidor, sizeof(servidor));
        cout << "Enviado \"" << mensajes[i] << "\" (sendto devolvio " << enviados << ")\n";

        char buffer[1024];
        // recvfrom espera un datagrama. Los dos nullptr = no me interesa saber quién lo envía.
        // Devuelve los bytes recibidos, o -1 si pasan los 3 s sin nada (ssize_t admite el -1).
        ssize_t n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
        if (n == -1) {
            // Una carta sin respuesta: no sabemos si se perdió, si no hay nadie o si va lenta
            cout << "Sin respuesta en " << SEGUNDOS_RESPUESTA << " s. Esta arrancado el servidor?\n";
            servidor_responde = false;
        } else {
            buffer[n] = '\0';   // marcar el fin del texto: recvfrom no lo pone
            cout << "Respuesta: \"" << buffer << "\"\n";
        }
        i++;
    }

    close(sock);   // liberar el socket
    return 0;      // 0 = el programa ha terminado bien
}

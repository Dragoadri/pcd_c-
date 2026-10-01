// =====================================================================
// Tema 11.4  |  Emisor UDP: envía aunque nadie escuche
// En este programa he enviado con sendto 2000 datagramas numerados lo más rápido posible, para ver que no hay error aunque nadie los reciba.
// Requisitos previos: 03_receptor_udp_sin_garantias.cpp
// Qué observar al ejecutar: "2000 enviados sin error" tanto CON receptor como SIN él (pruébalo de las dos formas).
// Salida determinista: SI (el emisor siempre cree que todo ha ido bien; eso es justo lo que hay que ver)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: 1) sin receptor: ejecútalo solo y mira que no hay error.
//                2) con receptor: arranca antes 03_receptor_udp_sin_garantias en otro terminal.
#include <iostream>       // cout
#include <string>         // string, to_string
#include <cstdio>         // perror
#include <sys/socket.h>   // socket, sendto
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // htons, inet_pton
#include <unistd.h>       // close

using namespace std;

const char* IP_DESTINO = "127.0.0.1";   // localhost: este mismo equipo
const int PUERTO = 5101;                // el puerto donde escucha 03_receptor_udp_sin_garantias
const int TOTAL = 2000;

int main() {
    // socket UDP: AF_INET = IPv4, SOCK_DGRAM = datagramas. Devuelve su número o -1 si falla
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }
    sockaddr_in destino{};   // dirección IPv4 a la que enviamos ({} = todo a 0)
    destino.sin_family = AF_INET;
    destino.sin_port = htons(PUERTO);                    // htons: puerto en orden de red
    inet_pton(AF_INET, IP_DESTINO, &destino.sin_addr);   // IP de texto a sus 4 bytes

    // Ráfaga: tan rápido como se pueda, cada datagrama lleva su número de orden
    int sin_error = 0;
    for (int i = 0; i < TOTAL; i++) {
        string texto = to_string(i);   // to_string: entero a texto (17 -> "17")
        // sendto(socket, datos, cuántos bytes, opciones, destino, tamaño del destino).
        // (sockaddr*) &destino: la dirección de destino vista como el tipo genérico que pide sendto.
        // Devuelve los bytes enviados o -1. Que no sea -1 solo significa "entregado a MI sistema".
        ssize_t n = sendto(sock, texto.c_str(), texto.size(), 0, (sockaddr*) &destino, sizeof(destino));
        if (n != -1) {
            sin_error++;
        }
    }

    cout << sin_error << " de " << TOTAL << " enviados sin error a "
              << IP_DESTINO << ":" << PUERTO << "\n";
    cout << "El emisor no sabe si alguien los ha recibido: UDP no confirma nada\n";
    close(sock);   // liberar el socket
    return 0;      // 0 = el programa ha terminado bien
}

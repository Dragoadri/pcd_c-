// =====================================================================
// Tema 11.3  |  Receptor UDP lento: se pierden datagramas sin avisar
// En este programa he recibido una ráfaga de 2000 datagramas numerados tardando 1 ms en cada uno, y he contado cuántos llegan, huecos y desorden.
// Requisitos previos: 01_servidor_udp_eco.cpp, 02_cliente_udp_eco.cpp
// Qué observar al ejecutar: "recibidos X de 2000" con X muy por debajo y el último número que llegó; nadie ha dado ningún error.
// Salida determinista: NO (cuántos se pierden depende de la carga del equipo y del tamaño del búfer del sistema)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: terminal 1 -> ./build/bin/03_receptor_udp_sin_garantias
//                terminal 2 -> ./build/bin/04_emisor_udp_sin_conexion
// Medido en el equipo de desarrollo (localhost, búfer por defecto, 3 ejecuciones):
//   MS_TRABAJO = 1 -> recibidos 224, 222 y 222 de 2000. El búfer del sistema se llena
//                     con los primeros ~220 y descarta TODOS los demás en silencio.
//   MS_TRABAJO = 0 -> recibidos 2000, 2000 y 1898: incluso rápido se pierde alguno.
// En localhost no suele haber desorden; entre dos equipos por wifi aparecen huecos
// y desorden. Hazlo con dos portátiles si la red del aula lo permite.
#include <iostream>       // cout
#include <string>         // string, stoi
#include <thread>         // this_thread::sleep_for
#include <chrono>         // milliseconds
#include <cstdio>         // perror
#include <sys/socket.h>   // socket, bind, recvfrom, setsockopt
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // htons, inet_pton
#include <sys/time.h>     // timeval
#include <unistd.h>       // close

using namespace std;

const char* IP_ESCUCHA = "127.0.0.1";   // localhost: este mismo equipo
const int PUERTO = 5101;
const int TOTAL_ESPERADO = 2000;       // lo que envía 04_emisor_udp_sin_conexion
const int MS_TRABAJO = 1;              // "procesar" cada datagrama tarda esto (pon 0 y compara)

int main() {
    // socket UDP (AF_INET = IPv4, SOCK_DGRAM = datagramas). Devuelve su número o -1 si falla
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }
    int activar = 1;
    // SO_REUSEADDR: poder volver a usar el puerto nada más relanzar el programa
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &activar, sizeof(activar));
    timeval espera{};                  // tiempo en segundos (tv_sec) y microsegundos (tv_usec)
    espera.tv_sec = 30;                // para el primer datagrama esperamos hasta 30 s
    // SO_RCVTIMEO: tiempo máximo que recvfrom se queda esperando antes de devolver -1
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));

    sockaddr_in direccion{};           // dirección IPv4 donde vamos a escuchar
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(PUERTO);                    // htons: puerto en orden de red
    inet_pton(AF_INET, IP_ESCUCHA, &direccion.sin_addr);   // IP de texto a sus 4 bytes
    // ::bind es el bind de POSIX. Con using namespace std, sin los '::' el compilador elegiría bind de la librería estándar.
    // bind reserva IP y puerto para este socket. (sockaddr*) convierte la dirección al tipo genérico que pide bind.
    if (::bind(sock, (sockaddr*) &direccion, sizeof(direccion)) == -1) {
        perror("bind");
        close(sock);
        return 1;
    }
    cout << "Receptor UDP en el puerto " << PUERTO << ", esperando la rafaga...\n";

    int recibidos = 0;
    int ultimo = -1;          // último número recibido; -1 = todavía ninguno
    int desordenados = 0;
    int huecos = 0;
    bool llegan_datos = true;
    while (llegan_datos) {
        char buffer[64];
        // recvfrom: espera un datagrama (nullptr, nullptr = no me interesa quién lo envía).
        // Devuelve cuántos bytes llegaron, o -1 si se agota el tiempo de espera.
        ssize_t n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
        if (n == -1) {
            llegan_datos = false;      // timeout: la ráfaga ha terminado (o nunca empezó)
        } else {
            buffer[n] = '\0';          // fin de cadena, para poder tratarlo como texto
            int numero = stoi(buffer); // stoi: texto a entero ("17" -> 17)
            if (numero < ultimo) {     // llega uno menor que el anterior: vino desordenado
                desordenados++;
            }
            if (numero > ultimo + 1) { // se ha saltado números: esos datagramas se perdieron
                huecos++;
                if (huecos <= 5) {     // solo los 5 primeros, para no llenar la pantalla
                    cout << "  Hueco: faltan del " << ultimo + 1 << " al " << numero - 1 << "\n";
                }
            }
            ultimo = numero;
            recibidos++;
            // Tras el primero, 2 s de silencio bastan para dar la ráfaga por terminada
            espera.tv_sec = 2;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));
            // Simula lo que tarda en procesar el datagrama: mientras duerme, los que llegan
            // se acumulan en el búfer del sistema y, cuando se llena, se tiran
            this_thread::sleep_for(chrono::milliseconds(MS_TRABAJO));
        }
    }

    cout << "Ultimo numero recibido: " << ultimo << "\n";
    cout << "Recibidos " << recibidos << " de " << TOTAL_ESPERADO
              << ", desordenados: " << desordenados << "\n";
    cout << "Perdidos: " << TOTAL_ESPERADO - recibidos << " y NADIE ha recibido un error\n";
    close(sock);   // liberar el socket y su puerto
    return 0;      // 0 = el programa ha terminado bien
}

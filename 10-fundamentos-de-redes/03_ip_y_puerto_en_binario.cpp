// =====================================================================
// Tema 10.3  |  IP y puerto tal como viajan por la red (inet_pton y htons)
// En este programa he pasado una IP de texto a sus 4 bytes con inet_pton y he mostrado los bytes del puerto 5000 antes y después de htons.
// Requisitos previos: 02_resolver_nombre_dns.cpp, APUNTES_REDES.md (apartado orden de bytes)
// Qué observar al ejecutar: los bytes del puerto 5000 salen al revés antes y después de htons en un PC Intel/AMD.
// Salida determinista: SI
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Estas dos conversiones aparecen en TODOS los ejemplos de sockets de las
// carpetas 11, 12 y 13. Aquí se ven solas, sin enviar nada todavía.
#include <iostream>      // cout
#include <cstdint>       // uint16_t
#include <arpa/inet.h>   // inet_pton, inet_ntop, htons, ntohs

using namespace std;

// Muestra los 2 bytes de un número de 16 bits en el orden en que están en memoria
// const char* titulo: un texto entre comillas. uint16_t: entero sin signo de 16 bits (2 bytes, de 0 a 65535)
void mostrar_bytes(const char* titulo, uint16_t valor) {
    // &valor es la dirección del número; con el cast (unsigned char*) lo recorro byte a byte
    unsigned char* bytes = (unsigned char*) &valor;
    // (int) hace que cout muestre cada byte como número (sin él lo imprimiría como una letra)
    cout << titulo << ": valor=" << valor
              << "  bytes en memoria = " << (int) bytes[0] << " " << (int) bytes[1] << "\n";
}

int main() {
    // ---- 1) IP: de texto a binario y vuelta ----
    in_addr ip{};   // una IPv4 son 4 bytes (guardados en su campo s_addr); {} los pone a 0
    // inet_pton (presentation to network): convierte el texto "192.168.1.10" en 4 bytes y los
    // escribe en ip (por eso se pasa &ip, su dirección). Devuelve 1 si el texto era una IPv4 válida.
    if (inet_pton(AF_INET, "192.168.1.10", &ip) == 1) {
        unsigned char* bytes = (unsigned char*) &ip.s_addr;   // miro los 4 bytes uno a uno
        cout << "192.168.1.10 en memoria: " << (int) bytes[0] << " " << (int) bytes[1]
                  << " " << (int) bytes[2] << " " << (int) bytes[3] << "\n";
    }
    char texto[INET_ADDRSTRLEN];   // INET_ADDRSTRLEN = 16: cabe "255.255.255.255" y el fin de cadena
    // inet_ntop (network to presentation): el camino contrario, de 4 bytes a texto
    inet_ntop(AF_INET, &ip, texto, sizeof(texto));
    cout << "Y de vuelta a texto: " << texto << "\n";

    // inet_pton devuelve 0 si el texto no es una IPv4 válida
    in_addr mala{};
    cout << "Es valida 300.1.1.1? " << inet_pton(AF_INET, "300.1.1.1", &mala) << " (0 = no)\n\n";

    // ---- 2) Puerto: orden de la máquina frente a orden de red ----
    // 5000 = 0x1388 -> byte alto 0x13 (19), byte bajo 0x88 (136).
    // Los PC Intel/AMD guardan primero el byte bajo (little endian);
    // la red exige primero el byte alto (big endian). htons = host to network short.
    uint16_t puerto = 5000;
    mostrar_bytes("Puerto en orden de la maquina", puerto);
    uint16_t puerto_red = htons(puerto);   // da la vuelta a los 2 bytes (en un PC Intel/AMD)
    mostrar_bytes("Puerto tras htons (orden red)", puerto_red);
    // ntohs = network to host short: deshace htons, para leer un puerto que llega de la red
    mostrar_bytes("Recuperado con ntohs        ", ntohs(puerto_red));

    // Si olvidas htons, el servidor escucha en OTRO puerto: 5000 se leería como 34835
    cout << "\nSin htons el otro extremo leeria el puerto " << ntohs(puerto) << "\n";
    return 0;   // 0 = el programa ha terminado bien
}

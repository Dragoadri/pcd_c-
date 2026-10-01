// =====================================================================
// Tema 10.1  |  Nombre del equipo y sus direcciones IP
// En este programa he pedido al sistema el nombre del equipo y he recorrido sus interfaces de red para mostrar la IP y la máscara de cada una.
// Requisitos previos: APUNTES_REDES.md (apartados 1 a 3)
// Qué observar al ejecutar: la interfaz "lo" con 127.0.0.1 y la de la wifi o el cable con la IP de la red local.
// Salida determinista: SI en un mismo equipo (cambia de un ordenador a otro y de una red a otra)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Compara la salida con el comando de terminal:  hostname  y  ip -4 a
#include <iostream>      // cout
#include <cstdio>        // perror: imprime un texto y el motivo del último error del sistema
#include <unistd.h>      // gethostname (POSIX)
#include <ifaddrs.h>     // getifaddrs: lista de interfaces de red
#include <netinet/in.h>  // sockaddr_in: dirección IPv4
#include <arpa/inet.h>   // inet_ntop: dirección binaria -> texto

using namespace std;

int main() {
    // 1) El nombre del equipo (el que ves en el prompt del terminal)
    char nombre[256] = {0};   // array de 256 caracteres, todos a 0 (texto vacío)
    // gethostname copia el nombre dentro de 'nombre'; sizeof(nombre) le dice cuánto cabe (256).
    // Las funciones del sistema devuelven -1 cuando fallan.
    if (gethostname(nombre, sizeof(nombre)) == -1) {
        perror("gethostname");   // muestra "gethostname: <motivo del error>"
        return 1;                // 1 = el programa ha terminado con error
    }
    cout << "Nombre del equipo: " << nombre << "\n\n";

    // 2) Las interfaces de red. getifaddrs rellena una lista enlazada:
    //    cada elemento apunta al siguiente con ifa_next, y el último a nullptr.
    ifaddrs* lista = nullptr;   // puntero: guardará DÓNDE está el primer elemento de la lista
    // &lista = la dirección de nuestra variable, para que getifaddrs pueda escribir en ella
    if (getifaddrs(&lista) == -1) {
        perror("getifaddrs");
        return 1;
    }

    ifaddrs* actual = lista;   // empezamos por el primer elemento
    while (actual != nullptr) {   // nullptr = ya no quedan elementos
        // Solo nos interesan las direcciones IPv4 (familia AF_INET)
        // actual->ifa_addr es "el campo ifa_addr del elemento al que apunta actual".
        // Primero se comprueba que no sea nullptr: si lo fuera, leer sa_family haría fallar el programa.
        if (actual->ifa_addr != nullptr && actual->ifa_addr->sa_family == AF_INET) {
            // La dirección viene como sockaddr "genérico"; sabemos que es IPv4,
            // así que la tratamos como sockaddr_in para leer sus campos.
            // (sockaddr_in*) es un cast: le digo al compilador que mire esos bytes como sockaddr_in.
            sockaddr_in* ip = (sockaddr_in*) actual->ifa_addr;
            sockaddr_in* mascara = (sockaddr_in*) actual->ifa_netmask;   // la máscara indica qué parte de la IP es la red

            // INET_ADDRSTRLEN = 16, lo justo para "255.255.255.255" más el fin de cadena
            char texto_ip[INET_ADDRSTRLEN];
            char texto_mascara[INET_ADDRSTRLEN];
            // inet_ntop (network to presentation): convierte los 4 bytes de sin_addr en texto "192.168.1.10".
            // Se le pasa la familia, DÓNDE están los bytes (&), dónde escribir el texto y cuánto cabe.
            inet_ntop(AF_INET, &ip->sin_addr, texto_ip, sizeof(texto_ip));
            inet_ntop(AF_INET, &mascara->sin_addr, texto_mascara, sizeof(texto_mascara));

            cout << "Interfaz " << actual->ifa_name << "\n";   // ifa_name: "lo", "wlp2s0", "eth0"...
            cout << "   IP:      " << texto_ip << "\n";
            cout << "   Mascara: " << texto_mascara << "\n";
        }
        actual = actual->ifa_next;   // avanzar al siguiente elemento de la lista
    }

    freeifaddrs(lista);   // la lista la reservó el sistema: hay que liberarla
    return 0;             // 0 = el programa ha terminado bien
}

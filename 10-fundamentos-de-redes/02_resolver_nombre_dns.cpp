// =====================================================================
// Tema 10.2  |  Resolver un nombre con DNS
// En este programa he traducido tres nombres a direcciones IP con getaddrinfo, para ver que se conecta a IPs y que el DNS hace la traducción.
// Requisitos previos: 01_mis_direcciones_ip.cpp, APUNTES_REDES.md (apartado DNS)
// Qué observar al ejecutar: localhost siempre da 127.0.0.1; un nombre de Internet da una o varias IPs, o un error si no hay red.
// Salida determinista: NO (las IPs de un nombre de Internet pueden cambiar entre ejecuciones y según la red)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Compara con el comando de terminal:  getent hosts www.universidadeuropea.com
#include <iostream>      // cout
#include <string>        // string
#include <netdb.h>       // getaddrinfo, gai_strerror, addrinfo
#include <netinet/in.h>  // sockaddr_in
#include <arpa/inet.h>   // inet_ntop

using namespace std;

// const string& nombre: recibe el texto sin copiarlo (&) y promete no modificarlo (const)
void resolver(const string& nombre) {
    addrinfo pistas{};              // qué tipo de respuesta queremos; {} pone todos sus campos a 0
    pistas.ai_family = AF_INET;     // solo IPv4
    pistas.ai_socktype = SOCK_STREAM;   // direcciones pensadas para TCP (así no sale cada IP repetida por UDP)

    addrinfo* resultado = nullptr;  // aquí getaddrinfo dejará la dirección de la lista de respuestas
    // getaddrinfo pregunta al sistema (y este al servidor DNS) qué IPs tiene 'nombre'.
    // nombre.c_str() da el texto como char* (lo que piden las funciones de C).
    // nullptr = no pedimos ningún puerto concreto. &resultado = dónde guardar la lista.
    // Devuelve 0 si ha ido bien, o un código de error.
    int error = getaddrinfo(nombre.c_str(), nullptr, &pistas, &resultado);

    if (error != 0) {
        // Sin red o nombre inexistente: no es un fallo del programa, es la respuesta del DNS
        // gai_strerror convierte el código de error en un texto legible
        cout << nombre << " -> no se pudo resolver: " << gai_strerror(error) << "\n";
    } else {
        // Un nombre puede tener VARIAS IPs (balanceo de carga): se recorre la lista
        addrinfo* actual = resultado;
        while (actual != nullptr) {
            // ai_addr es un sockaddr genérico; como pedimos AF_INET, lo miro como sockaddr_in (cast)
            sockaddr_in* ip = (sockaddr_in*) actual->ai_addr;
            char texto[INET_ADDRSTRLEN];   // hueco para el texto de una IPv4 (16 caracteres)
            // inet_ntop: los 4 bytes de la IP (&ip->sin_addr) pasan a texto "a.b.c.d"
            inet_ntop(AF_INET, &ip->sin_addr, texto, sizeof(texto));
            cout << nombre << " -> " << texto << "\n";
            actual = actual->ai_next;      // siguiente respuesta (nullptr si era la última)
        }
        freeaddrinfo(resultado);   // la lista la reservó getaddrinfo: se libera al terminar
    }
}

int main() {
    resolver("localhost");                   // lo resuelve el propio equipo (/etc/hosts)
    resolver("www.universidadeuropea.com");  // necesita un servidor DNS (Internet)
    resolver("esto.no.existe.invalid");      // .invalid está reservado: nunca existe
    return 0;   // 0 = el programa ha terminado bien
}

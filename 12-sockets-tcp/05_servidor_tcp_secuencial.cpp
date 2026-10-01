// =====================================================================
// Tema 12.5  |  Servidor TCP secuencial: atiende de uno en uno
// En este programa he atendido a los clientes TCP de uno en uno en un bucle de accept, tardando 2 s con cada uno, y he medido los tiempos.
// Requisitos previos: 03_servidor_tcp_por_lineas.cpp, 04_cliente_tcp_por_lineas.cpp
// Qué observar al ejecutar: los tiempos "t=": el segundo cliente no empieza a ser atendido hasta que termina el primero.
// Salida determinista: NO (los tiempos y el orden dependen de cuándo se lancen los clientes)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: terminal 1 -> ./build/bin/05_servidor_tcp_secuencial
//                terminales 2 y 3, casi a la vez -> ./build/bin/06_cliente_tcp_lento
//   (o en un solo terminal: ./build/bin/06_cliente_tcp_lento & ./build/bin/06_cliente_tcp_lento)
// PROTOCOLO: el cliente envía su nombre en una línea; el servidor trabaja 2 s y contesta una línea.
// Termina tras MAX_CLIENTES clientes o 30 s sin clientes nuevos.
// enviar_todo y recibir_linea se copian tal cual en los servidores de la carpeta 13.
#include <iostream>       // cout
#include <string>         // string
#include <thread>         // this_thread::sleep_for
#include <chrono>         // steady_clock, duration, seconds
#include <cstdio>         // perror
#include <sys/socket.h>   // socket, bind, listen, accept, send, recv, setsockopt
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // htons, inet_pton
#include <sys/time.h>     // timeval
#include <unistd.h>       // close

using namespace std;

const char* IP_ESCUCHA = "127.0.0.1";   // localhost: solo clientes de este mismo equipo
const int PUERTO = 5202;
const int MAX_CLIENTES = 5;

// send puede enviar MENOS bytes de los pedidos: se repite hasta que sale todo.
// const string& texto: el texto sin copiarlo (&) y sin poder cambiarlo (const). false = la conexión falló
bool enviar_todo(int fd, const string& texto) {
    size_t enviados = 0;   // bytes que ya han salido
    bool ok = true;
    while (ok && enviados < texto.size()) {
        // c_str() + enviados: puntero al primer byte que falta; size() - enviados: cuántos faltan.
        // MSG_NOSIGNAL: si el otro extremo ya cerró, send devuelve -1 en vez de matar el programa.
        ssize_t n = send(fd, texto.c_str() + enviados, texto.size() - enviados, MSG_NOSIGNAL);
        if (n <= 0) {
            ok = false;
        } else {
            enviados += n;
        }
    }
    return ok;
}

// Lee de byte en byte hasta '\n' (que no se guarda). Leer así es lento pero sencillo,
// y nunca se "come" bytes de la línea siguiente. false = el otro cerró, error o timeout.
// string& linea: SIN const, porque la función escribe en la variable de quien la llama (así devuelve la línea)
bool recibir_linea(int fd, string& linea) {
    linea = "";
    bool ok = true;
    bool fin_de_linea = false;
    while (ok && !fin_de_linea) {   // seguir mientras no haya fallo y no haya llegado el '\n'
        char c = 0;
        // recv de 1 byte: &c es la dirección de c, donde recv deja el carácter.
        // Devuelve 1 si llegó, 0 si el otro cerró, -1 si error o timeout.
        ssize_t n = recv(fd, &c, 1, 0);
        if (n <= 0) {
            ok = false;
        } else if (c == '\n') {
            fin_de_linea = true;
        } else {
            linea += c;   // añadir el carácter al final de la línea
        }
    }
    return ok;
}

int main() {
    // steady_clock::now(): el instante actual, en un reloj que nunca va hacia atrás
    chrono::steady_clock::time_point inicio = chrono::steady_clock::now();
    // socket TCP: AF_INET = IPv4, SOCK_STREAM = flujo de bytes. Devuelve su número o -1
    int escucha = socket(AF_INET, SOCK_STREAM, 0);
    int activar = 1;
    // SO_REUSEADDR: poder relanzar el servidor en el mismo puerto enseguida
    setsockopt(escucha, SOL_SOCKET, SO_REUSEADDR, &activar, sizeof(activar));
    timeval espera{};   // tiempo: tv_sec segundos, tv_usec microsegundos
    espera.tv_sec = 30;
    // SO_RCVTIMEO: accept y recv esperan como mucho 30 s y luego devuelven -1
    setsockopt(escucha, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));
    sockaddr_in direccion{};                               // dirección IPv4 donde escuchar
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(PUERTO);                    // htons: puerto en orden de red
    inet_pton(AF_INET, IP_ESCUCHA, &direccion.sin_addr);   // IP de texto a 4 bytes
    // ::bind es el bind de POSIX. Con using namespace std, sin los '::' el compilador elegiría bind de la librería estándar.
    // Con ||, si socket falló (-1) ya no se llama a bind. (sockaddr*) = la dirección vista como el tipo genérico.
    if (escucha == -1 || ::bind(escucha, (sockaddr*) &direccion, sizeof(direccion)) == -1) {
        perror("socket o bind");
        return 1;
    }
    listen(escucha, 10);   // listen: aceptar llamadas; hasta 10 clientes esperando en cola
    cout << "Servidor SECUENCIAL en el puerto " << PUERTO << "\n";

    int atendidos = 0;
    bool seguir = true;   // bandera: a false cuando pasan 30 s sin clientes
    while (seguir && atendidos < MAX_CLIENTES) {
        // accept: saca al siguiente cliente de la cola y devuelve un socket para hablar con él.
        // nullptr, nullptr = no me interesa su IP ni su puerto. Mientras atiendo a este, los demás esperan.
        int conexion = accept(escucha, nullptr, nullptr);
        if (conexion == -1) {
            cout << "30 s sin clientes nuevos: cierro\n";
            seguir = false;
        } else {
            // Restar dos instantes da una duración; duration<double> la guarda en segundos con decimales
            chrono::duration<double> t = chrono::steady_clock::now() - inicio;
            cout << "t=" << t.count() << " s  acepto un cliente y espero su nombre\n";   // count(): el número de segundos
            setsockopt(conexion, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));   // recv espera 30 s como mucho

            string nombre;
            if (recibir_linea(conexion, nombre)) {   // rellena 'nombre' con la primera línea del cliente
                this_thread::sleep_for(chrono::seconds(2));   // simula el trabajo
                enviar_todo(conexion, "HOLA " + nombre + ", te ha atendido el servidor secuencial\n");
                t = chrono::steady_clock::now() - inicio;
                cout << "t=" << t.count() << " s  termino con " << nombre << "\n";
            } else {
                cout << "El cliente no envio su nombre\n";
            }
            close(conexion);   // colgar a este cliente; solo ahora se atiende al siguiente
            atendidos++;
        }
    }
    cout << "Clientes atendidos: " << atendidos << "\n";
    close(escucha);   // liberar el puerto
    return 0;         // 0 = el programa ha terminado bien
}

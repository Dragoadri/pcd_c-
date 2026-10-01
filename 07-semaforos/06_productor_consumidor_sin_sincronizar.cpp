// =====================================================================
// Tema 7.6  |  Productor-consumidor sin sincronizar
// En este programa he hecho que un productor y un consumidor compartan un búfer circular de 5 huecos sin protección, para ver que se pierden datos.
// Requisitos previos: 04-hilos-usados-mal, 05_semaforo_artesanal.cpp
// Qué observar al ejecutar: el resumen final: perdidos, repetidos y huecos vacíos leídos distintos de 0.
// Salida determinista: NO (las cifras cambian en cada ejecución; nunca salen las tres a 0)
// Compilar con: -O0 (target sin optimizar; con -O2 la carrera se ve mucho menos)
// =====================================================================
// Material original: Tema 2, productor-consumidor. En Python se usaba queue.Queue, que
// ya viene sincronizada; aquí escribimos el búfer a mano para ver el problema.
// El original tenía bucles "while True"; aquí se acotan por número de elementos y por
// un tiempo máximo de 3 segundos, para que el programa termine siempre.
// Espera activa: si el búfer está lleno o vacío, se vuelve a mirar (yield). Gasta CPU.
// Calibración (16 núcleos, GCC 13, -O0, 11 ejecuciones): entre 12 800 y 15 300 perdidos
// de 100 000. El consumidor cuenta huecos vacíos como si fueran datos, cree que ya tiene
// los 100 000 y se va; el productor se queda esperando un sitio libre que nadie va a
// dejar y el programa termina por el límite de 3 s (fallo de vivacidad además de seguridad).
#include <functional>   // ref, para pasar variables por referencia a un hilo
#include <iostream>     // cout
#include <thread>       // thread y this_thread
#include <vector>       // vector
#include <chrono>       // steady_clock y seconds, para el tiempo límite

using namespace std;

const int TAM = 5;             // capacidad del búfer
const int ELEMENTOS = 100000;  // cuántos produce el productor

vector<int> bufer(TAM, -1);   // TAM casillas, todas a -1 al empezar (-1 = hueco vacío)
int entrada = 0;     // dónde escribe el productor
int salida = 0;      // dónde lee el consumidor
int cantidad = 0;    // cuántos hay: lo modifican LOS DOS hilos sin protección

// time_point: un instante concreto del reloj steady_clock (un reloj que nunca va hacia atrás)
chrono::steady_clock::time_point limite;

void productor() {
    int siguiente = 0;
    // Para cuando ha producido los ELEMENTOS o cuando la hora actual (now) pasa del límite
    while (siguiente < ELEMENTOS && chrono::steady_clock::now() < limite) {
        if (cantidad < TAM) {                 // ¿queda sitio en el búfer?
            bufer[entrada] = siguiente;
            entrada = (entrada + 1) % TAM;    // % TAM: después de la casilla 4 vuelve a la 0 (búfer circular)
            cantidad++;                    // carrera: leer, sumar, escribir
            siguiente++;
        } else {
            this_thread::yield();     // lleno: vuelvo a intentarlo (yield cede la CPU a otro hilo un momento)
        }
    }
}

// vector<int>&: llena el vector de main, no una copia.
void consumidor(vector<int>& recibidos) {
    // size() dice cuántos lleva recibidos; (int) lo convierte a int para compararlo con ELEMENTOS
    while ((int)recibidos.size() < ELEMENTOS && chrono::steady_clock::now() < limite) {
        if (cantidad > 0) {                       // ¿hay algo que leer?
            recibidos.push_back(bufer[salida]);   // push_back añade al final lo que haya en esa casilla
            bufer[salida] = -1;                   // marca la casilla como hueco vacío
            salida = (salida + 1) % TAM;
            cantidad--;                    // carrera con el cantidad++ del productor
        } else {
            this_thread::yield();     // vacío: vuelvo a intentarlo
        }
    }
}

int main() {
    limite = chrono::steady_clock::now() + chrono::seconds(3);   // ahora + 3 segundos
    vector<int> recibidos;
    thread p(productor);
    thread c(consumidor, ref(recibidos));   // ref(): el hilo recibe recibidos por referencia
    p.join();                               // espero a los dos hilos
    c.join();

    // Comprobación: cuántas veces llegó cada número
    vector<int> veces(ELEMENTOS, 0);        // veces[n] = cuántas veces ha llegado el número n
    int huecos_vacios = 0;
    for (int i = 0; i < (int)recibidos.size(); i++) {
        if (recibidos[i] >= 0) {            // un dato de verdad (no un -1)
            veces[recibidos[i]]++;          // uso el número recibido como posición en veces
        } else {
            huecos_vacios++;
        }
    }
    int perdidos = 0;
    int repetidos = 0;
    for (int i = 0; i < ELEMENTOS; i++) {
        if (veces[i] == 0) {                // el número i no llegó nunca
            perdidos++;
        } else if (veces[i] > 1) {          // el número i llegó más de una vez
            repetidos++;
        }
    }
    cout << "Recibidos: " << recibidos.size() << " de " << ELEMENTOS << "\n";
    cout << "Perdidos: " << perdidos << "  Repetidos: " << repetidos
              << "  Huecos vacios leidos: " << huecos_vacios << "\n";
    int ocupados_de_verdad = 0;             // casillas del búfer que no están a -1
    for (int i = 0; i < TAM; i++) {
        if (bufer[i] >= 0) {
            ocupados_de_verdad++;
        }
    }
    cout << "Variable cantidad: " << cantidad << "  Elementos reales en el bufer: "
              << ocupados_de_verdad << "\n";
    // Si ya pasó el límite, algún hilo salió por tiempo y no por haber terminado su trabajo
    if (chrono::steady_clock::now() >= limite) {
        cout << "Se agoto el tiempo: un hilo se quedo esperando algo que nunca iba a llegar\n";
    }
    return 0;   // 0 = el programa ha terminado bien
}

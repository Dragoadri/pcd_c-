// =====================================================================
// Tema 5.6  |  Intento 3 de exclusión mutua: señalizar primero, interbloqueo (LECTURA, NO EXAMINABLE)
// En este programa he hecho que cada hilo baje su bandera y luego espere al otro, para ver que los dos se quedan esperando: interbloqueo.
// Requisitos previos: 05/05, LECTURA_cuatro_intentos.md (intento 3)
// Qué observar al ejecutar: los dos hilos informan "interbloqueo detectado" y ninguno llega a entrar.
// Salida determinista: NO (en teoría; con la ventana abierta a propósito sale siempre el interbloqueo)
// Compilar con: cualquier nivel (las banderas son atomic)
// =====================================================================
// Pseudocódigo (UA1_T04):
//   c[yo] := 0                 <- señalizo primero
//   while c[otro] == 0: {}     <- espero al otro
//   SC ; c[yo] := 1
// Arregla el intento 2 (ya no entran los dos), pero si ambos ponen su
// bandera a 0 antes de mirar, cada uno espera algo que solo el otro puede
// cambiar. Es un fallo de VIVACIDAD, no de seguridad.
// Cambio respecto al material: la espera "while c[otro] == 0: {}" es
// infinita. Aquí se acota a MAX_VUELTAS (~1 s) y se informa, para que el
// programa termine siempre. atomic y seq_cst por la trampa 3 (ver 05/05).
#include <iostream>   // cout
#include <string>     // string y to_string
#include <thread>     // thread y this_thread::sleep_for
#include <atomic>     // atomic y memory_order_seq_cst
#include <chrono>     // milliseconds

using namespace std;

const long MAX_VUELTAS = 300000000;
atomic<int> c[2];          // bandera de cada proceso: 1 = no quiere entrar, 0 = quiere entrar o está dentro
atomic<int> entradas(0);   // cuántos procesos han llegado a la sección crítica

void proceso(int yo) {
    int otro = 1 - yo;   // si yo soy 0 el otro es 1, y al revés
    c[yo].store(0, memory_order_seq_cst);                     // quiero entrar (store escribe la bandera)
    this_thread::sleep_for(chrono::milliseconds(10));    // ventana abierta: da tiempo a que el otro baje la suya

    long vueltas = 0;
    // load lee la bandera del otro. Sigo esperando mientras el otro quiera
    // entrar (0) y no haya dado MAX_VUELTAS vueltas.
    while (c[otro].load(memory_order_seq_cst) == 0 && vueltas < MAX_VUELTAS) {
        vueltas++;                                                 // espera activa
    }
    if (vueltas == MAX_VUELTAS) {   // he salido del while por agotar vueltas, no porque el otro soltara
        // No bajamos la bandera: un proceso interbloqueado no sale nunca
        // El mensaje se construye entero con + y sale con un solo <<, para que no se mezcle.
        cout << "P" + to_string(yo + 1) + ": interbloqueo detectado, el otro nunca suelta\n";
    } else {
        entradas++;                                                // SECCIÓN CRÍTICA
        c[yo].store(1, memory_order_seq_cst);                 // POSTPROTOCOLO
    }
}

int main() {
    // Las dos banderas empiezan a 1: nadie quiere entrar todavía.
    c[0].store(1, memory_order_seq_cst);
    c[1].store(1, memory_order_seq_cst);
    thread p1(proceso, 0);   // el 0 llega a proceso como el parámetro yo
    thread p2(proceso, 1);
    p1.join();   // join: main espera a que termine cada proceso
    p2.join();
    cout << "Entradas en la seccion critica: " << entradas << " de 2\n";
    return 0;   // 0 = el programa ha terminado bien
}

// =====================================================================
// Tema 5.3  |  Seguridad: un invariante que se rompe
// En este programa he movido dinero entre saldo_a y saldo_b con dos hilos mientras un observador cuenta las veces que la suma no vale 2000.
// Requisitos previos: 05/02, 04-hilos-usados-mal (transferencias bancarias rotas)
// Qué observar al ejecutar: el observador ve sumas distintas de 2000 y el total final tampoco cuadra.
// Salida determinista: NO (el número de violaciones cambia, pero siempre aparecen)
// Compilar con: -O0 (target sin optimizar, como todos los ejemplos de carreras)
// =====================================================================
// Invariante: saldo_a + saldo_b == 2000 (el dinero ni se crea ni se destruye).
// Seguridad = "nunca ocurre algo malo". Basta UNA traza que llegue a un
// estado prohibido para demostrar que el programa es inseguro.
// La bandera 'terminado' es atomic<bool> para no mezclar dos problemas:
// aquí solo queremos ver el invariante roto.
// Calibración (16 núcleos, GCC 13.3, -O0, 200000 transferencias por hilo):
// en 6 ejecuciones, entre 28000 y 60000 estados prohibidos observados y un
// total final muy lejos de 2000 (de -182931 a 222477): además del estado
// intermedio, se pierden actualizaciones como en 05/02.
#include <iostream>   // cout
#include <thread>     // thread
#include <atomic>     // atomic, variable que varios hilos pueden leer y escribir sin carrera

using namespace std;

const int TRANSFERENCIAS = 200000;
// Variables globales: todos los hilos ven las mismas (por eso son compartidas).
int saldo_a = 1000;
int saldo_b = 1000;
atomic<bool> terminado(false);   // bandera para avisar al observador de que pare; empieza en false

void de_a_hacia_b() {
    for (int i = 0; i < TRANSFERENCIAS; i++) {
        saldo_a = saldo_a - 1;   // entre esta línea...
        saldo_b = saldo_b + 1;   // ...y esta, la suma vale 1999
    }
}

void de_b_hacia_a() {
    for (int i = 0; i < TRANSFERENCIAS; i++) {
        saldo_b = saldo_b - 1;
        saldo_a = saldo_a + 1;
    }
}

// int& violaciones: escribe en el contador de main, no en una copia.
void observador(int& violaciones) {
    while (!terminado) {   // sigue mirando hasta que main ponga terminado a true
        if (saldo_a + saldo_b != 2000) {   // estado prohibido: se ha roto el invariante
            violaciones++;
        }
    }
}

int main() {
    int violaciones = 0;
    // ref(violaciones): el hilo recibe la variable de main; sin ref recibiría una copia.
    thread vigilante(observador, ref(violaciones));
    thread h1(de_a_hacia_b);
    thread h2(de_b_hacia_a);
    h1.join();   // join: main espera a que acabe cada hilo de transferencias
    h2.join();
    terminado = true;   // ya no hay transferencias: aviso al observador para que salga de su bucle
    vigilante.join();   // y espero a que el observador termine

    cout << "Estados prohibidos observados: " << violaciones << "\n";
    cout << "Total final: " << saldo_a + saldo_b << " (debe ser 2000)\n";
    return 0;   // 0 = el programa ha terminado bien
}

// =====================================================================
// Tema 4.8  |  Referencia colgante: el hilo lee una variable que ya no existe
// En este programa he pasado a un hilo una referencia a dato_local, que muere cuando lanzar_hilo retorna, y el hilo acaba leyendo basura.
// Requisitos previos: 03-hilos-creacion (05_trampa_1_solucion_std_ref), 01-repaso-cpp (tiempo de vida)
// Qué observar al ejecutar: el hilo esperaba leer 42 y lee otro número (basura).
// Salida determinista: NO (comportamiento indefinido: puede salir basura, 42 o incluso reventar)
// Compilar con: -O0 (target sin optimizar; a otro nivel la basura es otra)
// =====================================================================
// 'dato_local' vive en la pila de lanzar_hilo(). Cuando lanzar_hilo()
// retorna, esa memoria queda libre y la siguiente función que se llame la
// reutiliza. El hilo sigue teniendo una referencia a esa dirección: una
// referencia colgante. Es comportamiento indefinido (UB): el estándar no
// dice qué pasa, puede "funcionar" en tu ordenador y fallar en el del profesor.
// El hilo solo LEE: escribir en esa dirección podría corromper la pila de main.
// Calibración (-O0): GCC 13.3, 10 de 10 ejecuciones leen basura (entre 21 955 y 25 925);
// Clang 18, 5 de 5 (en torno a 32 765). Cada compilador deja una basura distinta.
#include <chrono>       // milliseconds
#include <functional>   // cref
#include <iostream>     // cout
#include <thread>       // thread y this_thread
#include <vector>       // vector

using namespace std;

// const int&: no es una copia del número, sino una referencia de solo lectura a la variable original.
void leer_mas_tarde(const int& valor) {
    this_thread::sleep_for(chrono::milliseconds(200));   // cuando lee, la variable ya murió
    cout << "[hilo] esperaba leer 42 y leo " << valor << "\n";
}

// vector<thread>&: referencia al vector de main, para que el hilo quede guardado allí.
void lanzar_hilo(vector<thread>& hilos) {
    int dato_local = 42;   // variable local: vive en la pila de esta función
    hilos.push_back(thread(leer_mas_tarde, cref(dato_local)));   // cref = ref de solo lectura
    cout << "[lanzar_hilo] retorno: dato_local deja de existir\n";
}   // <- aquí muere dato_local, pero el hilo sigue vivo

// Una función cualquiera que usa la pila y reutiliza la memoria que dejó libre dato_local.
int calcular_otra_cosa() {
    int numeros[16];   // array local de 16 enteros: ocupa sitio en la pila
    int suma = 0;
    for (int i = 0; i < 16; i++) {
        numeros[i] = 7000 + i;
        suma = suma + numeros[i];
    }
    return suma;
}

int main() {
    vector<thread> hilos;   // el hilo vive aquí, en main: se podrá hacer join
    lanzar_hilo(hilos);
    cout << "[main] otra cosa: " << calcular_otra_cosa() << "\n";

    hilos[0].join();   // el único hilo está en la posición 0 del vector
    return 0;   // 0 = el programa ha terminado bien
}

// =====================================================================
// Tema 6.4  |  La consola es un recurso compartido: imprimir con candado
// En este programa he protegido cada cout con el candado mtx_consola y he creado la función imprimir, para que 4 hilos no mezclen sus líneas.
// Requisitos previos: 04-hilos-usados-mal (cout entremezclado), 06/02
// Qué observar al ejecutar: todas las líneas salen enteras; el orden entre hilos sigue cambiando (eso no lo arregla el candado).
// Salida determinista: NO (el orden de las líneas cambia; su contenido ya no se mezcla)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// A partir de aquí, en todo el curso, los hilos escriben con imprimir().
// Ojo: el candado protege que cada línea salga ENTERA, no el ORDEN.
#include <iostream>   // cout
#include <string>     // string y to_string
#include <thread>     // thread
#include <mutex>      // mutex
#include <vector>     // vector, para guardar los hilos

using namespace std;

const int NUM_HILOS = 4;
mutex mtx_consola;   // un único candado global para la consola, compartido por todos

// const string& texto: recibe el texto sin copiarlo y sin poder modificarlo.
void imprimir(const string& texto) {
    mtx_consola.lock();     // ---- empieza la sección crítica: nadie más escribe
    cout << texto << "\n";
    mtx_consola.unlock();   // ---- acaba: otro hilo ya puede escribir
}

void saludar(int id) {
    for (int i = 1; i <= 5; i++) {
        // Aunque encadenemos << dentro del candado, nadie puede colarse:
        mtx_consola.lock();
        cout << "Hilo " << id << " escribe su linea " << i << " de 5" << "\n";
        mtx_consola.unlock();
    }
    // to_string pasa el número a texto para poder unirlo con + a las otras partes.
    imprimir("Hilo " + to_string(id) + " ha terminado");
}

int main() {
    vector<thread> hilos;   // vector de hilos: empieza vacío
    for (int i = 0; i < NUM_HILOS; i++) {
        hilos.push_back(thread(saludar, i));   // push_back añade al vector un hilo que ejecuta saludar(i)
    }
    for (int i = 0; i < NUM_HILOS; i++) {
        hilos[i].join();   // join: main espera a cada hilo
    }
    // Aquí ya no quedan hilos, pero uso imprimir igual por costumbre.
    imprimir("Fin: ninguna linea mezclada");
    return 0;   // 0 = el programa ha terminado bien
}

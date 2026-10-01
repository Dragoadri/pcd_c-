// =====================================================================
// Tema 3.3  |  Pasar argumentos a un hilo
// En este programa he lanzado la misma función saludar en dos hilos con argumentos distintos: "Ana" 3 veces y "Luis" 5 veces.
// Requisitos previos: 02_dos_hilos_y_tiempo
// Qué observar al ejecutar: cada hilo usa su propio nombre y su propio número de veces.
// Salida determinista: NO (las líneas de Ana y de Luis se intercalan de forma distinta)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <chrono>     // milliseconds
#include <iostream>   // cout
#include <string>     // string y to_string
#include <thread>     // thread y this_thread

using namespace std;

// const string&: recibo el texto sin copiarlo y sin poder cambiarlo.
void saludar(const string& nombre, int veces) {
    for (int i = 1; i <= veces; i++) {
        // to_string(i) convierte el número en texto para poder unirlo con +.
        // Así toda la línea es un único texto y sale en un solo cout.
        cout << "hola, soy " + nombre + " (" + to_string(i) + ")\n";
        this_thread::sleep_for(chrono::milliseconds(200));   // duerme este hilo 200 ms
    }
}

int main() {
    // Los argumentos van detrás de la función, en el mismo orden que sus parámetros.
    //                      función   nombre  veces
    thread hilo_ana(saludar,  "Ana",  3);
    thread hilo_luis(saludar, "Luis", 5);

    hilo_ana.join();    // espero a que Ana termine
    hilo_luis.join();   // y después a Luis (tarda más: saluda 5 veces)

    cout << "Los dos hilos han terminado\n";
    return 0;   // 0 = el programa ha terminado bien
}

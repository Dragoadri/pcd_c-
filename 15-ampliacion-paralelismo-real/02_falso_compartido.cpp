// =====================================================================
// Tema 15.2  |  Falso compartido: hilos que no comparten datos y aun así se estorban
// En este programa he puesto 8 hilos a contar en casillas vecinas, separadas o en una variable local, para medir cuánto frena el falso compartido.
// Requisitos previos: 01_suma_paralela_speedup
// Qué observar al ejecutar: las casillas contiguas tardan claramente más que las separadas o que la variable local.
// Salida determinista: NO (los tiempos dependen de la máquina y de su carga)
// Compilar con: -O2 (target optimizado; a -O0 el coste de operator[] tapa el efecto)
// =====================================================================
// No hay condición de carrera: cada hilo escribe SOLO en su casilla.
// El problema es la caché del procesador: trabaja por bloques de 64 bytes
// (líneas de caché). 8 long long contiguos caben en la misma línea, así que
// cada escritura de un hilo invalida la copia en caché de todos los demás.
// Medido (8 hilos, i7-11800H): contiguas 0,36 a 0,43 s | separadas 0,13 a 0,19 s.
#include <iostream>     // cout
#include <vector>       // vector
#include <thread>       // thread
#include <chrono>       // steady_clock, duration
#include <functional>   // ref

using namespace std;

const long long N = 200000000LL;   // LL: el número es de tipo long long
const int NUM_HILOS = 8;

// Cuenta múltiplos de 3 escribiendo en memoria compartida en CADA vuelta.
// vector<long long>& casillas: todos los hilos reciben el MISMO vector; cada uno usa su casilla.
void contar_en_casilla(int casilla, vector<long long>& casillas) {
    for (long long i = 0; i < N; i++) {
        if (i % 3 == 0) {           // %: resto de dividir entre 3; 0 = múltiplo de 3
            casillas[casilla]++;    // escribe en el vector compartido en cada vuelta: aquí está el frenazo
        }
    }
}

// Lo mismo, pero acumulando en una variable local y escribiendo una sola vez.
void contar_en_local(int casilla, vector<long long>& casillas) {
    long long cuenta = 0;           // local: vive en la pila de este hilo, lejos de los demás
    for (long long i = 0; i < N; i++) {
        if (i % 3 == 0) {
            cuenta++;
        }
    }
    casillas[casilla] = cuenta;     // una sola escritura en el vector, al final
}

// separacion = 1: casillas 0,1,2... (vecinas). separacion = 16: 0,16,32... (128 bytes entre ellas).
double medir(int separacion, bool usar_local) {
    chrono::steady_clock::time_point inicio = chrono::steady_clock::now();   // instante de inicio
    vector<long long> casillas(NUM_HILOS * separacion, 0);   // con separacion 16 el vector es 16 veces más grande
    vector<thread> hilos;                                    // vector de hilos, vacío al principio
    for (int i = 0; i < NUM_HILOS; i++) {
        // ref(casillas): pasa el vector por referencia; sin ref cada hilo recibiría su propia copia.
        if (usar_local) {
            hilos.push_back(thread(contar_en_local, i * separacion, ref(casillas)));
        } else {
            hilos.push_back(thread(contar_en_casilla, i * separacion, ref(casillas)));
        }
    }
    for (int i = 0; i < NUM_HILOS; i++) {
        hilos[i].join();            // espera a que termine cada hilo antes de parar el reloj
    }
    // Restar dos instantes da una duración en segundos; count() da ese número como double.
    chrono::duration<double> segundos = chrono::steady_clock::now() - inicio;
    return segundos.count();
}

int main() {
    // Se repite 3 veces para ver cuánto varían los tiempos de una medida a otra.
    for (int repeticion = 1; repeticion <= 3; repeticion++) {
        cout << "Repeticion " << repeticion << ":\n";
        cout << "  casillas contiguas: " << medir(1, false) << " s\n";
        cout << "  casillas separadas: " << medir(16, false) << " s\n";
        cout << "  variable local:     " << medir(1, true) << " s\n";
    }
    return 0;   // 0 = el programa ha terminado bien
}

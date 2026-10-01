// =====================================================================
// Tema 15.1  |  Paralelismo real: repartir un cálculo entre núcleos y medir la aceleración
// En este programa he repartido una suma muy larga entre 1, 2, 4... hilos con sumar_trozo y he medido con steady_clock cuánto acelera cada reparto.
// Requisitos previos: 03-hilos-creacion (ref, vector de hilos), 02-fundamentos-multiprogramacion
// Qué observar al ejecutar: el tiempo baja casi a la mitad con 2 y 4 hilos, se frena al llegar a los núcleos físicos y no mejora con más.
// Salida determinista: NO (los tiempos dependen de la máquina y de su carga; el total calculado sí es siempre el mismo)
// Compilar con: -O2 (target optimizado: medimos rendimiento, no carreras)
// =====================================================================
// Medido en un i7-11800H (8 núcleos físicos, hardware_concurrency = 16),
// con N = 2 000 000 000: 1 hilo 2,66 s | 2 hilos 1,25 s | 4 hilos 0,63 s |
// 8 hilos 0,47 s | 16 hilos 0,37 s | 32 y 64 hilos 0,35 s.
// Con 16 no llega a x16: la mitad son hilos "hyperthreading", no núcleos reales.
//
// Contraste con el Tema 4 original: el mismo reparto en Python con threading
// tardaba MÁS que en secuencial por el GIL (ver NOTA_GIL_PYTHON.md).
#include <iostream>     // cout
#include <iomanip>      // fixed, setprecision, setw
#include <vector>       // vector
#include <thread>       // thread
#include <chrono>       // steady_clock, duration
#include <functional>   // ref

using namespace std;

const long long N = 1000000000LL;   // mil millones de sumandos (LL: el número es de tipo long long)

// Trabajo de CPU puro: no espera a nada, solo calcula.
// long long& resultado: es la casilla del vector de quien llama; se escribe "hacia fuera".
void sumar_trozo(long long desde, long long hasta, long long& resultado) {
    long long suma = 0;                 // local: ningún otro hilo la toca
    for (long long i = desde; i < hasta; i++) {
        suma += (i * i) % 1000003;      // %: resto de la división; solo sirve para dar trabajo a la CPU
    }
    resultado = suma;                   // una sola escritura, en SU casilla
}

double medir_con_hilos(int num_hilos, long long& total) {
    // steady_clock: reloj que solo avanza hacia delante; now() da el instante actual.
    chrono::steady_clock::time_point inicio = chrono::steady_clock::now();
    vector<long long> resultados(num_hilos, 0);   // una casilla por hilo, todas a 0
    vector<thread> hilos;                         // vector de hilos, vacío al principio
    long long tamanio_trozo = N / num_hilos;      // división entera: puede sobrar un resto
    for (int i = 0; i < num_hilos; i++) {
        long long desde = i * tamanio_trozo;
        long long hasta = desde + tamanio_trozo;
        if (i == num_hilos - 1) {
            hasta = N;                  // el último trozo se lleva el resto de la división
        }
        // push_back añade el hilo al vector. ref(resultados[i]): pasa la casilla por referencia;
        // sin ref, el hilo escribiría en una copia y la casilla seguiría a 0.
        hilos.push_back(thread(sumar_trozo, desde, hasta, ref(resultados[i])));
    }
    for (int i = 0; i < num_hilos; i++) {
        hilos[i].join();                // espera a que cada hilo termine ANTES de sumar sus resultados
    }
    total = 0;
    for (int i = 0; i < num_hilos; i++) {
        total += resultados[i];
    }
    // Restar dos instantes da una duración; duration<double> la guarda en segundos con decimales.
    chrono::duration<double> segundos = chrono::steady_clock::now() - inicio;
    return segundos.count();            // count(): el número de segundos como double
}

int main() {
    int nucleos = thread::hardware_concurrency();   // hilos hardware; 0 si no lo sabe
    if (nucleos == 0) {
        nucleos = 2;
    }
    // fixed y setprecision(2): a partir de aquí los double salen con 2 decimales.
    cout << "hardware_concurrency: " << nucleos << "\n" << fixed << setprecision(2);

    vector<int> pruebas = {1, 2, 4, nucleos, 2 * nucleos, 4 * nucleos};   // cuántos hilos se prueban en cada vuelta
    double tiempo_un_hilo = 0;
    for (int i = 0; i < (int) pruebas.size(); i++) {   // cast (int): size() devuelve un número sin signo
        long long total = 0;
        double segundos = medir_con_hilos(pruebas[i], total);   // total vuelve relleno por referencia
        if (i == 0) {
            tiempo_un_hilo = segundos;  // la primera prueba es con 1 hilo: es la referencia
        }
        // setw(3): el número ocupa 3 posiciones, para que las columnas salgan alineadas.
        // Aceleración = tiempo con 1 hilo / tiempo con estos hilos.
        cout << setw(3) << pruebas[i] << " hilos: " << segundos << " s  aceleracion x"
                  << tiempo_un_hilo / segundos << "  (total = " << total << ")\n";
    }
    return 0;   // 0 = el programa ha terminado bien
}

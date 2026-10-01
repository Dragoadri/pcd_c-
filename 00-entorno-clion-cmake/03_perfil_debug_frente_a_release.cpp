// =====================================================================
// Tema 0.3  |  Perfiles de compilación: Debug frente a Release
// En este programa he detectado con #ifdef si se compiló en Debug o Release y he cronometrado un bucle de 300 millones de sumas.
// Requisitos previos: 01_hola_mundo.cpp
// Qué observar al ejecutar: cambia de perfil en CLion (Debug o Release) y compara mensajes y tiempo.
// Salida determinista: NO (el tiempo medido depende de la máquina y del perfil)
// Compilar con: el del perfil elegido (este target NO fuerza nivel)
// =====================================================================
#include <chrono>     // steady_clock, milliseconds y duration_cast, para medir tiempo
#include <iostream>   // cout

using namespace std;

int main() {
// Las líneas que empiezan por # las procesa el preprocesador ANTES de compilar:
// #ifdef X deja pasar el primer bloque solo si X está definido; si no, el de #else.
// Por eso van pegadas al margen izquierdo: no son instrucciones normales.
#ifdef NDEBUG
    cout << "NDEBUG definido: perfil Release (los assert se desactivan)\n";
#else
    cout << "NDEBUG sin definir: perfil Debug\n";
#endif

// __OPTIMIZE__ lo define el compilador cuando se le pide optimizar (-O1 o más).
#ifdef __OPTIMIZE__
    cout << "Compilado CON optimizacion (-O1, -O2 o -O3)\n";
#else
    cout << "Compilado SIN optimizacion (-O0)\n";
#endif

    // steady_clock::now() da el instante actual; lo guardo para restar al final.
    chrono::steady_clock::time_point inicio = chrono::steady_clock::now();
    long long suma = 0;   // long long: entero grande, porque la suma no cabe en un int
    // 300000000LL: la LL final indica que el literal es long long.
    for (long long i = 0; i < 300000000LL; i++) {
        suma = suma + i;
    }
    chrono::steady_clock::time_point fin = chrono::steady_clock::now();
    // fin - inicio es una duración; duration_cast la convierte a milisegundos.
    chrono::milliseconds duracion =
        chrono::duration_cast<chrono::milliseconds>(fin - inicio);

    // .count() saca el número de milisegundos como un entero normal para imprimirlo.
    cout << "Suma = " << suma << " en " << duracion.count() << " ms\n";

    // Con -O2 el compilador puede sustituir el bucle entero por una fórmula.
    // Por eso los ejemplos de condiciones de carrera (tema 04) se fuerzan a -O0:
    // si el bucle desaparece, la carrera también, y parece que no hay error.
    return 0;   // 0 = el programa ha terminado bien
}

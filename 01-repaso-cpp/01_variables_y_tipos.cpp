// =====================================================================
// Tema 1.1  |  Variables y tipos básicos
// En este programa he declarado variables int, double, char, bool, string y una constante, las he impreso y he cambiado el valor de edad.
// Requisitos previos: 00-entorno-clion-cmake
// Qué observar al ejecutar: cada variable con su valor; el bool sale como 1 o 0.
// Salida determinista: SI (no hay hilos ni aleatoriedad)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <iostream>   // cout, para escribir en pantalla
#include <string>     // string, el tipo para guardar texto

using namespace std;   // así escribo cout y string en vez de std::cout y std::string

int main() {
    // tipo nombre = valor;   El tipo no cambia nunca después.
    // Cada instrucción termina en punto y coma (;).
    int edad = 20;              // número entero
    double nota = 7.5;          // número con decimales (punto, no coma)
    char letra = 'A';           // UN carácter, comillas simples
    bool aprobado = true;       // true o false
    string nombre = "Ana";      // texto, comillas dobles

    // const: una vez dado el valor, no se puede cambiar.
    // Por costumbre, las constantes se escriben en MAYÚSCULAS.
    const int NUM_HILOS = 4;
    // NUM_HILOS = 8;           // <- descomenta: error de compilación

    // cout << "texto" << variable: se pueden encadenar tantos << como haga falta.
    // "\n" es el salto de línea: sin él, todo saldría en la misma línea.
    cout << "edad: " << edad << "\n";
    cout << "nota: " << nota << "\n";
    cout << "letra: " << letra << "\n";
    cout << "aprobado: " << aprobado << "   (true se imprime como 1)\n";
    cout << "nombre: " << nombre << "\n";
    cout << "NUM_HILOS: " << NUM_HILOS << "\n";

    // Una variable se puede reasignar las veces que haga falta.
    // Se lee de derecha a izquierda: calcula edad + 1 y guarda el resultado en edad.
    edad = edad + 1;
    cout << "edad tras cumplir: " << edad << "\n";
    return 0;   // 0 = el programa ha terminado bien
}

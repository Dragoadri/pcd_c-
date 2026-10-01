// =====================================================================
// Tema 1.10  |  vector y string
// En este programa he llenado el vector metros con push_back, lo he duplicado por referencia y he montado cada mensaje en un string con to_string.
// Requisitos previos: 08_paso_por_referencia_y_puntero.cpp
// Qué observar al ejecutar: el vector se modifica por referencia y cada línea se imprime de una vez.
// Salida determinista: SI (no hay hilos ni aleatoriedad)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <iostream>   // cout
#include <string>     // string y to_string
#include <vector>     // vector, una lista de elementos del mismo tipo que puede crecer

using namespace std;

// vector<int>& : la función modifica el vector original (sin copiarlo).
void duplicar_todos(vector<int>& numeros) {
    // numeros.size() dice cuántos elementos hay. Devuelve un entero sin signo,
    // así que (int) lo convierte a int para compararlo con i sin avisos del compilador.
    for (int i = 0; i < (int) numeros.size(); i++) {
        // numeros[i] es el elemento de la posición i; la primera posición es la 0.
        numeros[i] = numeros[i] * 2;
    }
}

int main() {
    // Entre < > va el tipo de lo que guarda el vector.
    vector<int> metros;   // vector vacío
    metros.push_back(4);        // push_back añade al final
    metros.push_back(7);
    metros.push_back(1);        // ahora metros = {4, 7, 1}
    duplicar_todos(metros);    // ahora metros = {8, 14, 2}

    // Sin struct: varios datos por elemento = vectores paralelos
    // (la posición i de cada vector habla del mismo elemento).
    vector<string> nombres = {"Ana", "Luis", "Eva"};   // vector creado ya con 3 textos

    for (int i = 0; i < (int) metros.size(); i++) {
        // to_string convierte un número en texto; + une cadenas.
        string linea = nombres[i] + " lleva " + to_string(metros[i]) + " metros\n";
        cout << linea;  // UNA sola escritura con la línea completa
    }

    // Vector con tamaño inicial: 3 posiciones a 0. Así daremos a cada
    // hilo "su" casilla para dejar un resultado (tema 03).
    // Ojo: (3, 0) con paréntesis = 3 casillas valiendo 0; {3, 0} con llaves serían los números 3 y 0.
    vector<int> resultados(3, 0);
    cout << "resultados tiene " << resultados.size() << " casillas\n";
    return 0;   // 0 = el programa ha terminado bien
}

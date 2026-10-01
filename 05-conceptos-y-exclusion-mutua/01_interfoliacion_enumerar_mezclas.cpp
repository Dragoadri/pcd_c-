// =====================================================================
// Tema 5.1  |  Interfoliación: todas las mezclas posibles de dos hilos
// En este programa he generado con la función recursiva generar las 20 mezclas posibles de dos hilos de 3 instrucciones.
// Requisitos previos: 04-hilos-usados-mal (haber visto resultados distintos en cada ejecución)
// Qué observar al ejecutar: las 20 mezclas; dentro de cada hilo el orden se respeta (A1 antes que A2), entre hilos no.
// Salida determinista: SI (aquí no hay hilos: enumeramos las mezclas, no las ejecutamos)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Analogía: barajar dos mazos. Cada mazo conserva su orden interno, pero
// cada barajada produce una mezcla distinta. El planificador del sistema
// operativo "baraja" las instrucciones de los hilos.
// Número de mezclas = (n1 + n2)! / (n1! * n2!)   (coeficiente multinomial)
#include <iostream>   // cout
#include <string>     // string y to_string
#include <vector>     // vector, una lista que crece según añadimos cosas

using namespace std;

// Genera recursivamente todas las mezclas: en cada paso, o avanza el hilo A
// o avanza el hilo B, siempre que le queden instrucciones.
// const string& prefijo: recibe el texto sin copiarlo y sin poder cambiarlo.
// vector<string>& mezclas: por referencia (&), así lo que añadimos aquí se
// guarda en el vector de main y no en una copia.
void generar(int usadas_a, int usadas_b, int total, const string& prefijo,
             vector<string>& mezclas) {
    // Caso base: los dos hilos han gastado todas sus instrucciones.
    if (usadas_a == total && usadas_b == total) {
        mezclas.push_back(prefijo);   // push_back añade la mezcla al final del vector
    } else {
        if (usadas_a < total) {
            // Probamos que ahora ejecute A. to_string convierte el número en texto.
            generar(usadas_a + 1, usadas_b, total,
                    prefijo + "A" + to_string(usadas_a + 1) + " ", mezclas);
        }
        if (usadas_b < total) {
            // Y también probamos que ahora ejecute B: así salen las dos ramas.
            generar(usadas_a, usadas_b + 1, total,
                    prefijo + "B" + to_string(usadas_b + 1) + " ", mezclas);
        }
    }
}

// unsigned long long: entero muy grande y sin signo, porque 15! no cabe en un int.
unsigned long long factorial(int n) {
    unsigned long long resultado = 1;
    for (int i = 2; i <= n; i++) {
        resultado = resultado * i;
    }
    return resultado;
}

int main() {
    const int INSTRUCCIONES = 3;
    vector<string> mezclas;   // vector vacío que irá llenando generar
    generar(0, 0, INSTRUCCIONES, "", mezclas);   // empezamos sin instrucciones usadas y con el texto vacío

    // size() dice cuántos elementos tiene el vector; (int) lo convierte a int
    // para compararlo con i sin aviso del compilador.
    for (int i = 0; i < (int) mezclas.size(); i++) {
        cout << "Mezcla " << i + 1 << ": " << mezclas[i] << "\n";   // mezclas[i]: el elemento en la posición i
    }

    unsigned long long formula = factorial(6) / (factorial(3) * factorial(3));
    cout << "\nMezclas generadas: " << mezclas.size()
              << "   Formula 6!/(3!*3!) = " << formula << "\n";

    // Tres hilos de 5 instrucciones: 15! / (5! * 5! * 5!)
    unsigned long long tres_hilos = factorial(15) / (factorial(5) * factorial(5) * factorial(5));
    cout << "Con 3 hilos de 5 instrucciones: " << tres_hilos
              << " mezclas. No se prueban a mano: se razonan.\n";
    return 0;   // 0 = el programa ha terminado bien
}

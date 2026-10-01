// =====================================================================
// Tema 1.2  |  Operaciones aritméticas
// En este programa he operado con a = 7 y b = 2 para ver la división entera, el resto %, el cast a double, la precedencia y += o ++.
// Requisitos previos: 01_variables_y_tipos.cpp
// Qué observar al ejecutar: 7 / 2 da 3, no 3.5; el paréntesis cambia el resultado.
// Salida determinista: SI (no hay hilos ni aleatoriedad)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <iostream>   // cout

using namespace std;

int main() {
    int a = 7;
    int b = 2;

    // La operación se calcula primero y cout imprime el resultado.
    cout << "a + b = " << a + b << "\n";
    cout << "a - b = " << a - b << "\n";
    cout << "a * b = " << a * b << "\n";
    cout << "a / b = " << a / b << "   <- entre enteros: division ENTERA, se pierde el .5\n";
    // % (módulo) da el resto: 7 % 2 = 1 porque 7 = 2 * 3 + 1.
    cout << "a % b = " << a % b << "   <- resto de la division (sirve para par o impar)\n";

    // Para tener decimales, al menos uno de los dos debe ser double.
    // (double) a es un cast: convierte a en 7.0 solo para esta cuenta; a sigue siendo int.
    double media = (double) a / b;
    cout << "(double) a / b = " << media << "\n";

    // Primero * y /, después + y -. Los paréntesis mandan.
    cout << "2 + 3 * 4 = " << 2 + 3 * 4 << "\n";
    cout << "(2 + 3) * 4 = " << (2 + 3) * 4 << "\n";

    // Formas cortas: hacen lo mismo que la versión larga del comentario.
    int contador = 10;
    contador += 5;      // contador = contador + 5;     (vale 15)
    contador -= 3;      // contador = contador - 3;     (vale 12)
    contador *= 2;      // contador = contador * 2;     (vale 24)
    contador++;         // contador = contador + 1;     (vale 25)
    cout << "contador = " << contador << "\n";

    // Guardad esta línea para el tema 05: contador++ PARECE una sola
    // operación, pero la máquina hace tres pasos (leer, sumar, guardar).
    return 0;   // 0 = el programa ha terminado bien
}

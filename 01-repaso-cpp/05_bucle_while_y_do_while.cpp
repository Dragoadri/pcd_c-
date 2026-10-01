// =====================================================================
// Tema 1.5  |  Bucles while y do-while, y salir con una bandera
// En este programa he duplicado con while hasta pasar de 1000, he visto que do-while entra una vez y he parado una búsqueda con la bandera encontrado.
// Requisitos previos: 04_bucle_for.cpp
// Qué observar al ejecutar: el do-while entra una vez aunque la condición sea falsa.
// Salida determinista: SI (no hay hilos ni aleatoriedad)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <iostream>   // cout

using namespace std;

int main() {
    // while: no sabemos cuántas vueltas. Comprueba ANTES de entrar.
    int cantidad = 1;
    int pasos = 0;
    while (cantidad <= 1000) {       // se repite mientras la condición sea cierta
        cantidad = cantidad * 2;     // cambia la variable de la condición: así el bucle acaba
        pasos++;                     // pasos = pasos + 1;
    }
    cout << "Duplicando desde 1, supero 1000 en " << pasos << " pasos (" << cantidad << ")\n";

    // do-while: comprueba DESPUÉS, así que entra al menos una vez.
    int intentos = 10;
    do {
        cout << "Dentro del do-while con intentos = " << intentos << "\n";
        intentos++;
    } while (intentos < 5);     // falsa desde el principio y aun así entró (ojo al ; final)

    // Salir antes de tiempo SIN break: una bandera en la condición.
    // Buscamos el primer múltiplo de 7 mayor que 50.
    int numero = 51;
    bool encontrado = false;   // bandera: empieza en false porque aún no lo he encontrado
    while (!encontrado) {
        encontrado = (numero++ % 7 == 0); // mismo resultado
        // se lee "mientras NO encontrado"
        if (numero % 7 == 0) {                // resto 0 al dividir entre 7: es múltiplo de 7
            encontrado = true;          // la próxima comprobación sale del bucle
        } else {
            numero++;                         // no lo es: pruebo el siguiente
        }
    }
    cout << "Primer multiplo de 7 mayor que 50: " << numero << "\n";

    // Si se olvida actualizar la variable de la condición, el bucle no
    // termina nunca. En CLion se para con el botón rojo.
    return 0;   // 0 = el programa ha terminado bien
}

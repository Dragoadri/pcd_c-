// =====================================================================
// Tema 1.6  |  Funciones libres: declarar, definir y llamar
// En este programa he declarado arriba saludar y calcular_cuadrado, las he llamado desde main y he escrito su cuerpo debajo.
// Requisitos previos: 05_bucle_while_y_do_while.cpp
// Qué observar al ejecutar: cada función hace una sola tarea y main las encadena.
// Salida determinista: SI (no hay hilos ni aleatoriedad)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <iostream>   // cout
#include <string>     // string

using namespace std;

// Declaración (prototipo): el compilador tiene que conocer la función
// antes de que se use. El cuerpo puede ir más abajo.
// Se lee: tipo_que_devuelve nombre(parámetros);   void = no devuelve nada.
// const string& : recibe el texto sin copiarlo (&) y sin poder cambiarlo (const).
void saludar(const string& nombre);
int calcular_cuadrado(int numero);   // recibe un int y devuelve un int

int main() {
    // Llamar a una función: su nombre y, entre paréntesis, el valor de cada parámetro.
    saludar("Ana");
    saludar("Luis");

    // La llamada se sustituye por lo que devuelve (49) y eso se guarda en resultado.
    int resultado = calcular_cuadrado(7);
    cout << "El cuadrado de 7 es " << resultado << "\n";
    return 0;   // 0 = el programa ha terminado bien
}

// Definición: el cuerpo de cada función.
void saludar(const string& nombre) {
    cout << "Hola, " << nombre << "\n";
}

int calcular_cuadrado(int numero) {
    return numero * numero;   // return: termina la función y entrega este valor a quien llamó
}

// Para el tema 03: un hilo ejecuta UNA FUNCIÓN. Si sabes escribir una
// función libre, sabes escribir el trabajo de un hilo.

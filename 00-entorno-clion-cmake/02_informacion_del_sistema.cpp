// =====================================================================
// Tema 0.2  |  Información del sistema y del compilador
// En este programa he impreso la versión del compilador, el estándar de C++, los núcleos disponibles y el id del hilo principal.
// Requisitos previos: 01_hola_mundo.cpp
// Qué observar al ejecutar: versión del compilador, estándar 202002 y el id del hilo principal.
// Salida determinista: NO (el id del hilo y los núcleos cambian según la máquina)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Adaptado del Anexo A de la Actividad 0. Versión solo Linux: se han quitado
// los #if de Windows y macOS porque la asignatura fija Linux (sockets y fork).
#include <iostream>   // cout
#include <string>     // string, para guardar texto
#include <thread>     // thread::hardware_concurrency y this_thread::get_id

using namespace std;

int main() {
    string alumno = "Nombre Apellido Apellido";  // escribe aquí tu nombre

    cout << "Alumno: " << alumno << "\n";
    cout << "Sistema operativo: Linux\n";

    // __VERSION__ la define el compilador (GCC y Clang) al compilar.
    cout << "Compilador: " << __VERSION__ << "\n";

    // __cplusplus vale 202002 cuando se compila en C++20.
    cout << "Estandar de C++: " << __cplusplus << "\n";

    // Núcleos lógicos que el sistema pone a nuestra disposición.
    // thread:: delante significa "la función hardware_concurrency que pertenece a thread".
    // La instrucción sigue en la línea de abajo: termina en el punto y coma.
    cout << "Procesadores disponibles: "
              << thread::hardware_concurrency() << "\n";

    // Todo programa ya tiene un hilo antes de crear ninguno: el principal.
    // this_thread::get_id() devuelve el identificador del hilo que ejecuta esta línea.
    cout << "Hilo en ejecucion: " << this_thread::get_id() << "\n";

    return 0;   // 0 = el programa ha terminado bien
}

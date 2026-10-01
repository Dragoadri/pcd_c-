// =====================================================================
// Tema 14.6  |  Varios hilos escribiendo en la misma base de datos
// En este programa he puesto 4 hilos a insertar 50 filas cada uno en hilos.db, primero sin busy_timeout y luego con 5000 ms, para ver los bloqueos.
// Requisitos previos: 01_sqlite_crear_insertar_consultar, 06-mutex, 07-semaforos
// Qué observar al ejecutar: sin espera fallan casi todas las inserciones (database is locked); con busy_timeout entran las 200.
// Salida determinista: NO (cuántas fallan sin espera depende del entrelazado de los hilos)
// Compilar con: cualquier nivel
// =====================================================================
// Medido en clase (4 hilos x 50 inserciones): sin busy_timeout se guardan
// entre 1 y 34 filas de 200 en cada ejecución; con busy_timeout, 200 siempre.
//
// SQLite bloquea el FICHERO entero mientras alguien escribe. Es una sección
// crítica que protege la propia biblioteca: el segundo escritor recibe
// SQLITE_BUSY ("database is locked") salvo que le digamos que espere.
//
// Una conexión POR HILO. ¿Y compartir una sola? Con SQLITE_THREADSAFE=1
// SQLite serializa las llamadas con su propio mutex y no se corrompe, pero
// sqlite3_errmsg o last_insert_rowid pueden devolver lo que hizo OTRO hilo:
// es el mismo problema que compartir cout. Mejor no compartirla.
#include <iostream>    // cout
#include <string>      // string, to_string
#include <vector>      // vector
#include <thread>      // thread
#include <mutex>       // mutex
#include <sqlite3.h>   // funciones sqlite3_* de SQLite

using namespace std;

const int NUM_HILOS = 4;
const int INSERCIONES_POR_HILO = 50;
mutex mtx_consola;   // protege cout: solo un hilo imprime a la vez

void imprimir(const string& texto) {
    mtx_consola.lock();     // empieza la sección crítica: los demás hilos esperan aquí
    cout << texto << "\n";
    mtx_consola.unlock();   // acaba la sección crítica: puede entrar otro hilo
}

// Cada hilo abre SU conexión y apunta sus fallos en SU casilla del vector.
// int& fallos: referencia a esa casilla; lo que sume aquí se ve fuera, en ronda.
void escribir_registros(int id_hilo, int espera_ms, int& fallos) {
    sqlite3* conexion = nullptr;
    sqlite3_open("hilos.db", &conexion);         // &conexion: open rellena el puntero de ESTE hilo
    // busy_timeout: si la base está bloqueada, SQLite reintenta durante espera_ms milisegundos antes de fallar.
    sqlite3_busy_timeout(conexion, espera_ms);   // 0 = no esperar nada
    for (int i = 0; i < INSERCIONES_POR_HILO; i++) {
        // to_string convierte un número en texto para pegarlo dentro del SQL.
        string sql = "INSERT INTO registros (hilo, numero) VALUES (" +
                          to_string(id_hilo) + ", " + to_string(i) + ")";
        // exec devuelve algo distinto de SQLITE_OK si falla, por ejemplo con "database is locked".
        if (sqlite3_exec(conexion, sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
            if (fallos == 0) {   // solo se imprime el primer fallo de cada hilo, para no llenar la pantalla
                imprimir("  hilo " + to_string(id_hilo) + ": " + sqlite3_errmsg(conexion));   // errmsg: texto del error
            }
            fallos++;            // cada hilo suma en SU casilla: no hay condición de carrera
        }
    }
    sqlite3_close(conexion);
}

void ronda(sqlite3* conexion, int espera_ms) {
    sqlite3_exec(conexion, "DELETE FROM registros", nullptr, nullptr, nullptr);   // vacía la tabla antes de la ronda
    vector<int> fallos(NUM_HILOS, 0);   // NUM_HILOS casillas, todas a 0: una por hilo
    vector<thread> hilos;               // vector de hilos, vacío al principio
    for (int i = 0; i < NUM_HILOS; i++) {
        // push_back añade el hilo al vector. ref(fallos[i]): pasa la casilla por referencia;
        // sin ref, el hilo trabajaría sobre una copia y aquí seguiríamos viendo 0.
        hilos.push_back(thread(escribir_registros, i, espera_ms, ref(fallos[i])));
    }
    for (int i = 0; i < NUM_HILOS; i++) {
        hilos[i].join();                // espera a que termine cada hilo antes de contar
    }
    int total_fallos = 0;
    for (int i = 0; i < NUM_HILOS; i++) {
        total_fallos += fallos[i];
    }
    sqlite3_stmt* sentencia = nullptr;
    sqlite3_prepare_v2(conexion, "SELECT COUNT(*) FROM registros", -1, &sentencia, nullptr);
    sqlite3_step(sentencia);            // COUNT(*) devuelve una sola fila
    cout << "  Filas guardadas: " << sqlite3_column_int(sentencia, 0) << " de "
              << NUM_HILOS * INSERCIONES_POR_HILO << ", fallos: " << total_fallos << "\n";
    sqlite3_finalize(sentencia);        // libera la sentencia
}

int main() {
    sqlite3* conexion = nullptr;        // conexión de main, distinta de las de los hilos
    sqlite3_open("hilos.db", &conexion);
    sqlite3_exec(conexion, "CREATE TABLE IF NOT EXISTS registros (hilo INTEGER, numero INTEGER)",
                 nullptr, nullptr, nullptr);

    cout << "Ronda 1: sin busy_timeout (el que encuentra la base bloqueada, falla)\n";
    ronda(conexion, 0);                 // espera_ms = 0
    cout << "Ronda 2: busy_timeout de 5000 ms (espera su turno y reintenta)\n";
    ronda(conexion, 5000);

    sqlite3_close(conexion);
    return 0;   // 0 = el programa ha terminado bien
}

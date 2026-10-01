// =====================================================================
// Tema 14.3  |  Consulta preparada con parámetros: la solución a la inyección
// En este programa he rehecho el login con huecos ? y sqlite3_bind_text, para que el ataque ' OR '1'='1 ya no consiga entrar.
// Requisitos previos: 02_inyeccion_sql_concatenando
// Qué observar al ejecutar: el mismo ataque ' OR '1'='1 ahora es denegado; la clave correcta sigue entrando.
// Salida determinista: SI
// Compilar con: cualquier nivel
// =====================================================================
#include <iostream>    // cout
#include <string>      // string
#include <sqlite3.h>   // funciones sqlite3_* de SQLite

using namespace std;

// BIEN: el SQL es fijo y lleva huecos '?'. Los datos van aparte, con bind.
bool login_preparado(sqlite3* conexion, const string& nombre, const string& clave) {
    const char* sql = "SELECT COUNT(*) FROM usuarios WHERE nombre = ? AND clave = ?";   // const char*: texto fijo al estilo C

    sqlite3_stmt* sentencia = nullptr;
    sqlite3_prepare_v2(conexion, sql, -1, &sentencia, nullptr);   // compila el SQL con los huecos aún vacíos
    // Los huecos se numeran desde 1. SQLITE_TRANSIENT: SQLite se hace su copia del texto.
    // Lo que escriba el usuario es SIEMPRE un valor, jamás parte de la sentencia.
    // c_str(): el texto como char*. -1: la longitud la calcula SQLite hasta el final del texto.
    sqlite3_bind_text(sentencia, 1, nombre.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(sentencia, 2, clave.c_str(), -1, SQLITE_TRANSIENT);

    int coincidencias = 0;
    if (sqlite3_step(sentencia) == SQLITE_ROW) {           // ejecuta la consulta: COUNT(*) da una fila
        coincidencias = sqlite3_column_int(sentencia, 0);  // columna 0 de esa fila, como int
    }
    sqlite3_finalize(sentencia);   // libera la sentencia
    return coincidencias > 0;      // true si algún usuario coincide
}

void probar(sqlite3* conexion, const string& nombre, const string& clave) {
    cout << "Intento con clave [" << clave << "]: ";
    if (login_preparado(conexion, nombre, clave)) {
        cout << "ACCESO CONCEDIDO\n";
    } else {
        cout << "acceso denegado\n";
    }
}

int main() {
    sqlite3* conexion = nullptr;
    sqlite3_open("usuarios_juguete.db", &conexion);   // &conexion: open rellena nuestro puntero
    // Varias sentencias separadas por ';'. Los nullptr: sin filas y sin mensaje de error.
    sqlite3_exec(conexion, "CREATE TABLE IF NOT EXISTS usuarios (nombre TEXT, clave TEXT);"
                           "DELETE FROM usuarios;"
                           "INSERT INTO usuarios VALUES ('admin', 'S3creta!');",
                 nullptr, nullptr, nullptr);

    probar(conexion, "admin", "S3creta!");
    probar(conexion, "admin", "1234");
    probar(conexion, "admin", "' OR '1'='1");   // se busca literalmente esa "clave": no existe

    sqlite3_close(conexion);   // cierra la base de datos
    return 0;                  // 0 = el programa ha terminado bien
}

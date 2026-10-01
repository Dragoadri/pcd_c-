# 14-n-capas: cómo se ha metido SQLite en el proyecto

En Python, `import sqlite3` funciona sin más porque SQLite viene dentro del intérprete (así lo contaba el Tema 3). **En C++ no viene nada**: ni la biblioteca ni la cabecera. Para que nadie tenga que instalar paquetes en su ordenador, el proyecto trae SQLite dentro y lo compila como una parte más.

## Qué es la amalgamación

SQLite está escrito en C y repartido en cientos de ficheros. Sus autores publican además una versión "amalgamada": **todo SQLite en un solo `sqlite3.c`** (unos 9 MB de código) y su cabecera `sqlite3.h`. Compilar ese fichero da exactamente la misma biblioteca, sin dependencias externas.

| Dato | Valor |
|---|---|
| Versión incluida | SQLite 3.53.4 |
| Fecha de publicación | 24 de julio de 2026 |
| Fichero descargado | `sqlite-amalgamation-3530400.zip` |
| Origen | https://sqlite.org/download.html (sección *Source Code*) |
| SHA3-256 del zip | `628a44cfe82c66aed1ccbbe85a562d2e33ebe64b3288981ed76285612227934e` (coincide con el publicado en la web) |
| Ficheros copiados | `sqlite/sqlite3.c` y `sqlite/sqlite3.h` (el resto del zip no hace falta) |
| Licencia | Dominio público |

## Cómo se enlaza en CMake

En `CMakeLists.txt` de esta carpeta:

```cmake
add_library(sqlite3_amalgamacion STATIC sqlite/sqlite3.c)
target_include_directories(sqlite3_amalgamacion PUBLIC sqlite)
target_compile_definitions(sqlite3_amalgamacion PRIVATE
    SQLITE_THREADSAFE=1
    SQLITE_OMIT_LOAD_EXTENSION)
target_link_libraries(sqlite3_amalgamacion PUBLIC Threads::Threads)

pcd_ejemplo(01_sqlite_crear_insertar_consultar)
target_link_libraries(01_sqlite_crear_insertar_consultar PRIVATE sqlite3_amalgamacion)
```

Qué hace cada línea:

1. `add_library(... STATIC ...)`: compila `sqlite3.c` una sola vez y lo empaqueta como biblioteca estática. La primera compilación tarda unos segundos; después CMake no lo vuelve a compilar.
2. `target_include_directories(... PUBLIC sqlite)`: quien enlace con la biblioteca puede escribir `#include <sqlite3.h>` sin rutas.
3. `SQLITE_THREADSAFE=1`: modo serializado, se puede llamar a SQLite desde varios hilos (lo usamos en el ejemplo 06).
4. `SQLITE_OMIT_LOAD_EXTENSION`: quita la carga de extensiones dinámicas, que obligaría a enlazar además con `libdl`.
5. `target_link_libraries(<ejemplo> PRIVATE sqlite3_amalgamacion)`: cada ejemplo usa la biblioteca.

El proyecto **no** usa el SQLite que pueda tener instalado el sistema: así todos compilan exactamente la misma versión.

## Para usarlo en otro proyecto (por ejemplo, la práctica final)

1. Copia la carpeta `sqlite/` a tu proyecto.
2. Copia las cuatro primeras órdenes de arriba (`add_library` hasta `target_link_libraries(sqlite3_amalgamacion ...)`).
3. Añade `target_link_libraries(tu_programa PRIVATE sqlite3_amalgamacion)`.
4. Asegúrate de que el `project()` declara el lenguaje C: `project(MiProyecto LANGUAGES C CXX)`, porque `sqlite3.c` es código C.

Para actualizar de versión: descarga el zip nuevo de la misma página y sustituye los dos ficheros.

## Dónde aparece el fichero `.db`

`sqlite3_open("tareas.db", ...)` usa una ruta relativa, así que el fichero se crea en el **directorio de trabajo** del programa, que no tiene por qué ser la carpeta del código fuente:

* Desde terminal, en la carpeta desde la que lances el ejecutable (si haces `cd build/bin && ./05_tareas_en_tres_capas`, aparece en `build/bin`).
* Desde CLion, en el directorio que indique la configuración de ejecución (*Run > Edit Configurations > Working directory*).

No lo supongas: el ejemplo 01 imprime la ruta con `filesystem::current_path()`. Si "desaparecen" las tareas guardadas, casi siempre es que se ha ejecutado desde otra carpeta y se ha creado otro `.db` vacío.

Para ver el contenido de un `.db` sin programar se puede usar la extensión de base de datos de CLion (*Database*, arrastrando el fichero) o el programa `sqlite3` de terminal si está instalado.

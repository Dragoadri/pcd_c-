# =====================================================================
# 14-n-capas: aplicaciones en N capas con SQLite
#
# SQLite NO viene con C++. En lugar de pedir que cada alumno lo instale,
# el proyecto incluye la "amalgamación" oficial (un único sqlite3.c y su
# sqlite3.h) dentro de sqlite/ y la compila como una biblioteca más.
# Detalles en README.md de esta carpeta.
# =====================================================================

# Biblioteca estática con todo SQLite. Se compila una vez y la enlazan
# todos los ejemplos. Sin -Wall: es código C de terceros.
add_library(sqlite3_amalgamacion STATIC sqlite/sqlite3.c)
target_include_directories(sqlite3_amalgamacion PUBLIC sqlite)
# SQLITE_THREADSAFE=1: modo "serializado", se puede usar desde varios hilos.
# SQLITE_OMIT_LOAD_EXTENSION: sin carga de extensiones dinámicas, así no
# hace falta enlazar con libdl.
target_compile_definitions(sqlite3_amalgamacion PRIVATE
    SQLITE_THREADSAFE=1
    SQLITE_OMIT_LOAD_EXTENSION)
target_link_libraries(sqlite3_amalgamacion PUBLIC Threads::Threads)

# Ejemplos, en el orden en que se escriben en clase
pcd_ejemplo(01_sqlite_crear_insertar_consultar)
pcd_ejemplo(02_inyeccion_sql_concatenando)
pcd_ejemplo(03_consulta_preparada_con_bind)
pcd_ejemplo(04_tareas_todo_mezclado)
pcd_ejemplo(05_tareas_en_tres_capas)
pcd_ejemplo(06_hilos_escriben_en_sqlite)

# Todos enlazan con SQLite
target_link_libraries(01_sqlite_crear_insertar_consultar PRIVATE sqlite3_amalgamacion)
target_link_libraries(02_inyeccion_sql_concatenando PRIVATE sqlite3_amalgamacion)
target_link_libraries(03_consulta_preparada_con_bind PRIVATE sqlite3_amalgamacion)
target_link_libraries(04_tareas_todo_mezclado PRIVATE sqlite3_amalgamacion)
target_link_libraries(05_tareas_en_tres_capas PRIVATE sqlite3_amalgamacion)
target_link_libraries(06_hilos_escriben_en_sqlite PRIVATE sqlite3_amalgamacion)

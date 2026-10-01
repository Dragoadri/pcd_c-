# =====================================================================
# 99-practica-final-whatsapp  |  Enunciado de la práctica final
#
# Esta carpeta NO contiene código: solo el enunciado (ENUNCIADO.md), la
# rúbrica (RUBRICA.md) y el guion del profesor (GUION.md). Por eso no
# declara ningún target; existe para que el add_subdirectory del
# CMakeLists.txt raíz funcione igual que en el resto de temas.
#
# Los alumnos crean su PROPIO proyecto de CLion, fuera de este curso.
# CMakeLists.txt mínimo de referencia para su proyecto, con la
# amalgamación de SQLite dentro del propio proyecto (mismo enfoque que
# la carpeta 14-n-capas, así no hay que instalar nada):
#
#   cmake_minimum_required(VERSION 3.22)
#   project(WhatsAppPCD LANGUAGES C CXX)
#
#   set(CMAKE_CXX_STANDARD 20)
#   set(CMAKE_CXX_STANDARD_REQUIRED ON)
#   set(CMAKE_CXX_EXTENSIONS OFF)
#
#   find_package(Threads REQUIRED)
#
#   # sqlite/sqlite3.c y sqlite/sqlite3.h copiados de la amalgamación
#   add_library(sqlite3_amalgamacion STATIC sqlite/sqlite3.c)
#   target_include_directories(sqlite3_amalgamacion PUBLIC sqlite)
#   target_compile_definitions(sqlite3_amalgamacion PRIVATE
#       SQLITE_THREADSAFE=1 SQLITE_OMIT_LOAD_EXTENSION)
#   target_link_libraries(sqlite3_amalgamacion PUBLIC Threads::Threads)
#
#   add_executable(servidor
#       servidor/servidor_main.cpp
#       servidor/capa_presentacion.cpp
#       servidor/capa_negocio.cpp
#       servidor/capa_datos.cpp)
#   target_link_libraries(servidor PRIVATE sqlite3_amalgamacion Threads::Threads)
#   target_compile_options(servidor PRIVATE -Wall -Wextra)
#
#   add_executable(cliente cliente/cliente_main.cpp)
#   target_link_libraries(cliente PRIVATE Threads::Threads)
#   target_compile_options(cliente PRIVATE -Wall -Wextra)
#
#   add_executable(lanzador_clientes pruebas/lanzador_clientes.cpp)
#   target_link_libraries(lanzador_clientes PRIVATE Threads::Threads)
# =====================================================================

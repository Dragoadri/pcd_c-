# =====================================================================
# Programación Concurrente y Distribuida (C++20)  |  Proyecto raíz
#
# Abre ESTA carpeta (curso-pcd-cpp) como proyecto en CLion. Cada fichero
# .cpp de cada tema es un ejecutable independiente: aparece con su propio
# nombre en el desplegable de configuraciones de CLion.
#
# Desde terminal:
#   cmake -S . -B build
#   cmake --build build --target 01_hola_mundo
#   ./build/bin/01_hola_mundo
#
# Todos los ejecutables quedan en build/bin (o cmake-build-debug/bin en
# CLion), así es fácil lanzar servidor y cliente en dos terminales.
# =====================================================================
cmake_minimum_required(VERSION 3.22)

# C para compilar la amalgamación de SQLite (tema 14), C++ para todo lo demás.
project(CursoPCD LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Si nadie elige tipo de compilación, Debug (sin optimizar y con símbolos).
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Tipo de compilacion" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Biblioteca de hilos del sistema (pthread en Linux).
find_package(Threads REQUIRED)

# ---------------------------------------------------------------------
# Funciones de ayuda que usan los CMakeLists.txt de cada carpeta.
# El nombre del target es el nombre del fichero sin ".cpp".
# Si un ejemplo necesita más ficheros fuente, se añaden detrás del nombre.
# ---------------------------------------------------------------------

# Ejemplo normal: respeta el nivel de optimización del perfil (Debug/Release).
function(pcd_ejemplo nombre)
    add_executable(${nombre} ${nombre}.cpp ${ARGN})
    target_link_libraries(${nombre} PRIVATE Threads::Threads)
    target_compile_options(${nombre} PRIVATE -Wall -Wextra)
endfunction()

# Ejemplo compilado SIEMPRE a -O0, aunque el perfil sea Release.
# Se usa en los ejemplos de condiciones de carrera: con -O2 el compilador
# puede convertir un bucle de un millón de incrementos en una sola suma y
# la carrera deja de verse. La opción -O0 va detrás de las del perfil,
# y en GCC y Clang gana la última -O que aparece.
function(pcd_ejemplo_sin_optimizar nombre)
    pcd_ejemplo(${nombre} ${ARGN})
    target_compile_options(${nombre} PRIVATE -O0)
endfunction()

# Ejemplo compilado SIEMPRE a -O2: medidas de rendimiento y el ejemplo
# que demuestra cómo la optimización esconde una carrera.
function(pcd_ejemplo_optimizado nombre)
    pcd_ejemplo(${nombre} ${ARGN})
    target_compile_options(${nombre} PRIVATE -O2)
endfunction()

# ---------------------------------------------------------------------
# Compilar solo una carpeta (útil en clase para que CMake vaya rápido):
#   cmake -S . -B build -DPCD_SOLO=06-mutex
# Vacío = todas las carpetas.
# ---------------------------------------------------------------------
set(PCD_SOLO "" CACHE STRING "Compilar solo esta carpeta (vacio = todas)")

if(PCD_SOLO)
    add_subdirectory(${PCD_SOLO})
else()
    add_subdirectory(00-entorno-clion-cmake)
    add_subdirectory(01-repaso-cpp)
    add_subdirectory(02-fundamentos-multiprogramacion)
    add_subdirectory(03-hilos-creacion)
    add_subdirectory(04-hilos-usados-mal)
    add_subdirectory(05-conceptos-y-exclusion-mutua)
    add_subdirectory(06-mutex)
    add_subdirectory(07-semaforos)
    add_subdirectory(08-barreras)
    add_subdirectory(09-repaso-parcial-1)
    add_subdirectory(10-fundamentos-de-redes)
    add_subdirectory(11-sockets-udp)
    add_subdirectory(12-sockets-tcp)
    add_subdirectory(13-servidores-concurrentes)
    add_subdirectory(14-n-capas)
    add_subdirectory(15-ampliacion-paralelismo-real)
    add_subdirectory(16-ampliacion-rpc-distribucion)
    add_subdirectory(99-practica-final-whatsapp)
endif()

# Programación Concurrente y Distribuida en C++20

Material de clase de la asignatura (2º curso, Universidad Europea de Madrid), grupo que la cursa en C++ con CMake y CLion.

El objetivo que atraviesa toda la asignatura es el salto de la ejecución **lineal** a la ejecución **concurrente**: dejar de poder predecir cuándo se ejecuta algo y en qué estado estarán los datos cuando se ejecute. Por eso el orden del curso es siempre el mismo: primero se ve fallar, luego se le pone nombre, y solo al final se enseña la herramienta que lo arregla.

## Cómo usar este proyecto

**En CLion:** *File > Open* sobre esta carpeta (`curso-pcd-cpp`), no sobre la carpeta padre. Al cargar CMake, cada fichero `.cpp` aparece como una configuración propia en el desplegable, con el mismo nombre que el fichero. Se elige y se pulsa *Run*.

**En terminal:**

```bash
cmake -S . -B build                      # configurar (Debug por defecto)
cmake --build build --target 01_primer_hilo
./build/bin/01_primer_hilo

cmake -S . -B build -DPCD_SOLO=06-mutex  # compilar solo una carpeta (más rápido en clase)
```

Todos los ejecutables quedan juntos en `build/bin/` (en CLion, `cmake-build-debug/bin/`), así es sencillo lanzar servidor y cliente en dos terminales.

**Requisitos:** Linux, GCC 11 o superior (o Clang con libstdc++), CMake 3.22 o superior. Verificado con GCC 13.3, Clang 18.1 y CMake 3.28 en Ubuntu 24.04.

> **Aviso de plataforma.** La Actividad 0 admite Windows (MinGW de CLion) y macOS. Hasta la carpeta 10 todo es C++ estándar, salvo la carpeta 08 (`barrier` no existe en macOS ni en MSVC) y los `taskset` de algunas demos. Desde la carpeta 10 los ejemplos usan sockets POSIX y `fork()`, que **no compilan con MinGW**. Quien use Windows necesita WSL2, una máquina virtual con Linux o los ordenadores del laboratorio. Conviene pedirlo antes de llegar a redes.

### Funciones de CMake

Cada carpeta tiene un `CMakeLists.txt` con una línea por ejemplo:

| Función | Optimización | Se usa en |
|---|---|---|
| `pcd_ejemplo(nombre)` | la del perfil (Debug o Release) | casi todo |
| `pcd_ejemplo_sin_optimizar(nombre)` | siempre `-O0` | condiciones de carrera: con `-O2` el compilador puede convertir un millón de incrementos en una sola suma y la carrera deja de verse |
| `pcd_ejemplo_optimizado(nombre)` | siempre `-O2` | medidas de rendimiento y la demostración de que la optimización esconde la carrera |

Para añadir un ejemplo: crear el `.cpp` en la carpeta, añadir su línea en el `CMakeLists.txt` de esa carpeta y recargar CMake.

## Norma de código de la asignatura

Se cumple en todo el material, incluidos los ejemplos del profesor:

- Prohibidos `break`, `continue` y, por consecuencia, `switch` (se sustituye por `if / else if / else`). Los bucles se controlan con su condición y banderas booleanas.
- Prohibido definir clases o structs propios: código procedimental con funciones libres, variables sueltas y `vector` paralelos. Los tipos de la librería estándar sí se usan.
- Candados con `mtx.lock()` y `mtx.unlock()` a mano, nunca `lock_guard` ni `scoped_lock`, para ver dónde empieza y acaba la sección crítica. Única excepción: `unique_lock` para `condition_variable::wait`, comentada donde aparece.
- Todo hilo creado se une con `join()`. Nada de `detach()` ni `jthread`.
- `using namespace std;` en todos los ficheros, justo debajo de los `#include`, y nunca `std::` en el código. Única precaución: la llamada POSIX `bind` de los servidores se escribe `::bind`, porque con el `using` el compilador elegiría el `bind` de la librería estándar (está comentado donde aparece).
- Ningún ejemplo se queda colgado: los bucles infinitos y los servidores del material heredado están acotados (número de clientes, tiempo de inactividad, intentos) y el cambio está comentado.
- Todo en español. Los textos que se imprimen van sin tildes ni eñes (la consola de Windows no siempre muestra UTF-8); los comentarios, con tildes.

## Formato de cada carpeta

- **Ficheros numerados** `01_nombre_descriptivo.cpp` en el orden en que se teclean en clase, uno por concepto. Las parejas problema y solución van consecutivas y con nombres emparejados (por ejemplo `08_taquilla_cine_sin_mutex.cpp` y `09_taquilla_cine_con_mutex.cpp`).
- **Cabecera** en cada fichero con tema, una línea en primera persona que cuenta lo que hace el programa ("En este programa he..."), requisitos previos, qué observar al ejecutar, si la salida es determinista y con qué optimización compilar. Debajo, cuando hace falta, las cifras de calibración medidas.
- **Comentarios en las líneas que un alumno puede no entender**: cada `#include`, cada llamada de la librería estándar, de sockets o de SQLite la primera vez que aparece, la sintaxis de referencias y punteros, y dónde empieza y acaba cada sección crítica.
- **`GUION.md`** con lo que se escribe en la pizarra, la secuencia de ficheros con minutos, las preguntas a la clase antes de ejecutar (con la respuesta esperada), los errores típicos y el enlace con la carpeta siguiente.
- En sockets, servidor y cliente van en ficheros separados y el GUION explica cómo lanzarlos en dos terminales y en CLion.

## Recorrido completo

Duraciones en minutos de clase, suponiendo sesiones de 2 horas con unos 110 minutos útiles.

### Arranque

| Carpeta | Contenido | `.cpp` | Min | Examen |
|---|---|---|---|---|
| [00-entorno-clion-cmake](00-entorno-clion-cmake/GUION.md) | Sesión 0: proyecto CMake en CLion, un ejecutable por ejemplo, perfiles Debug y Release, terminal, dónde ver la salida | 3 | 90 | No (Actividad 0) |
| [01-repaso-cpp](01-repaso-cpp/GUION.md) | Parte A, lo básico: variables y tipos, operaciones aritméticas, condicionales, `for`, `while` y `do-while`. Parte B, lo imprescindible para concurrencia: funciones libres, valor frente a referencia y puntero, ámbito y tiempo de vida, vector y string, consola, el mismo ejercicio con `for` y con `while`, `<random>`, `<chrono>` y `sleep_for`, carrera de tortugas lineal | 15 | 150 | Base del Parcial 1 |

### Bloque 1 (Parcial 1)

| Carpeta | Contenido | `.cpp` | Min | Examen |
|---|---|---|---|---|
| [02-fundamentos-multiprogramacion](02-fundamentos-multiprogramacion/GUION.md) | Autómata frente a ordenador, Von Neumann, estados de un proceso, planificador y cambio de contexto, por qué con un núcleo no hay paralelismo real (resumido, previa de Sistemas Operativos) | 5 | 105 | Vocabulario |
| [03-hilos-creacion](03-hilos-creacion/GUION.md) | `thread` y `join`, argumentos, **trampa 1: los argumentos se copian** (y `ref`), vector de hilos, orden no garantizado, tortugas con hilos | 8 | 110 | Sí |
| [04-hilos-usados-mal](04-hilos-usados-mal/GUION.md) | Los hilos usados mal a propósito: `cout` entremezclado, contador roto, sin `ref` "parece que funciona", transferencias que descuadran, coordinar con `sleep`, espera activa que satura la CPU, referencia colgante, la optimización esconde la carrera | 9 | 110 | Sí (síntomas) |
| [05-conceptos-y-exclusion-mutua](05-conceptos-y-exclusion-mutua/GUION.md) | Se pone nombre a lo visto: interfoliación, atomicidad, seguridad e invariantes, vivacidad, equidad, inanición, Bernstein. Intentos de exclusión mutua y Peterson como lectura ([LECTURA_cuatro_intentos.md](05-conceptos-y-exclusion-mutua/LECTURA_cuatro_intentos.md)) | 7 | 110 | Vocabulario sí, intentos no |
| [06-mutex](06-mutex/GUION.md) | `mutex` a mano: contador, sección crítica demasiado grande, consola protegida, `unlock` olvidado, interbloqueo con dos candados y su solución, taquilla de cine | 9 | 110 | Sí |
| [07-semaforos](07-semaforos/GUION.md) | `counting_semaphore` y `binary_semaphore`, `condition_variable`, semáforo artesanal (un contador protegido), productor y consumidor, filósofos con camarero | 8 | 200 | Sí |
| [08-barreras](08-barreras/GUION.md) | **Clase invertida** por equipos y sesión de cierre del profesor: `latch`, barrera artesanal, `barrier` de libstdc++ (no estándar) | 5 | 180 | No (se permite usarlas) |
| [09-repaso-parcial-1](09-repaso-parcial-1/GUION.md) | Tres ejercicios tipo examen en pareja enunciado y solución, y [SIMULACRO_PARCIAL_1.md](09-repaso-parcial-1/SIMULACRO_PARCIAL_1.md) con solucionario | 6 | 220 | Repaso |

**Parcial 1:** creación de hilos, candados y semáforos (carpetas 03 a 07, con el vocabulario de 02 y 05). Las barreras quedan fuera aunque se permite usarlas.

### Bloque 2 (Parcial 2)

| Carpeta | Contenido | `.cpp` | Min | Examen |
|---|---|---|---|---|
| [10-fundamentos-de-redes](10-fundamentos-de-redes/GUION.md) | IP, máscara, localhost, puerto, DNS, NAT, orden de bytes ([APUNTES_REDES.md](10-fundamentos-de-redes/APUNTES_REDES.md)) | 3 | 110 | Sí |
| [11-sockets-udp](11-sockets-udp/GUION.md) | Eco UDP y datagramas sin conexión ni garantías | 4 | 110 | Sí |
| [12-sockets-tcp](12-sockets-tcp/GUION.md) | Eco TCP, TCP es un flujo (protocolo por líneas), servidor secuencial que hace esperar al segundo cliente; tabla UDP frente a TCP | 6 | 165 | Sí |
| [13-servidores-concurrentes](13-servidores-concurrentes/GUION.md) | Un hilo por conexión, lanzador de clientes concurrentes, contador de visitas sin y con candado, límite de conexiones con semáforo, chat de difusión | 7 | 220 | Sí |
| [14-n-capas](14-n-capas/GUION.md) | SQLite con la amalgamación dentro del proyecto ([README](14-n-capas/README.md)), inyección SQL frente a consulta preparada, aplicación de tareas mezclada frente a tres capas, varios hilos contra SQLite | 6 | 150 | Sí |

**Parcial 2:** carpetas 10 a 14.

### Ampliación (no entra en examen)

| Carpeta | Contenido | `.cpp` | Min |
|---|---|---|---|
| [15-ampliacion-paralelismo-real](15-ampliacion-paralelismo-real/GUION.md) | Paralelismo real en C++: aceleración medida, falso compartido, `async` y futures, `fork()` y `waitpid`. Sustituye al Tema 4 del GIL, que queda en [NOTA_GIL_PYTHON.md](15-ampliacion-paralelismo-real/NOTA_GIL_PYTHON.md) (una página) | 6 | 110 |
| [16-ampliacion-rpc-distribucion](16-ampliacion-rpc-distribucion/GUION.md) | RPC con stubs sobre TCP, maestro que reparte trabajo entre trabajadores remotos. En dos años nunca ha dado tiempo a llegar: preparada aparte | 4 | 110 |

### Práctica final

[99-practica-final-whatsapp](99-practica-final-whatsapp/ENUNCIADO.md): un WhatsApp de consola con servidor TCP concurrente, varios clientes y persistencia en SQLite. Se evalúa en tres tramos acumulativos (máximo 5, 7 y 10) según la [RUBRICA.md](99-practica-final-whatsapp/RUBRICA.md), con defensa oral individual y uso de IA declarado. Se presenta al empezar el bloque de redes (unos 50 minutos), tiene tres seguimientos de 30 minutos, se entrega antes de Navidad y se reutiliza en Proyectos de Informática II. La carpeta contiene el enunciado y la rúbrica, no la solución.

### Resumen de tiempo

| Tramo | Carpetas | Minutos | Sesiones aprox. |
|---|---|---|---|
| Arranque | 00 y 01 | 240 | 2,5 |
| Bloque 1 | 02 a 09 | 1145 | 10,5 + examen |
| Bloque 2 | 10 a 14 | 755 | 7 + examen |
| Ampliación | 15 y 16 | 220 | 2 |
| Práctica | presentación y seguimientos | 140 | repartidos en el bloque 2 |

## Avisos técnicos que conviene conocer antes de clase

- **`thread` copia sus argumentos.** Sin `ref` el hilo trabaja sobre una copia y el ejemplo de la carrera "funciona". Es la trampa 1 (`03-hilos-creacion/04` y `04-hilos-usados-mal/03`).
- **Las carreras dependen de la optimización y de los núcleos.** Medido en un i7 de 16 hilos con GCC 13.3: 2 hilos con 10⁶ incrementos cada uno a `-O0` pierden alrededor del 45 % en todas las ejecuciones. A `-O2` casi siempre sale el valor correcto (1 fallo en 250 ejecuciones). Con un solo núcleo (`taskset -c 0`, o una máquina virtual con 1 vCPU) falla solo alrededor del 14 % de las veces. El error está ahí igual.
- **`barrier` no es C++20 estándar.** Se retiró de la propuesta antes de publicarse. libstdc++ trae una implementación propia que funciona con GCC y con Clang en Linux, pero no existe en macOS ni en MSVC. `latch` sí es estándar.
- **Los intentos clásicos de exclusión mutua** se implementan con `atomic` y `memory_order_seq_cst`. Con `int` normales el compilador puede sacar lecturas del bucle de espera o reordenar escrituras, porque la teoría supone una memoria secuencialmente consistente que el hardware no da gratis.
- **`fork()` duplica el búfer de `cout`** si no se vacía antes (`flush`).

## Documentos de referencia

- [MAPA.md](MAPA.md): trazabilidad de cada fichero con la transparencia o unidad de origen, y el material heredado que se ha descartado y por qué.

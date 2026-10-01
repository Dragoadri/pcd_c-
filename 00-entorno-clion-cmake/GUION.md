# Guion de clase: 00 entorno CLion y CMake

**Duración estimada:** 90 minutos (1 sesión de 2 horas; el resto, resolver instalaciones)
**Entra en examen:** No (es requisito para todo lo demás y se evalúa con la Actividad 0)

## Objetivo de la sesión

Que todos salgan con el proyecto `curso-pcd-cpp` abierto en CLion, sabiendo compilar y ejecutar cualquier ejemplo desde el desplegable y desde el terminal, y sabiendo dónde mirar la salida. Nadie escribe todavía un hilo.

## Antes de empezar (preparación del profesor)

- Tener el proyecto abierto en CLion con el perfil Debug cargado y un perfil Release añadido (Settings, Build Execution Deployment, CMake, botón +).
- Comprobar que tras compilar aparece `cmake-build-debug/bin` con los ejecutables.
- Tener a mano el Anexo B de la Actividad 0 (problemas frecuentes).
- **Aviso de sistema operativo.** La Actividad 0 admite Windows (MinGW incluido en CLion) y macOS. Hasta el tema 10 casi todo compila en los tres (salvo `barrier` en macOS, tema 08). Desde el tema 11 (sockets POSIX) y en el 15 (`fork`) el código es **solo Linux**: en Windows hay que usar WSL (CLion admite una toolchain WSL) o los ordenadores del laboratorio. Decirlo hoy, no la semana del parcial.

## Pizarra

1. La cadena de herramientas con flechas: **CLion** (editor) → **CMake** (lee `CMakeLists.txt` y prepara la compilación) → **g++ o clang++** (traduce el `.cpp` a binario) → **ejecutable** en `bin/`.
2. El árbol: `curso-pcd-cpp/CMakeLists.txt` raíz con un `add_subdirectory(...)` por tema, y en cada carpeta un `CMakeLists.txt` con **una línea por fichero**: `pcd_ejemplo(01_hola_mundo)`.
3. La regla: **un fichero .cpp = un ejecutable = una entrada en el desplegable**. El ejecutable se llama como el fichero sin `.cpp`.
4. Las tres funciones: `pcd_ejemplo` (normal), `pcd_ejemplo_sin_optimizar` (carreras, siempre `-O0`), `pcd_ejemplo_optimizado` (medir rendimiento, siempre `-O2`).
5. Perfiles: **Debug** (sin optimizar, para depurar) y **Release** (optimizado).
6. En una esquina que se queda todo el cuatrimestre:

```
cmake -S . -B build
cmake --build build --target 01_hola_mundo
./build/bin/01_hola_mundo
cmake -S . -B build -DPCD_SOLO=06-mutex     (compilar solo un tema)
```

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 0 | (abrir el proyecto) | 25 | File, Open, carpeta `curso-pcd-cpp` (la raíz, no un tema). Esperar a que termine la ventana CMake. Si falla: Settings, Toolchains, y después Tools, CMake, Reset Cache and Reload Project. Enseñar el desplegable de configuraciones, el martillo (compilar) y el play (compilar y ejecutar). |
| 1 | `01_hola_mundo.cpp` | 15 | Ejecutar desde el desplegable. Ventana Run: la salida normal, la línea de `cerr` en rojo y "Process finished with exit code 0". Explicar que 0 significa "todo bien" y que cuando un programa aborte veremos 134. Repetir en el terminal integrado con los comandos de la pizarra y `echo $?`: es el mismo binario. |
| 2 | `02_informacion_del_sistema.cpp` | 15 | Cada uno pone su nombre. `__VERSION__` y `__cplusplus` (202002 significa C++20). Comparamos cuántos núcleos tiene cada portátil. La última línea es el hilo principal: ya existe antes de que creemos ninguno, lo retomamos en el tema 03. |
| 3 | `03_perfil_debug_frente_a_release.cpp` | 15 | Ejecutar en Debug, cambiar el perfil a Release y volver a ejecutar. Comparar mensajes y tiempos. Mensaje clave: optimizar cambia el código que se ejecuta, y por eso los ejemplos de condiciones de carrera se fuerzan a `-O0`. |
| 4 | (añadir un ejemplo propio) | 10 | Crear `04_mi_prueba.cpp` en esta carpeta, copiar la cabecera de otro fichero, añadir `pcd_ejemplo(04_mi_prueba)` al `CMakeLists.txt` de la carpeta, Reload CMake Project y comprobar que aparece en el desplegable. Borrarlo al acabar. |
| 5 | Instalaciones pendientes | 10 | Repasar con quien tenga problemas el Anexo B de la Actividad 0. |

## Preguntas a la clase antes de ejecutar

- **01:** "¿Dónde está el ejecutable que acaba de crear CLion?" Respuesta: en `cmake-build-debug/bin/01_hola_mundo`. Abrirlo desde el explorador de archivos para que lo vean.
- **02:** "¿Cuántos hilos tiene este programa si no hemos creado ninguno?" Respuesta: uno, el principal.
- **03:** "¿Tardará lo mismo en Debug que en Release?" Respuesta: no; en Release muchísimo menos, porque el compilador transforma el bucle.

## Errores típicos de los alumnos

- Abrir la carpeta de un tema en vez de la raíz `curso-pcd-cpp`: CMake no conoce `pcd_ejemplo` y da error.
- Crear el `.cpp` y no añadir su línea al `CMakeLists.txt`, o añadirla y no pulsar Reload: no aparece en el desplegable.
- Repetir un nombre de fichero que ya existe en otra carpeta: CMake avisa de que el target ya existe. Los nombres son únicos en todo el proyecto.
- En Linux o macOS, instalar CLion antes que el compilador: no aparece ninguna toolchain.
- Buscar la salida en la ventana Build (la de compilación) en lugar de en la ventana Run.

## Cierre y enlace con la siguiente carpeta

Todos deben poder ejecutar cualquier ejemplo por su nombre. La carpeta `01-repaso-cpp` repasa primero lo básico (tipos, aritmética, condicionales y bucles) y después el C++ que necesitamos para hilos: funciones, valor frente a referencia, tiempo de vida, vector y string, entrada por consola, aleatoriedad y tiempo, y el mismo ejercicio con for y con while.

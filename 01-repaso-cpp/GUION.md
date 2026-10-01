# Guion de clase: 01 repaso de C++

**Duración estimada:** 150 minutos (1 sesión y media de 2 horas): parte A, 45 min; parte B, 105 min
**Entra en examen:** No directamente, pero todo lo de aquí se usa en el Parcial 1

## Objetivo de la sesión

**Parte A (ficheros 01 a 05), lo básico:** que todos partan del mismo suelo con variables y tipos, operaciones aritméticas (con la trampa de la división entera), condicionales y los tres bucles. Si el grupo lo domina, se da rápido o se deja como tarea con el `repaso.cpp` general.

**Parte B (ficheros 06 a 15), lo imprescindible para concurrencia:** dos ideas que con hilos se pagan caras, **qué se copia y qué se comparte** (valor, referencia, puntero) y **cuánto tiempo vive una variable**. Además, que vean explícitamente que un `for` y un `while` son el mismo bucle, y que se sale de un bucle con una bandera, nunca con `break`.

## Antes de empezar (preparación del profesor)

- Recordar la norma en voz alta y dejarla escrita: sin `break`, sin `continue`, sin `switch`, sin clases ni structs propios, y `using namespace std;` debajo de los `#include`.
- Tener preparado CLion con la carpeta `01-repaso-cpp` desplegada.

## Pizarra

Parte A:

1. **Tabla de tipos**: `int` (entero), `double` (decimales), `char` (un carácter, comillas simples), `bool` (true o false), `string` (texto, comillas dobles).
2. **La división entera**: `7 / 2 = 3` y `7 % 2 = 1`, al lado `(double) 7 / 2 = 3.5`.
3. **Anatomía del for**: `for (inicio; condición; actualización)` con una flecha que indica el orden en que se ejecuta cada parte en cada vuelta.
4. **while frente a do-while**: el while pregunta antes de entrar (puede no entrar nunca); el do-while pregunta al salir (entra al menos una vez).

Parte B:

5. **Tres cajas** para el paso de parámetros. Dibujar la variable `saldo = 100` en `main`:
   - Por valor: una flecha que sale de `saldo` y crea **otra caja** `numero = 100` dentro de la función (fotocopia).
   - Por referencia: la caja `saldo` con **dos etiquetas** colgando, `saldo` y `numero` (mismo sitio, dos nombres).
   - Por puntero: una caja `puntero` que contiene una dirección y una flecha hacia `saldo`.
6. **Línea de tiempo** de una función: se entra, nace `local`; se sale, muere `local`. Encima, una barra larga que dura todo el programa para la global y la `static`.
7. **Tabla for/while** con tres filas: (1) inicialización, (2) condición, (3) actualización, y en qué lugar del código va cada una en cada bucle.
8. La **plantilla de bucle con bandera**: `bool terminado = false; while (condicion && !terminado) { ... if (...) { terminado = true; } }`.
9. Aparte, la frase que se usará todo el curso: **"thread copia los argumentos"**. Se deja escrita sin explicar.

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| **A** | **Parte A: lo básico** | | |
| 1 | `01_variables_y_tipos.cpp` | 8 | Tipo, nombre y valor. Comillas simples frente a dobles. `const` y descomentar la línea marcada para ver el error. El `bool` sale como 1. |
| 2 | `02_operaciones_aritmeticas.cpp` | 10 | Antes de ejecutar, que digan cuánto da `7 / 2`. División entera y resto; `(double)` para decimales. Precedencia y paréntesis. `+=` y `++` como forma corta, y dejar caer que `contador++` son tres pasos. |
| 3 | `03_condicionales.cpp` | 9 | Cadena `if / else if / else`: solo entra en la primera que se cumpla, por eso el orden de las notas importa. `&&`, `||` y `!`. Por qué no `switch`. El error de `=` frente a `==`. |
| 4 | `04_bucle_for.cpp` | 9 | Contar las vueltas con los dedos: `i` va de 0 a 4. Acumulador, contador con `if` dentro y bucles anidados. |
| 5 | `05_bucle_while_y_do_while.cpp` | 9 | While cuando no sabemos cuántas vueltas. El do-while entra aunque la condición sea falsa. Primera bandera para salir sin `break`. |
| **B** | **Parte B: lo imprescindible para concurrencia** | | |
| 6 | `06_funciones_libres.cpp` | 8 | Prototipo arriba, definición abajo. Una función, una tarea. Un hilo ejecutará una función como estas. |
| 7 | `07_paso_por_valor.cpp` | 8 | La función recibe una fotocopia. Antes de ejecutar, pregunta a la clase. Señalar la frase de la pizarra: thread también copia. |
| 8 | `08_paso_por_referencia_y_puntero.cpp` | 12 | `int&` es otro nombre de la misma variable; `int*` guarda la dirección. Dibujar las tres cajas. Llamada con `saldo` frente a `&saldo`. |
| 9 | `09_ambito_y_tiempo_de_vida.cpp` | 12 | Local, static y global. Descomentar en directo una de las líneas marcadas para que vean el error de compilación y volver a comentarla. Anunciar la referencia colgante del tema 04. |
| 10 | `10_vector_y_string.cpp` | 12 | `push_back`, `size`, acceso con `[]`, paso por referencia. Vectores paralelos en lugar de struct. Construir la línea completa con `to_string` y `+` e imprimirla de una vez (lo usaremos con hilos). |
| 11 | `11_entrada_por_consola.cpp` | 8 | `if (cin >> leido)`. Probar escribiendo letras. Nunca dejar un programa colgado esperando una entrada que no llega. |
| 12 | `12_mismo_ejercicio_con_for.cpp` | 10 | Dictar el enunciado. Resolverlo con `for` señalando (1), (2) y (3). |
| 13 | `13_mismo_ejercicio_con_while.cpp` | 12 | Mismo enunciado, **sin mirar el anterior**: pedir a la clase dónde va cada pieza. Ejecutar los dos y comprobar que la salida es idéntica. Borrar el `i++` en directo para que vean el bucle infinito (parar con el botón rojo) y restaurarlo. |
| 14 | `14_aleatoriedad_y_tiempo.cpp` | 10 | `random_device`, `mt19937`, `uniform_int_distribution`. `steady_clock` y `sleep_for`. Las dos reglas para hilos del final del fichero. |
| 15 | `15_carrera_tortugas_lineal.cpp` | 13 | Port del ejemplo lineal del Tema 1. La bandera `hay_ganadora` sustituye al `break`. Cerrar con la pregunta: ¿y si cada tortuga corriera a la vez? |

## Preguntas a la clase antes de ejecutar

- **02:** "¿Cuánto da `7 / 2`? ¿Y `7 % 2`?" Respuesta: 3 y 1, porque entre enteros la división es entera. "¿Y `2 + 3 * 4`?" Respuesta: 14, primero la multiplicación.
- **03:** "Con nota 6.5, ¿qué imprime la cadena de if?" Respuesta: "Aprobado". "¿Y con 9.5?" Respuesta: solo "Sobresaliente", aunque también cumple `>= 7` y `>= 5`: la cadena se detiene en la primera condición cierta. "¿Y si pongo primero `nota >= 5`?" Respuesta: un 9.5 saldría "Aprobado", el orden importa.
- **04:** "¿Cuántas vueltas da `for (int i = 0; i < 5; i++)` y cuánto vale `i` en la última?" Respuesta: 5 vueltas, la última con `i` igual a 4.
- **05:** "El do-while tiene una condición falsa desde el principio, ¿entra?" Respuesta: sí, una vez.
- **07:** "¿Cuánto valdrá `saldo` después de llamar a `sumar_diez(saldo)`?" Respuesta: 100, la función trabajó con una copia.
- **08:** "¿Y ahora, con `int&`?" Respuesta: 110. "¿Y tras `sumar_cien(&saldo)`?" Respuesta: 210.
- **09:** "¿Qué valor tendrá `local` en la tercera llamada?" Respuesta: 1, nace de nuevo cada vez. "¿Y `recuerdo`?" Respuesta: 3.
- **10:** "Si `duplicar_todos` recibiera el vector sin `&`, ¿qué imprimiría?" Respuesta: 4, 7 y 1, porque duplicaría una copia.
- **11:** "¿Qué pasa si escribo `hola`?" Respuesta: la lectura falla y se usa el valor 3.
- **13:** "¿Dónde pongo el `i++` en el while?" Respuesta: al final del cuerpo. "¿Y si lo olvido?" Respuesta: el bucle no termina.
- **14:** "Si duermo cinco veces 100 ms, ¿tardará exactamente 500 ms?" Respuesta: no, al menos 500 ms y un poco más; el sistema despierta al programa cuando puede.
- **15:** "¿Puede la tortuga 3 moverse antes que la 1 en una vuelta?" Respuesta: no, aquí el orden es fijo. Con hilos no lo será.

## Errores típicos de los alumnos

- Escribir `if (nota = 5)` en lugar de `==`: asigna en vez de comparar y siempre entra.
- Esperar decimales de `7 / 2` entre enteros, o calcular una media con dos `int`.
- Poner `;` justo después del `for (...)` o del `while (...)`: el bucle queda vacío.
- Usar `<=` donde iba `<` y dar una vuelta de más (o salirse del vector).
- Olvidar el `&` en el parámetro y creer que la función modifica el original.
- Poner `&` en la llamada cuando la función espera una referencia (`sumar_diez(&saldo)`): error de compilación, confunden referencia con dirección.
- Usar una variable fuera de las llaves donde se declaró.
- Comparar `i < notas.size()` con `i` entero y recibir un aviso de signo: por eso escribimos `(int) notas.size()`.
- En el `while`, olvidar el `i++` o ponerlo antes de usar `notas[i]` (se salta el primero y se sale del vector).
- Salir de un bucle con `break` "porque es más fácil": no está permitido; bandera en la condición.
- Crear el generador aleatorio dentro del bucle en cada vuelta, o usar `rand() % 5`.
- No gestionar el fallo de `cin` y dejar el programa en un bucle de lecturas fallidas.

## Cierre y enlace con la siguiente carpeta

Quedan en la pizarra las dos ideas clave: qué se copia y qué se comparte, y cuánto vive cada variable. La carpeta `02-fundamentos-multiprogramacion` explica, muy resumido, qué hace el sistema operativo para que varios programas parezcan ejecutarse a la vez; en la `03` convertimos la carrera lineal en una carrera con hilos.

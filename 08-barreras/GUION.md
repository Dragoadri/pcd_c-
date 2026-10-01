# Guion de clase: 08 barreras

**Duración estimada:** 180 minutos (1 sesión de ~110 min de exposiciones por equipos + ~70 min de sesión de cierre del profesor; la investigación de los equipos se hace fuera del aula durante una semana)
**Entra en examen:** No entra en el Parcial 1, pero se permite usar `latch` y barreras en sus ejercicios

## Objetivo de la sesión

Que el alumno sepa **reunir a varios hilos en un punto** antes de continuar: con `latch` (se abre una vez y se queda abierto) y con una barrera (se abre cada vez que llegan N hilos y se vuelve a cerrar). Y que sepa distinguirlos de un semáforo, que limita cuántos pasan pero no espera a nadie.

## Aviso técnico que el profesor debe tener claro antes de la clase

- `latch` es **estándar de C++20** (`<latch>`). Funciona en GCC, Clang y MSVC.
- `barrier` **no es estándar**. Estaba en la propuesta P1135 para C++20 pero se retiró antes de aprobarse. libstdc++ (GCC, y Clang en Linux porque usa la misma biblioteca) la incluye como extensión con `arrive_and_wait()`. En macOS (libc++) y en Visual Studio da "file not found". Verificado aquí con GCC 13.3 y Clang 18.
- POSIX tiene su propia barrera, `pthread_barrier_t`, que funciona en Linux. No hay fichero de ejemplo: se menciona para que un equipo la investigue.
- Un equipo que aparezca con macOS no podrá compilar `05`; debe usar la barrera artesanal del `04`.

---

## Parte A: clase invertida (los alumnos investigan y exponen)

### Organización

- **Una semana antes:** se forman 5 equipos de 3 o 4 personas y se reparte un tema a cada uno. Se les da acceso solo a la tabla de temas de abajo, **no** a los ficheros de esta carpeta.
- **Entrega previa (48 h antes de la sesión):** el código de su demo y una hoja con 5 transparencias como máximo.
- **Día de la sesión:** 5 exposiciones de 12 min más 5 min de réplica cada una (85 min), con 15 min de margen para cambios de equipo y ajustes técnicos.

### Temas por equipo

| Equipo | Tema a investigar | Qué debe quedar claro |
|---|---|---|
| 1 | `latch` como "pistoletazo de salida" | Qué hacen `wait()` y `count_down()`, por qué un latch no se puede reutilizar, un ejemplo con `latch(1)` |
| 2 | `latch` para esperar a que N hilos terminen de prepararse | `latch(N)`, quién baja el contador y quién espera; por qué es mejor que un `sleep` "a ojo" |
| 3 | Qué es una barrera y cómo construirla con `mutex` y `condition_variable` | Una barrera que se reabre en cada ronda; por qué `notify_all` y no `notify_one` |
| 4 | `barrier` en C++: historia y disponibilidad | Propuesta P1135, por qué no está en el estándar, qué compiladores la traen, `arrive_and_wait()` |
| 5 | `pthread_barrier_t` de POSIX | `pthread_barrier_init`, `pthread_barrier_wait`, `pthread_barrier_destroy`, el valor especial `PTHREAD_BARRIER_SERIAL_THREAD`; se puede usar junto a `thread` |

### Qué debe contener cada exposición

1. **El problema** en una frase y con un ejemplo cotidiano (no vale copiar el de otro equipo).
2. **La API mínima**: las dos o tres llamadas imprescindibles, nada más.
3. **Una demo ejecutable en directo** en CLion que cumpla la norma de la asignatura: sin `break`, sin `continue`, sin clases propias, candados con `lock()` y `unlock()` a mano, todos los hilos con `join()` y que termine sola.
4. **Un error provocado a propósito** y su consecuencia (por ejemplo, un hilo que nunca llega a la barrera).
5. **Cuándo usarlo y cuándo no**, comparado con un semáforo.

### Rúbrica de la exposición (10 puntos)

| Criterio | Puntos | Qué se mira |
|---|---|---|
| Corrección técnica | 3 | Lo que cuentan es cierto; distinguen latch, barrera y semáforo |
| Demo | 3 | Compila, se ejecuta en directo, cumple la norma, se ve el efecto en la salida |
| Error provocado y explicación | 2 | Saben explicar por qué falla, no solo que falla |
| Claridad y tiempo | 1 | Se ajustan a 12 min, el resto de la clase lo entiende |
| Réplica | 1 | Responden a las preguntas sin leer |

### Preguntas para el turno de réplica

- Equipo 1: "¿Qué pasa si hago `count_down()` dos veces sobre un `latch(1)`?" (Comportamiento indefinido: el contador no puede bajar de 0.) "¿Puedo volver a cerrar el latch?" (No.)
- Equipo 2: "Si uno de los N hilos falla y nunca hace `count_down()`, ¿qué le pasa a main?" (Espera para siempre: fallo de vivacidad.)
- Equipo 3: "¿Por qué `while` alrededor de `wait` y no `if`?" (Despertares espurios.) "¿Qué pasa si llegan N+1 hilos?" (El que sobra espera a la ronda siguiente, que quizá nunca se complete.)
- Equipo 4: "Si `barrier` compila en tu portátil, ¿por qué no deberías entregarla sin avisar?" (No es estándar; en otro entorno no compila.)
- Equipo 5: "¿Qué devuelve `pthread_barrier_wait` a uno solo de los hilos y para qué sirve?" (`PTHREAD_BARRIER_SERIAL_THREAD`, para que un único hilo haga una tarea tras la reunión.)
- A cualquiera: "¿Un semáforo con 0 permisos y N `release()` hace lo mismo que una barrera?" (No: el semáforo no espera a que lleguen todos, deja pasar a quien encuentre permiso.)

---

## Parte B: sesión integral de cierre del profesor

Se hace justo después de las exposiciones (o al inicio de la clase siguiente). Objetivo: dejar ordenado lo que los equipos han contado, con ejemplos bien construidos y comparables entre sí.

### Pizarra

1. Una línea de salida con 4 corredores y tiempos distintos de preparación. Pregunta: "¿Salen a la vez?"
2. **Latch:** un número grande que solo baja (`1` o `N`), con una flecha hacia 0 y la palabra "abierto para siempre".
3. Dos latch con papeles cruzados (fichero 03): `listos(N)` lo bajan los hilos y espera main; `pistoletazo(1)` lo baja main y esperan los hilos.
4. **Barrera:** una puerta que se abre al llegar el cuarto excursionista y **se vuelve a cerrar**. Dibujar dos refugios seguidos.
5. Tabla final (se copia y se deja):

   | | Semáforo | Latch | Barrera |
   |---|---|---|---|
   | Pregunta que responde | ¿Cuántos pueden estar dentro? | ¿Ha ocurrido ya el evento? | ¿Hemos llegado todos? |
   | Se reutiliza | Sí | No | Sí |
   | Estándar C++20 | Sí | Sí | No (`barrier` es extensión de libstdc++) |

### Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 1 | `01_salida_desigual_sin_barrera.cpp` | 10 | Cada hilo escribe solo su casilla de `salida_ms`: no hay carrera. El problema no es de datos, es de coordinación. |
| 2 | `02_latch_pistoletazo_de_salida.cpp` | 12 | Añadir el latch al 01. Todos salen en el mismo milisegundo. Señalar el `sleep` de 1 s del juez: es un apaño. |
| 3 | `03_latch_esperar_a_n_listos.cpp` | 15 | Subir la preparación hasta 1500 ms: con el 02 fallaría. Dos latch y ningún `sleep` en main. |
| 4 | `04_barrera_artesanal.cpp` | 18 | Tres variables y una función, igual que el semáforo artesanal de 07. El truco de `barrera_aperturas` para saber que mi ronda ya se abrió. |
| 5 | `05_std_barrier_de_libstdcxx.cpp` | 10 | Misma salida que el 04 con una línea. Leer en voz alta el aviso de la cabecera. Mencionar `pthread_barrier_t` como alternativa POSIX. |
| | Tabla final y dudas | 5 | |

### Preguntas a la clase antes de ejecutar

- **01:** "¿Hay condición de carrera en `salida_ms`?" No: cada hilo escribe una casilla distinta y main solo lee después de los `join`.
- **02:** "¿Qué se imprime antes, `Juez: PUM!` o el último `listo`?" Casi siempre el último `listo`, pero solo porque la preparación máxima (800 ms) es menor que 1 s. "¿Y si alguien tarda 1,5 s?" Saldría tarde.
- **03:** "¿Puede aparecer `Juez: todos listos` antes de algún `listo`?" No: main está bloqueado en `listos.wait()` hasta el último `count_down()`.
- **04:** "¿Qué pasaría con un `latch` en la segunda etapa?" Ya estaría abierto: en el refugio 2 nadie esperaría a nadie. "¿Por qué `notify_all`?" Hay que despertar a los 3 que esperan, no a uno.
- **05:** "¿Compilará esto en el Mac de un compañero?" No.

## Errores típicos de los alumnos

- Usar un `latch` en un bucle esperando que se cierre otra vez: tras llegar a 0 no vuelve a bloquear.
- Hacer más `count_down()` que el valor inicial del latch.
- Pasar el `latch` o la barrera por valor a un hilo: no se pueden copiar (error de compilación). Se declaran globales o se pasan con `ref`.
- Crear la barrera con un N distinto del número de hilos que realmente llegan: si llegan menos, todos esperan para siempre.
- Un hilo que termina antes por un `if` y no llega a la barrera: los demás se quedan bloqueados.
- Usar `notify_one` en la barrera artesanal: solo despierta a uno y los demás siguen esperando.
- Confundir barrera con semáforo: el semáforo deja pasar a quien encuentre permiso, sin esperar a nadie.
- Entregar código con `#include <barrier>` sin avisar de que solo compila con libstdc++.

## Cierre y enlace con la siguiente carpeta

Con semáforos, candados y barreras el primer bloque queda completo. La carpeta **09-repaso-parcial-1** tiene ejercicios tipo examen y un simulacro. Recordar a la clase que las barreras **no entran en el examen**, pero que si las usan bien en un ejercicio se dan por válidas.

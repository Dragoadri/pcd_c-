# Guion de clase: 07 semáforos

**Duración estimada:** 200 minutos (2 sesiones de ~110 min útiles)
**Entra en examen:** Parcial 1

## Objetivo de la sesión

Que el alumno entienda que un semáforo es **un contador de permisos protegido**, que sirve para dos cosas distintas (limitar cuántos hilos usan un recurso y avisar a otro hilo) y que sepa resolver el productor-consumidor con búfer acotado.

## Antes de empezar (preparación del profesor)

- Tener compilados `01` y `02` y el `03` de la carpeta `04-hilos-usados-mal` (contador roto) para comparar.
- Comprobar que el perfil de CLion es Debug: los ejemplos `03`, `06` y `07` van a `-O0` de todas formas.
- La Actividad 3 (Semáforos) del material antiguo no estaba disponible; los ejercicios salen del Tema 2 presencial (restaurante, productor-consumidor) y del problema clásico de los filósofos.

## Pizarra

**Sesión 1**

1. Dibujo de un restaurante con 4 mesas y una fila de 10 personas en la puerta. Pregunta: "¿un mutex resuelve esto?". Respuesta a construir: el mutex deja pasar a **uno**; aquí queremos que pasen **hasta cuatro**.
2. Un semáforo es una caja con fichas:
   - `acquire()`: coges una ficha. Si no hay, **esperas dormido** (no gastas CPU).
   - `release()`: devuelves una ficha y despiertas a alguien.
3. Tabla comparativa (se queda toda la sesión en la pizarra):

   | | mutex | binary_semaphore | counting_semaphore |
   |---|---|---|---|
   | Cuántos pasan | 1 | 1 | N |
   | Tiene dueño | Sí | No | No |
   | Sirve de señal entre hilos | No | Sí | Sí |

4. Las tres piezas de la variable de condición: **condición** (un bool), **mutex** que la protege, **condition_variable** que duerme y despierta. Escribir grande: `while (!condicion) cv.wait(candado);` y al lado "WHILE, no IF".
5. Semáforo artesanal: `int permisos` + mutex + condition_variable. Frase para subrayar: "un semáforo es un contador protegido".

**Sesión 2**

6. Búfer circular de 5 casillas con dos flechas (`entrada`, `salida`) y la variable `cantidad`. Marcar en rojo `cantidad++` y `cantidad--`: los dos hilos la tocan.
7. Solución con dos semáforos: `huecos` (empieza en 5) y `elementos` (empieza en 0), más un mutex para el búfer. Escribir el orden: **primero semáforo, después mutex**.
8. Mesa redonda con 5 filósofos y 5 tenedores. Dibujar la espera circular con flechas. Poner al camarero en la puerta con 4 fichas.

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 1 | `01_restaurante_sin_limite.cpp` | 15 | El contador va protegido con mutex y aun así el aforo se supera: proteger un dato no es limitar el acceso. |
| 2 | `02_restaurante_con_counting_semaphore.cpp` | 15 | Copiar el 01 y añadir solo dos líneas: `acquire` y `release`. El `<4>` es el máximo; el `(4)` es el valor inicial. |
| 3 | `03_binary_semaphore_como_candado.cpp` | 20 | Parte 1: el contador vuelve a ser exacto. Parte 2: otro hilo hace `release()` de un permiso que no cogió. Con un mutex eso es comportamiento indefinido. |
| 4 | `04_condition_variable_basica.cpp` | 25 | Las tres piezas. Excepción de la norma: `unique_lock`, porque `wait` suelta y recupera el mutex. Despertares espurios y aviso perdido. |
| 5 | `05_semaforo_artesanal.cpp` | 25 | Mismo restaurante que el 02. Poner los dos ficheros en paralelo (CLion: Split Right) y ver que solo cambian `adquirir()` y `liberar()`. |
| 6 | `06_productor_consumidor_sin_sincronizar.cpp` | 30 | Búfer circular a mano. Ejecutar 3 veces: perdidos distintos cada vez, y el programa acaba por el límite de 3 s porque un hilo se queda esperando para siempre. |
| 7 | `07_productor_consumidor_con_semaforos.cpp` | 35 | Dos semáforos y un mutex. Ahora con 2 productores y 2 consumidores: todo a 0. Cambiar el orden semáforo/mutex en la pizarra y razonar el interbloqueo. |
| 8 | `08_filosofos_con_semaforo_camarero.cpp` | 30 | Primero ejecutarlo tal cual. Después comentar las dos líneas del camarero, ejecutar, ver que se queda parado y pararlo con Stop. Descomentar. |
| | Cierre y dudas | 5 | |

## Preguntas a la clase antes de ejecutar

- **01:** "El contador de mesas ocupadas va con mutex. ¿Puede salir un máximo de 10?" Sí: el mutex protege el número, pero nadie impide sentarse.
- **02:** "¿Qué clientes se sientan primero?" No se sabe: el semáforo no garantiza orden de llegada (no es una cola justa).
- **03:** "¿Tarda más o menos que el contador con mutex de la carpeta 06?" Parecido; lo importante es que el total es exacto. "¿Puede otro hilo hacer `unlock()` de un mutex que no ha cogido?" No, es comportamiento indefinido. Con un semáforo sí puede.
- **04:** "Si cambio el `while` por un `if`, ¿funciona?" Casi siempre sí, y por eso es peligroso: un despertar espurio lo rompe. "¿Qué pasa si quito la variable `plato_listo` y solo uso `notify_one`?" Si el cocinero avisa antes de que el camarero llegue a `wait`, el aviso se pierde y el camarero duerme para siempre.
- **05:** "¿Por qué `liberar()` hace `notify_one` y no `notify_all`?" Se libera un permiso, así que basta con despertar a un hilo.
- **06:** "¿Cuántos elementos se perderán?" Distinto en cada ejecución. "¿Por qué el programa tarda justo 3 segundos?" Porque `cantidad` se corrompe y un hilo espera algo que no va a llegar.
- **07:** "¿Hace falta el mutex si ya hay semáforos?" Sí: con 2 productores, los dos pueden escribir en la misma casilla `entrada`. Los semáforos cuentan, el mutex protege.
- **08:** "Sin camarero, ¿qué ocurre si los 5 cogen el tenedor izquierdo a la vez?" Interbloqueo: espera circular. "¿Por qué 4 permisos y no 5?" Con 5 se puede volver a dar la espera circular.

## Errores típicos de los alumnos

- Hacer `release()` más veces que `acquire()`: el contador pasa del máximo y es comportamiento indefinido.
- Olvidar el `release()` en un camino del código (un `if` que sale antes): el recurso se pierde para siempre, como el `unlock()` olvidado.
- Poner el `mutex.lock()` **antes** del `acquire()` en el productor-consumidor: interbloqueo.
- Usar `if` en lugar de `while` alrededor de `cv.wait`.
- Modificar la condición (`plato_listo = true`) sin coger el mutex.
- Llamar `counting_semaphore<n>` con una variable no constante: el máximo tiene que conocerse al compilar (`const int`).
- Confundir el máximo (entre `< >`) con el valor inicial (entre `( )`), sobre todo con `binary_semaphore(0)`, que empieza cerrado.
- Creer que el semáforo reparte los permisos por orden de llegada.

## Cierre y enlace con la siguiente carpeta

Resumen de la tabla de la pizarra y del patrón "primero semáforo, después mutex". Se anuncia la carpeta **08-barreras**, que se trabaja en clase invertida: los equipos investigan `latch` y las barreras (esperar a que **todos** los hilos lleguen a un punto), y la carpeta **09-repaso-parcial-1** con el simulacro. Las barreras no entran en el examen pero se pueden usar.

# Rúbrica de la práctica final: mensajería instantánea concurrente

Programación Concurrente y Distribuida · Universidad Europea de Madrid

La corrección es una lista de comprobación: cada requisito **o está o no está**. No hay medios puntos por "casi funciona". Un requisito cuenta como cumplido solo si se puede comprobar en la prueba que hace el profesor siguiendo el manual de la memoria.

## 1. Cómo se calcula la nota

1. Se comprueba que el proyecto **compila en Linux** siguiendo el manual. Si no compila: **NO ENTREGADA**.
2. Se recorren los requisitos del **tramo 1**. Si se cumplen todos los obligatorios (**O**), el tramo está completo.
3. Solo si el tramo 1 está completo se suman los puntos del **tramo 2**; solo si el tramo 2 está completo se suman los del **tramo 3**.
4. Si el tramo 1 **no** está completo, la nota funcional es la suma de lo cumplido en el tramo 1 con un **máximo de 4** (práctica suspensa).
5. Se restan las **penalizaciones** (sección 5). La nota del grupo nunca baja de 0.
6. Se aplica el **factor de defensa** individual (sección 6).

Nota individual = (puntos de tramos − penalizaciones) × factor de defensa

## 2. Tramo 1: servidor concurrente funcional (hasta 5 puntos)

| Código | Requisito | Tipo | Puntos | Cómo se comprueba | Cumple |
|---|---|---|---|---|---|
| T1.1 | Compila con CMake desde cero en Linux con `-Wall -Wextra`, sin avisos graves | O | 0,25 | `cmake -S . -B build && cmake --build build` en una copia limpia | ☐ |
| T1.2 | 5 o más clientes conectados a la vez, un hilo por conexión | O | 0,50 | Cinco terminales con cliente; revisión del bucle de aceptación | ☐ |
| T1.3 | Registro e inicio de sesión en memoria | O | 0,50 | `REGISTRO` y `LOGIN` correctos y con clave errónea | ☐ |
| T1.4 | Un mismo usuario no puede tener dos sesiones a la vez (comprobación en sección crítica) | O | 0,50 | Dos clientes hacen `LOGIN` con el mismo usuario; revisión del código | ☐ |
| T1.5 | `LISTA` de usuarios conectados | O | 0,25 | Conectar y desconectar clientes y repetir `LISTA` | ☐ |
| T1.6 | Mensajes privados que aparecen en el destinatario sin pulsar nada (hilo receptor) | O | 0,50 | Dos clientes; el receptor no escribe nada y ve el mensaje | ☐ |
| T1.7 | Estructuras compartidas protegidas con `lock()` y `unlock()` a mano | O | 0,75 | Revisión del código frente a la tabla de recursos compartidos | ☐ |
| T1.8 | Protocolo por líneas robusto ante órdenes juntas o partidas | O | 0,50 | `printf 'LISTA\nLISTA\n' \| nc localhost PUERTO` y envío por trozos | ☐ |
| T1.9 | Cierre ordenado con `join` de todos los hilos | O | 0,50 | Apagado con clientes conectados: termina sin `terminate called` ni Ctrl+C | ☐ |
| T1.10 | Desconexión brusca de un cliente sin caída del servidor y sin usuario "fantasma" | O | 0,25 | Cerrar una terminal de cliente y hacer `LISTA` y `MSG` desde otro | ☐ |
| T1.11 | Errores del protocolo correctos (órdenes mal formadas, destinos inexistentes) | Puntuable | 0,25 | Batería de órdenes erróneas | ☐ |
| T1.12 | Memoria: manual de ejecución, protocolo y tabla de recursos compartidos | Puntuable | 0,25 | Lectura de la memoria | ☐ |
| | **Total tramo 1** | | **5,00** | | |

## 3. Tramo 2: persistencia y grupos (hasta 2 puntos más, nota máxima 7)

| Código | Requisito | Tipo | Puntos | Cómo se comprueba | Cumple |
|---|---|---|---|---|---|
| T2.1 | SQLite con la amalgamación en el proyecto; usuarios y mensajes persisten tras reiniciar | O | 0,30 | Registrar, enviar, apagar, encender, `LOGIN` e `HISTORIAL` | ☐ |
| T2.2 | Tres capas en ficheros separados, sin SQL fuera de datos ni sockets fuera de presentación | O | 0,30 | `grep` de `sqlite3_` y `send`/`recv` por fichero | ☐ |
| T2.3 | Consultas preparadas en todas las consultas con datos del usuario | O | 0,20 | Revisión de la capa de datos; registro con usuario `a'--` o similar | ☐ |
| T2.4 | Mensajes diferidos a desconectados, entregados en orden al iniciar sesión | O | 0,30 | Enviar tres mensajes a un usuario desconectado y conectarlo | ☐ |
| T2.5 | Límite de conexiones con `counting_semaphore` y comportamiento definido al llenarse | O | 0,20 | Bajar `MAX_CONEXIONES` a 2 y conectar un tercer cliente | ☐ |
| T2.6 | Acceso concurrente a la base de datos protegido y justificado | O | 0,20 | Revisión del código y de la justificación en la memoria | ☐ |
| T2.7 | Grupos: crear, unirse, mensajes a miembros conectados y diferidos | Puntuable | 0,20 | Grupo de tres usuarios con uno desconectado | ☐ |
| T2.8 | `HISTORIAL` privado y de grupo con fechas | Puntuable | 0,15 | `HISTORIAL;luis;10` y `HISTORIAL;grupo;10` | ☐ |
| T2.9 | Memoria: diagrama de base de datos y tabla de recursos actualizada | Puntuable | 0,15 | Lectura de la memoria | ☐ |
| | **Total tramo 2** | | **2,00** | | |

## 4. Tramo 3: robusto, medido y seguro (hasta 3 puntos más, nota máxima 10)

| Código | Requisito | Tipo | Puntos | Cómo se comprueba | Cumple |
|---|---|---|---|---|---|
| T3.1 | Confirmaciones `ENTREGADO` y `LEIDO`, también para diferidos | O | 0,40 | Diálogo entre dos clientes y con destinatario desconectado | ☐ |
| T3.2 | `ESTADO` en línea o fecha de última conexión | O | 0,30 | Consultar un usuario conectado y otro desconectado | ☐ |
| T3.3 | `lanzador_clientes` con N hilos cliente que inician sesión y envían M mensajes | O | 0,40 | Ejecución con los parámetros de la memoria | ☐ |
| T3.4 | Pruebas de carga con al menos tres tamaños, tiempos, mensajes por segundo y cero mensajes perdidos | O | 0,40 | Repetición de una prueba y comparación con la base de datos | ☐ |
| T3.5 | Registro de actividad en fichero con fecha e hilo, protegido | Puntuable | 0,30 | Lanzador con 50 clientes y revisión de que ninguna línea está mezclada | ☐ |
| T3.6 | Contraseñas con hash y sal por usuario, sin librerías externas (algoritmo reconocido implementado o justificación honesta; con `hash` no puntúa) | Puntuable | 0,50 | Consulta de la tabla de usuarios y revisión del código | ☐ |
| T3.7 | Validación completa de entradas en presentación y negocio | Puntuable | 0,20 | Nombres largos, caracteres no permitidos, campos de más o de menos | ☐ |
| T3.8 | Límite de intentos de login fallidos con bloqueo temporal, estado protegido | Puntuable | 0,20 | Varios `LOGIN` erróneos seguidos desde dos clientes | ☐ |
| T3.9 | Mejora de libre elección documentada | Puntuable | 0,30 | Demostración y apartado en la memoria | ☐ |
| | **Total tramo 3** | | **3,00** | | |

T3.6 puntúa 0,50 con un algoritmo reconocido correctamente implementado (por ejemplo, SHA-256 comprobado con vectores de prueba conocidos) y 0,20 con un esquema más débil pero con sal y limitaciones explicadas con honestidad.

## 5. Penalizaciones

| Incumplimiento | Penalización |
|---|---|
| El proyecto no compila en Linux siguiendo el manual | **NO ENTREGADA** |
| Falta la memoria técnica | Nota máxima 4 |
| Construcción prohibida por la norma (`break`, `continue`, `goto`, `switch`, clase, `struct`, `union` o `enum class` propios, `lock_guard`, `scoped_lock`, `jthread`, `detach`) | −0,10 por aparición, hasta −2,00. Con más de 20 apariciones el tramo 1 no se considera completo |
| `unique_lock` fuera de `condition_variable::wait` | −0,10 por aparición (cuenta dentro del máximo anterior) |
| Hilo destruido sin `join` (el programa termina con `terminate called`) | −1,00 (además, T1.9 no se cumple) |
| Cuelgue reproducible (interbloqueo, bucle sin salida, cliente o servidor que no terminan) | −0,50 por caso, hasta −1,50 |
| Condición de carrera evidente sobre un recurso compartido sin proteger | −0,50 por recurso |
| Coordinación de hilos con `sleep_for` o espera activa | −0,25 por caso |
| Dependencias externas (Boost, Asio, bibliotecas no permitidas) | −1,00, o NO ENTREGADA si no compila sin ellas |
| Carpetas de compilación o base de datos incluidas en la entrega | −0,10 |
| Uso de IA no declarado en el anexo, detectado en la revisión o en la defensa | Nota 0 en la práctica para las personas implicadas y aplicación de la normativa académica |

## 6. Defensa oral individual

Cada estudiante defiende la práctica por separado, durante 10 a 15 minutos, con el proyecto abierto en su ordenador. El profesor elige ficheros y líneas al azar.

| Resultado | Descripción | Factor |
|---|---|---|
| Supera con solvencia | Explica cualquier parte del código, justifica las decisiones de concurrencia, predice correctamente el comportamiento en situaciones concurrentes y realiza la modificación pedida | × 1,0 |
| Supera con dudas | Explica su parte y la arquitectura general, pero duda en partes ajenas o necesita ayuda para predecir comportamientos o para la modificación | × 0,8 |
| No supera | No sabe explicar el código entregado, no identifica los recursos compartidos ni sus candados, o no es capaz de hacer una modificación sencilla | La nota **no cuenta** (0) |

Criterios que se observan en la defensa:

1. Localiza en el código dónde se crea, dónde se une y cuándo termina cada tipo de hilo.
2. Señala todas las secciones críticas de una función y qué candado protege cada una.
3. Predice qué ocurre en un entrelazado concreto propuesto por el profesor.
4. Explica por qué el protocolo funciona por líneas y qué pasaría si se leyera con un único `recv`.
5. Justifica el uso del semáforo y qué pasa al alcanzar el límite.
6. Hace en directo una modificación pequeña (por ejemplo, añadir una orden al protocolo o proteger un recurso nuevo) cumpliendo la norma.
7. Explica qué partes se generaron con IA y qué se cambió de ellas.

## 7. Criterios de calidad de la memoria

Los puntos de memoria (T1.12, T2.9 y parte de T3.4) se conceden solo si la memoria cumple:

| Criterio | Cumple |
|---|---|
| Portada, índice, miembros del grupo y tramo al que se opta | ☐ |
| Manual de compilación y ejecución reproducible paso a paso en dos o más terminales | ☐ |
| Diagrama de capas y diagrama de hilos coherentes con el código entregado | ☐ |
| Protocolo completo en tablas, incluidas las extensiones propias | ☐ |
| Tabla de recursos compartidos completa y con orden de adquisición de candados | ☐ |
| Apartado de problemas de concurrencia encontrados y su solución | ☐ |
| Pruebas con capturas de pantalla completa y, en el tramo 3, tablas o gráficas de carga | ☐ |
| Reparto de trabajo y anexo de uso de IA | ☐ |

## 8. Ejemplo de cálculo

Un grupo de tres personas entrega:

- **Tramo 1**: todos los obligatorios y los dos puntuables. Tramo completo: **5,00**.
- **Tramo 2**: todos los obligatorios (1,50), grupos (0,20) y memoria actualizada (0,15); no hay historial de grupo. Tramo completo: **1,85**.
- **Tramo 3**: confirmaciones (0,40), estados (0,30), lanzador (0,40) y registro de actividad (0,30). Las pruebas de carga no demuestran que no se pierden mensajes, así que T3.4 no cuenta. Como es el último tramo, lo cumplido suma igualmente: **1,40**.

Puntos de tramos: 5,00 + 1,85 + 1,40 = **8,25**.

Penalizaciones: tres `break` en el cliente (−0,30) y una espera con `sleep_for` para "dar tiempo" al servidor en el lanzador (−0,25). Total: **−0,55**.

Nota del grupo antes de la defensa: 8,25 − 0,55 = **7,70**.

| Estudiante | Defensa | Nota final |
|---|---|---|
| Estudiante A | Supera con solvencia (× 1,0) | 7,70 |
| Estudiante B | Supera con dudas (× 0,8) | 6,16 |
| Estudiante C | No supera | La nota no cuenta |

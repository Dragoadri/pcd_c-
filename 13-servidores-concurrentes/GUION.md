# Guion de clase: 13 servidores concurrentes

**Duración estimada:** 220 minutos (2 sesiones de ~110 min)
**Entra en examen:** Parcial 2 (servidor con un hilo por conexión, protección de estructuras compartidas con candado, límite de conexiones con semáforo)

## Objetivo de la sesión

Juntar los dos parciales: un servidor TCP que atiende a varios clientes **a la vez** con un hilo por conexión, y en el que vuelven a aparecer, ahora con clientes de red, los problemas del primer parcial: **condición de carrera** en lo compartido (se arregla con un candado) y **exceso de trabajo simultáneo** (se limita con un semáforo). Se cierra con un chat de difusión, esqueleto de la práctica final.

## Antes de empezar (preparación del profesor)

- Compilar las carpetas 12 y 13 completas (el lanzador se usa también contra el servidor secuencial `12/05`).
- Tres terminales visibles. Con el lanzador basta con dos.
- Tabla de puertos a mano (está también en la cabecera del lanzador):

| Puerto | Servidor |
|---|---|
| 5202 | `12-sockets-tcp/05_servidor_tcp_secuencial` |
| 5300 | `01_servidor_un_hilo_por_conexion` |
| 5301 | `03_servidor_visitas_sin_mutex` |
| 5302 | `04_servidor_visitas_con_mutex` |
| 5303 | `05_servidor_limite_con_semaforo` |
| 5304 | `06_servidor_chat_difusion` (cliente: `07_cliente_chat`) |

- Las llamadas de sockets ya se teclearon completas en la carpeta 12. Aquí están recogidas en `red_comun.hpp` (funciones libres `inline`), que cada ejemplo incluye con `#include "red_comun.hpp"`. Los `.cpp` quedan centrados en hilos, candados y semáforos.
- Todos los servidores de la carpeta (menos el chat) hablan el mismo protocolo: el cliente envía su nombre en una línea y recibe una línea de respuesta. Por eso un único lanzador sirve para todos.

## Pizarra

**Sesión 1**

1. Recuperar la línea de tiempo de la carpeta 12: dos clientes, 5 s y 7 s. Pregunta: "¿qué herramienta del primer parcial permite hacer dos cosas a la vez?".
2. Dibujo del servidor concurrente: `main` en un bucle de `accept`; de cada `accept` sale una flecha a un **hilo nuevo** con su propio socket de conexión. Al final, todas las flechas vuelven a `join`.
3. Recuadro de aviso: el descriptor se pasa **por valor** a `thread`. Con `ref` sobre la variable del bucle, el hilo podría leer el descriptor del cliente siguiente (trampa de la carpeta 04).
4. Tabla "qué comparte cada hilo": propio de cada hilo (socket de conexión, nombre, variables locales) frente a compartido (contador de visitas, libro, lista de conectados, `cout`). Lo compartido es territorio de carreras.

**Sesión 2**

5. Semáforo como **aforo del local**: 3 huecos. Dibujo de una puerta con un contador 3, 2, 1, 0 y una cola; un reloj de 5 s en la cola ("si no entras en 5 s, te vas").
6. Chat: una caja con la lista `sockets_conectados` y un candado; varios hilos con flechas hacia la caja (entrar, salir, recorrer para difundir). En el cliente, dos hilos sobre el mismo socket: uno escucha y otro habla.

## Cómo lanzarlo en dos terminales

Siempre primero el servidor, desde la carpeta del proyecto:

```
# Comparar secuencial frente a concurrente (5 clientes a la vez)
./build/bin/05_servidor_tcp_secuencial                                   # terminal 1
printf "5202\n5\n" | ./build/bin/02_lanzador_de_clientes_concurrentes    # terminal 2

./build/bin/01_servidor_un_hilo_por_conexion                             # terminal 1
./build/bin/02_lanzador_de_clientes_concurrentes                         # terminal 2 (Enter, Enter)

# Carrera en el contador de visitas (50 clientes a la vez)
./build/bin/03_servidor_visitas_sin_mutex                                # terminal 1
printf "5301\n50\n" | ./build/bin/02_lanzador_de_clientes_concurrentes   # terminal 2
./build/bin/04_servidor_visitas_con_mutex                                # terminal 1
printf "5302\n50\n" | ./build/bin/02_lanzador_de_clientes_concurrentes   # terminal 2

# Límite con semáforo (10 clientes a la vez)
./build/bin/05_servidor_limite_con_semaforo                              # terminal 1
printf "5303\n10\n" | ./build/bin/02_lanzador_de_clientes_concurrentes   # terminal 2

# Chat: un servidor y tres clientes seguidos
./build/bin/06_servidor_chat_difusion                                    # terminal 1
./build/bin/07_cliente_chat                                              # terminales 2, 3 y 4
# o en un solo terminal:
./build/bin/07_cliente_chat & ./build/bin/07_cliente_chat & ./build/bin/07_cliente_chat
```

En CLion: Run del servidor; en el desplegable, el lanzador y Run (el lanzador pregunta puerto y número de clientes en la pestaña Run: se escribe ahí). Para varios clientes de chat, *Edit Configurations*, *Allow multiple instances* en `07_cliente_chat` y pulsar Run tres veces. Ejecutables en `cmake-build-debug/bin/`.

## Secuencia de ficheros

**Sesión 1 (110 min)**

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 0 | Pizarra 1 a 3 | 10 | Del secuencial al hilo por conexión. Paso por valor del descriptor. |
| 0b | `red_comun.hpp` (se abre, no se teclea) | 5 | Es lo escrito en la carpeta 12 recogido en funciones: `abrir_escucha`, `conectar_con_servidor`, `configurar_timeout`, `enviar_todo`, `recibir_linea`. Por qué `inline` en una cabecera. Desde aquí, el código de clase es solo concurrencia. |
| 1 | `01_servidor_un_hilo_por_conexion.cpp` | 25 | `#include "red_comun.hpp"`. El cuerpo del bucle del secuencial pasa a `atender_cliente`. Vector de hilos, `join` de todos. `imprimir` con `mtx_consola` porque ahora varios hilos escriben. |
| 2 | `02_lanzador_de_clientes_concurrentes.cpp` | 25 | N hilos cliente que arrancan a la vez con `latch` (carpeta 08). Cada hilo escribe solo en su casilla del vector: sin candado (Bernstein). Lectura de `cin` con valor por defecto. |
| 3 | Experimento: lanzador contra 5202 y contra 5300 | 10 | Secuencial: 2, 4, 6, 8, 10 s. Concurrente: todos 2 s. |
| 4 | Pizarra 4 | 5 | Qué es propio y qué es compartido. |
| 5 | `03_servidor_visitas_sin_mutex.cpp` | 15 | Leer, trabajar 1 ms, escribir: la misma carrera de la carpeta 04. `main` cuenta las conexiones (fiable) y los hilos cuentan visitas (no fiable). Target a `-O0`. |
| 6 | Experimento con 50 clientes, varias veces | 5 | Salen 1 o 2 visitas de 50 y casi todo el libro vacío. |
| 7 | `04_servidor_visitas_con_mutex.cpp` | 10 | Solo cambian el candado y dónde empieza y acaba la sección crítica. El `send` queda fuera. Repetir: 50 de 50 siempre. |

**Sesión 2 (110 min)**

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 0 | Repaso rápido y pizarra 5 | 10 | ¿Qué pasa si llegan 10 000 clientes? 10 000 hilos trabajando. Aforo con semáforo. |
| 1 | `05_servidor_limite_con_semaforo.cpp` | 25 | `counting_semaphore` con 3 permisos. `try_acquire_for` para no esperar para siempre y contestar "SERVIDOR LLENO". `release` obligatorio. `atomic` para el contador que solo se muestra. |
| 2 | Experimento con 10 clientes | 10 | Entran de 3 en 3 cada 2 s; el décimo se rinde a los 5 s. Discutir las cuentas en la pizarra. |
| 3 | Pizarra 6 | 5 | La lista de conectados y sus tres operaciones concurrentes. |
| 4 | `06_servidor_chat_difusion.cpp` | 30 | Vector de sockets compartido con `mtx_clientes`. Entrar (`push_back`), difundir (recorrer), salir (búsqueda con bandera en la condición del `for` y `erase`, antes de `close`). |
| 5 | `07_cliente_chat.cpp` | 20 | Por qué un hilo receptor: los mensajes llegan cuando quieren (no puedes predecir cuándo, el tema de toda la asignatura). `join` del receptor tras `SALIR`. |
| 6 | Experimento con 3 clientes y cierre | 10 | Enlace con la práctica final. |

## Preguntas a la clase antes de ejecutar

- **01:** "Con 5 clientes a la vez y 2 s de trabajo cada uno, ¿cuánto tarda el último?" 2 s (con el secuencial eran 10 s).
- **01:** "¿Qué pasaría si pasamos `ref(conexion)` al hilo?" El hilo podría leer el descriptor ya cambiado por el siguiente `accept`: dos hilos con el mismo cliente y otro cliente sin atender.
- **01:** "¿Qué pasa si `main` termina sin hacer `join`?" El programa aborta (carpeta 03). Por eso el bucle final.
- **02:** "Los 5 hilos escriben en el mismo vector `segundos`. ¿Hace falta candado?" No: cada uno en su casilla, nadie lee la casilla de otro mientras tanto.
- **03:** "50 clientes, 50 visitas. ¿Cuántas contará?" Suelen decir "alguna menos". Salen 1 o 2: casi todos los hilos leen el 0 durante ese milisegundo.
- **03:** "¿Y si quito la pausa de 1 ms?" Medido: 50 de 50 en cinco pruebas. La ventana entre leer y escribir es de nanosegundos y los clientes llegan escalonados por la red, así que casi nunca coinciden. **Sigue siendo el mismo error**, solo que ya no se ve en clase y aparecerá con carga real. La pausa representa leer un fichero o una base de datos, que es lo normal en un servidor.
- **04:** "¿Por qué el candado abarca también la pausa?" Porque leer, trabajar y escribir son una sola operación. Si se suelta en medio, vuelve la carrera.
- **04:** "¿Y por qué el `send` queda fuera?" No toca nada compartido y puede tardar: dentro del candado haría esperar a todos.
- **05:** "3 huecos, 10 clientes, 2 s de trabajo, 5 s de paciencia. ¿Cuántos serán rechazados?" Hacer las cuentas: 3 en t=0, 3 en t=2, 3 en t=4, el décimo cumple 5 s esperando: 1 rechazado.
- **05:** "¿Qué pasa si olvido `release`?" Tras 3 clientes no entra nadie más, nunca: todos acaban rechazados.
- **06:** "¿Por qué `difundir` toma el candado si solo lee la lista?" Porque otro hilo puede estar añadiendo o borrando a la vez: leer mientras otro escribe también es carrera.
- **06:** "¿Por qué se quita al cliente de la lista antes de cerrar su socket?" Si se cierra antes, otro hilo podría enviar a un descriptor cerrado, o peor, reutilizado por un cliente nuevo.
- **07:** "¿Por qué no basta con un solo hilo en el cliente?" Mientras espera en `recv`, no podría enviar; y mientras duerme para enviar, no mostraría lo que llega.

## Errores típicos de los alumnos

- Hacer `join` de cada hilo justo después de crearlo, dentro del bucle de `accept`: vuelve a ser secuencial.
- Pasar el descriptor con `ref` o capturar variables del bucle que cambian.
- Olvidar que `cout` también es compartido y ver líneas mezcladas (carpeta 04).
- Proteger el contador pero no el vector (o al revés): hay que proteger **todo** lo que forma parte de la misma operación con el mismo candado.
- Hacer `unlock` solo en uno de los caminos de un `if` y dejar el candado tomado para siempre.
- Meter `send` o `sleep` innecesarios dentro de la sección crítica.
- Olvidar `release` del semáforo en el camino de error, o hacer `release` sin haber conseguido el permiso.
- Hacer `close` del socket en `main` después de lanzar el hilo (el hilo se queda con un descriptor cerrado).
- En el chat, recorrer la lista con un índice y borrar dentro del mismo recorrido sin controlar el índice.
- Servidor sin límite de conexiones ni timeout, que no termina nunca.

## Cierre y enlace con la siguiente carpeta

Un servidor concurrente es exactamente la asignatura aplicada: no sabemos cuándo llegará cada cliente ni en qué estado estarán los datos compartidos cuando llegue, así que protegemos las secciones críticas con candados y limitamos el trabajo simultáneo con semáforos. El chat de difusión es el esqueleto de la práctica final (WhatsApp). En la carpeta 14 organizamos la aplicación en capas y guardamos los datos en SQLite, que es la persistencia que pide la práctica.

# Guion de clase: 16 ampliación, RPC y gestión distribuida de procesos

**Duración estimada:** 110 minutos (una sesión)
**Entra en examen:** No. **AMPLIACIÓN**: en dos años nunca ha dado tiempo a llegar. Está preparada aparte por si sobra una sesión o para alumnos que quieran ir más allá (y como base para la práctica final).

## Objetivo de la sesión

Dar el último paso de la asignatura: de hilos que comparten memoria a **procesos que no la comparten y se comunican por mensajes**, posiblemente en máquinas distintas. Dos ideas:

1. **RPC** (*Remote Procedure Call*): hacer que llamar a una función que se ejecuta en otro proceso parezca una llamada normal.
2. **Gestión distribuida de procesos**: un maestro reparte un trabajo entre procesos trabajadores y reúne los resultados. Es la suma paralela de la carpeta 15, cambiando hilos por procesos y `ref` por mensajes.

## Antes de empezar (preparación del profesor)

* Solo Linux (sockets POSIX). En Windows con MinGW no compila.
* Compilar la carpeta y tener cuatro terminales abiertas en `build/bin` (o `cmake-build-debug/bin` en CLion).
* Comprobar que los puertos 5600 y 5603 están libres: `ss -tlnp | grep 56`.
* Si se quiere hacer la demo con los ordenadores de los alumnos como trabajadores: averiguar la IP del ordenador del profesor (`ip a`), comprobar que la red del aula deja pasar el puerto 5603 y que los alumnos cambian `IP_MAESTRO` en `04_trabajador_remoto.cpp`.
* Repasar `red_utilidades.hpp`: son los sockets de las carpetas 12 y 13 envueltos en cinco funciones. No se teclea en clase; se proyecta y se explica en 5 minutos.

## Pizarra

1. Dos columnas. Izquierda: `resultado = sumar(3, 4);` dentro de una caja "proceso". Derecha: la misma línea, pero la caja de `sumar` en otro proceso, y entre ambas una flecha con `"SUMAR 3 4\n"` y otra de vuelta con `"OK 7\n"`.
2. Debajo, la cadena completa: **stub del cliente** (empaqueta) → **red** → **esqueleto del servidor** (desempaqueta y llama) → **función normal** → vuelta. Rodear "empaquetar y desempaquetar" y escribir al lado **serialización** (*marshalling*).
3. Tabla "lo que una llamada local nunca hace y una remota sí": fallar por la red, tardar segundos, ejecutarse sin que te enteres de la respuesta.
4. Para el maestro y los trabajadores: un círculo "maestro" con tres flechas a tres círculos "trabajador", cada una etiquetada con su rango `[0, 1M)`, `[1M, 2M)`, `[2M, 3M)`, y tres flechas de vuelta con `RESULTADO n`. Al lado, el dibujo de la carpeta 15 con hilos y `resultados[i]` para compararlos.

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 0 | `red_utilidades.hpp` (proyectado) | 5 | Nada nuevo: `socket/bind/listen/accept/connect` de las carpetas 12 y 13. Protocolo de líneas porque TCP es un flujo. `SO_RCVTIMEO` para no colgarse. `MSG_NOSIGNAL`. |
| 1 | `01_servidor_rpc_calculadora.cpp` | 20 | Las funciones `sumar`, `restar`, `multiplicar` son normales. `ejecutar_peticion` es el esqueleto: texto a llamada. Cadena `if / else if` en lugar de `switch` (norma). Errores de formato. |
| 2 | `02_cliente_rpc_calculadora.cpp` | 20 | `llamar_remoto` empaqueta, envía, espera y desempaqueta. Los stubs tienen forma de función local. `main` no contiene ni un socket. Por qué devuelven `bool`. |
| | Discusión: serialización y fallos | 20 | Ver apartados siguientes. Sin fichero: pizarra y preguntas. |
| 3 | `03_maestro_reparte_trabajo.cpp` | 20 | Reunir trabajadores con espera máxima. Enviar todos los encargos antes de leer ninguna respuesta (si no, sería secuencial). Qué pasa si un trabajador no responde. |
| 4 | `04_trabajador_remoto.cpp` | 15 | Proceso independiente con su propio PID. Reintentos de conexión acotados. Bucle con bandera `seguir`. |
| | Cierre | 10 | Tabla hilos frente a procesos frente a distribuido. |

## Lo que se explica en la pizarra (sin fichero)

**Serialización.** Para que los datos viajen hay que convertirlos en bytes y reconstruirlos al otro lado. Aquí usamos texto separado por espacios porque se puede leer con `nc` y depurar a simple vista. Problemas a comentar:

* Un argumento con espacios (`"GUARDAR Juan Pérez"`) rompe el formato: hace falta escapar, entrecomillar o enviar la longitud delante.
* Enviar números en binario es más compacto, pero entonces importa el orden de bytes (`htonl` / `ntohl`) y el tamaño de `int` en cada máquina.
* El servidor nunca debe fiarse de lo que recibe: `SUMAR 3`, `POTENCIA 2 3` y `hola` deben dar `ERROR`, no reventar el programa. Se puede demostrar en directo con `nc 127.0.0.1 5600` escribiendo a mano.

**Semántica ante fallos.** El cliente envía `INGRESAR 100` a un banco y no llega respuesta en 5 segundos. ¿Se hizo el ingreso? No lo sabe. Tres opciones:

| Semántica | Qué hace el cliente | Riesgo |
|---|---|---|
| Como mucho una vez (*at most once*) | No reintenta | La operación pudo hacerse y el cliente cree que falló |
| Al menos una vez (*at least once*) | Reintenta hasta tener respuesta | Ingreso duplicado si la primera sí llegó |
| Exactamente una vez (en la práctica) | Reintenta enviando un identificador único de petición; el servidor recuerda los identificadores ya procesados y no los repite | Hay que guardar ese registro (y protegerlo con mutex si el servidor es concurrente) |

Idea clave: una operación **idempotente** (`CONSULTAR_SALDO`, `PONER_SALDO 500`) se puede reintentar sin miedo; una no idempotente (`INGRESAR 100`) no.

**Servidor RPC concurrente.** `01` atiende clientes de uno en uno. Convertirlo en concurrente es exactamente la carpeta 13: un `thread` por conexión que ejecute el bucle interior, un `vector<thread>` y `join` de todos al final. Si el servidor lleva un contador de llamadas o un saldo, ese dato pasa a ser compartido: `mtx.lock()` y `mtx.unlock()` alrededor. El cliente `02` no cambia en absoluto: RPC esconde si el servidor es secuencial o concurrente.

**Lanzar los trabajadores con fork.** En lugar de abrir tres terminales, el maestro puede crear los trabajadores: abre primero el socket de escucha, hace tres `fork()` (bandera como en `15/06_fork_esperar_hijos_waitpid.cpp`), cada hijo ejecuta el código del trabajador y conecta con `127.0.0.1`, y al final el padre hace `waitpid` de los tres. Así se gestionan procesos locales y remotos con el mismo protocolo.

**RPC de verdad.** Nadie escribe los stubs a mano en un proyecto real: se describen las funciones en un fichero de interfaz y una herramienta genera stubs y esqueletos (ONC RPC con `rpcgen`, gRPC con Protocol Buffers, Java RMI). Solo se menciona; en esta asignatura no se usa ninguna biblioteca externa.

## Cómo lanzarlo

**Calculadora (dos terminales):**

```
# Terminal 1
./01_servidor_rpc_calculadora
# Terminal 2
./02_cliente_rpc_calculadora
```

Probar a mano el protocolo en una tercera terminal: `nc 127.0.0.1 5600` y escribir `SUMAR 2 2`, `DIVIDIR 1 0`, `FIN`.

**Maestro y trabajadores (cuatro terminales):**

```
# Terminal 1
./03_maestro_reparte_trabajo
# Terminales 2, 3 y 4
./04_trabajador_remoto
```

O todo en una: `./03_maestro_reparte_trabajo & for i in 1 2 3; do ./04_trabajador_remoto & done; wait`

**En CLion:** crear una configuración de ejecución por programa y lanzarlas una tras otra; cada una abre su pestaña en el panel *Run*. Para varios trabajadores, marcar *Allow multiple instances* en la configuración de `04_trabajador_remoto`.

Verificado: con 3 trabajadores se obtienen 78498, 70435 y 67883 primos (total 216816); con 2, el maestro espera sus 20 s por el tercero y reparte en dos mitades con el mismo total. Solos, el servidor termina a los 30 s, el maestro a los 20 s y el trabajador se rinde a los 5 s.

## Preguntas a la clase antes de ejecutar

* **01**: "Arranco el servidor y no lanzo ningún cliente. ¿Se queda colgado?" No: a los 30 s `accept` devuelve -1 y termina.
* **02**: "¿Dónde se calcula `6 * 7`?" En el otro proceso: mirar su terminal. "¿Y si apago el servidor a mitad?" Los stubs devuelven `false`; una llamada local nunca haría eso.
* **02**: "¿Por qué `dividir_remoto(7, 0, r)` no revienta el cliente?" Porque el servidor responde `ERROR` y el stub lo traduce a `false`.
* **03**: "Si el maestro enviara el encargo al trabajador 0, esperara su respuesta, y luego enviara al 1, ¿cuánto tardaría?" Lo mismo que un solo trabajador: sería secuencial.
* **03**: "Los tres rangos tienen el mismo tamaño. ¿Tardan lo mismo?" No: comprobar si un número grande es primo cuesta más, el último trabajador es el más lento. Repartir "a partes iguales" no siempre es repartir el trabajo por igual.
* **04**: "¿Hace falta un mutex para sumar los resultados en el maestro?" No: los trabajadores no tocan la memoria del maestro; solo el maestro suma, en un único hilo.

## Errores típicos de los alumnos

* Olvidar el `\n` al enviar: el otro extremo se queda esperando el fin de línea hasta que vence la espera.
* Suponer que un `recv` devuelve un mensaje entero. Por eso se lee hasta `\n`.
* Lanzar el cliente antes que el servidor y pensar que el programa está mal.
* Dejar un servidor de una ejecución anterior vivo y obtener "No se pudo abrir el puerto". `ss -tlnp` y cerrarlo.
* Parsear con `stoi` directamente lo recibido: si llega basura, lanza una excepción y el servidor cae. Validar primero (aquí, con `istringstream`).
* Enviar y recibir de cada trabajador por turnos en el maestro, perdiendo todo el paralelismo.
* En la demo en red: cortafuegos del aula o IP equivocada (poner `127.0.0.1` en un ordenador que no es el del profesor).

## Cierre

| | Hilos (03 a 09) | Procesos (15) | Distribuido (16) |
|---|---|---|---|
| Memoria | Compartida | Copiada al hacer fork | Ninguna en común |
| Esperar el final | `join` | `waitpid` | Mensaje `FIN` o cierre del socket |
| Riesgo principal | Condiciones de carrera | Comunicarse | Fallos parciales y latencia |
| Herramienta | mutex, semáforos | código de salida, tuberías | sockets y protocolo |

Enlace con la práctica final (carpeta 99): el servidor de mensajería es un servidor concurrente (13) con capas y SQLite (14) y un protocolo de líneas como el de aquí.

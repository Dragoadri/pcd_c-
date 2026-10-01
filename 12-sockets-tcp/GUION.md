# Guion de clase: 12 sockets TCP

**Duración estimada:** 165 minutos (1 sesión y media: ~110 min + ~55 min al inicio de la siguiente)
**Entra en examen:** Parcial 2

## Objetivo de la sesión

Escribir un servidor y un cliente TCP, entender las diferencias de uso con UDP (conexión, `accept` que devuelve un socket nuevo, fallo inmediato si no hay servidor), descubrir que **TCP es un flujo de bytes sin límites de mensaje** y resolverlo con un protocolo de líneas, y **ver con tiempos reales** que un servidor secuencial deja esperando a todos. Ese último punto es el puente con la concurrencia de la carpeta 13.

## Antes de empezar (preparación del profesor)

- Compilado: `cmake --build build --target 01_servidor_tcp_eco 02_cliente_tcp_eco 03_servidor_tcp_por_lineas 04_cliente_tcp_por_lineas 05_servidor_tcp_secuencial 06_cliente_tcp_lento`.
- Dos o tres terminales visibles. Para la última pareja conviene un tercer terminal.
- `nc` instalado para probar el servidor de eco a mano: `nc 127.0.0.1 5200`.

## Pizarra

1. Analogía: **TCP = llamada telefónica**. Primero se marca y se descuelga (`connect` y `accept`), luego se habla en orden, al final se cuelga (`close`, el otro ve `recv == 0`).
2. Columna **servidor**: `socket` , `bind` , `listen` , `accept` , bucle { `recv` , `send` } , `close`.
   Columna **cliente**: `socket` , `connect` , bucle { `send` , `recv` } , `close`.
3. Dibujo clave: el servidor tiene **una centralita** (socket de escucha) y por cada llamada descolgada **un teléfono nuevo** (el socket que devuelve `accept`). Con varios clientes habrá varios teléfonos a la vez.
4. Tabla UDP frente a TCP (la dejo toda la sesión en una esquina):

| | UDP (carpeta 11) | TCP (esta carpeta) |
|---|---|---|
| Conexión | No hay: cada `sendto` lleva destino | Sí: `connect` / `listen` + `accept` |
| Sin servidor | `sendto` no da error | `connect` falla al instante (Connection refused) |
| Fiabilidad | Se puede perder sin aviso | Llega todo o hay error |
| Orden | No garantizado | Garantizado |
| Límites de mensaje | Sí: un `recvfrom` = un datagrama | **No**: flujo de bytes, hay que delimitar |
| Coste | Ligero, sin esperas de confirmación | Más pesado (conexión, confirmaciones, retransmisiones) |
| Sockets en el servidor | Uno para todos los clientes | Uno de escucha + uno por cliente |
| Usos | DNS, voz, vídeo en directo, juegos | Web, correo, SSH, chats, ficheros |

5. Para la pareja 03/04: una tubería con letras entrando por un lado `uno\n` `dos\n` `tr` `es\nFIN\n` y saliendo por el otro en dos grumos. Frase: **"un recv no es un mensaje"**. Solución: acordar un separador (`'\n'`) = **protocolo**.
6. Para la pareja 05/06: línea de tiempo con dos clientes. Cliente A: conecta en 0, habla en 3, respuesta en 5. Cliente B: conecta en 0, espera, respuesta en 7. "Todos pagan la lentitud del primero".

## Cómo lanzarlo en dos terminales

Siempre **primero el servidor**. Desde la carpeta del proyecto:

```
# Pareja 1
./build/bin/01_servidor_tcp_eco          # terminal 1
./build/bin/02_cliente_tcp_eco           # terminal 2

# Pareja 2
./build/bin/03_servidor_tcp_por_lineas   # terminal 1
./build/bin/04_cliente_tcp_por_lineas    # terminal 2

# Pareja 3: dos clientes casi a la vez
./build/bin/05_servidor_tcp_secuencial   # terminal 1
./build/bin/06_cliente_tcp_lento         # terminal 2
./build/bin/06_cliente_tcp_lento         # terminal 3 (enseguida)
# o bien, en un solo terminal:
./build/bin/06_cliente_tcp_lento & ./build/bin/06_cliente_tcp_lento
```

En CLion: Run del servidor, luego elegir el cliente en el desplegable y Run. Para lanzar el mismo cliente dos veces, *Edit Configurations*, marcar *Allow multiple instances*. Los ejecutables están en `cmake-build-debug/bin/`; el Terminal integrado de CLion se puede dividir en dos.

Si al relanzar un servidor sale "Address already in use", es que el anterior sigue vivo: `ss -tlpn | grep 52` para verlo.

Entre dos ordenadores: `IP_ESCUCHA = "0.0.0.0"` en el servidor e `IP_SERVIDOR` con la IP de la wifi del servidor en el cliente.

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 0 | Pizarra (puntos 1 a 4) | 15 | Llamada telefónica, centralita y teléfonos, tabla UDP/TCP. |
| 1 | `01_servidor_tcp_eco.cpp` | 30 | Lo nuevo respecto a UDP: `listen`, `accept` con su socket nuevo (imprimimos los dos descriptores), `recv == 0` significa "ha colgado", `MSG_NOSIGNAL`. `SO_RCVTIMEO` en escucha y en conexión. |
| 2 | `02_cliente_tcp_eco.cpp` | 20 | `connect`. Ejecutarlo primero SIN servidor (Connection refused al instante) y comparar con UDP. Luego con servidor. Probar también `nc 127.0.0.1 5200` contra el servidor. |
| 3 | Pizarra (punto 5) | 5 | La tubería: un `recv` no es un mensaje. |
| 4 | `03_servidor_tcp_por_lineas.cpp` | 25 | Acumular en `pendiente`, buscar `'\n'` con `find`, sacar la línea con `substr`. Imprimimos cada trozo tal como llega. |
| 5 | `04_cliente_tcp_por_lineas.cpp` | 20 | `enviar_todo` (send puede enviar menos). Envíos troceados a propósito. Ver en el servidor el trozo pegado de 10 bytes y las líneas bien reconstruidas. |
| 6 | Pizarra (punto 6) | 5 | Línea de tiempo de dos clientes. |
| 7 | `05_servidor_tcp_secuencial.cpp` | 25 | `recibir_linea` de byte en byte (sencilla, se reutiliza en la 13). Bucle de `accept` acotado. Los tiempos `t=`. |
| 8 | `06_cliente_tcp_lento.cpp` | 15 | El cliente se piensa el nombre 3 s. Lanzar dos a la vez: 5 s y 7 s. |
| 9 | Cierre | 5 | ¿Cómo atendemos a los dos a la vez? Con lo del primer parcial: hilos. |

## Preguntas a la clase antes de ejecutar

- **01:** "¿El servidor usará el mismo socket para escuchar y para hablar con el cliente?" No: `accept` devuelve otro descriptor (sale 3 y 4).
- **02 sin servidor:** "En UDP no había error. ¿Y ahora?" Error inmediato: `Connection refused`. Hay alguien que contesta "no existe ese número".
- **02:** "Si el cliente hace `close`, ¿cómo se entera el servidor?" `recv` devuelve 0.
- **03/04:** "El cliente hace cuatro `send`. ¿Cuántos `recv` con datos hará el servidor?" Casi todos dicen cuatro. Salen dos, y el primero trae tres envíos pegados y medio.
- **03/04:** "¿Por qué no pasaba en el eco del 01?" Porque el cliente esperaba la respuesta entre envío y envío; en cuanto hay prisa o red de verdad, se pegan.
- **05/06:** "Lanzo dos clientes a la vez y cada uno tarda 3 s en hablar más 2 s de trabajo. ¿Cuánto tardará cada uno?" Muchos dicen 5 y 5. Salen 5 y 7.
- **05/06:** "¿Por qué el segundo `connect` no falla si el servidor está ocupado?" Porque el sistema operativo acepta la conexión y la deja en la cola de `listen`; el programa aún no ha hecho `accept`.
- **05/06:** "¿Y con 100 clientes lentos?" El último esperaría minutos. Con un servidor de verdad, inaceptable.

## Errores típicos de los alumnos

- Usar el socket de escucha para `recv`/`send` en vez del que devuelve `accept`.
- Olvidar `listen` (el `accept` falla con "Invalid argument").
- Tratar cada `recv` como un mensaje completo: funciona en casa, falla en la entrega.
- Suponer que `send` envía siempre todo: por eso `enviar_todo`.
- Imprimir `buffer` directamente sin respetar `n` (no hay `'\0'`): usar `string(buffer, n)`.
- No distinguir `recv == 0` (colgó) de `recv == -1` (error o timeout) y quedarse en bucle infinito.
- Morir sin mensaje al escribir en un socket que el otro ya cerró (SIGPIPE) por no usar `MSG_NOSIGNAL`.
- Olvidar cerrar el socket de conexión de cada cliente: el servidor se queda sin descriptores.
- Arrancar el cliente antes que el servidor, o dos servidores en el mismo puerto.

## Cierre y enlace con la siguiente carpeta

Ya sabemos hablar por TCP con un protocolo de líneas, y hemos medido el problema: un servidor secuencial convierte la lentitud de un cliente en la espera de todos. En la carpeta 13 aplicamos el primer parcial: **un hilo por conexión**, con candados para lo que los hilos compartan y semáforos para no aceptar más clientes de los que podemos atender.

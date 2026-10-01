# Guion de clase: 11 sockets UDP

**Duración estimada:** 110 minutos (1 sesión)
**Entra en examen:** Parcial 2

## Objetivo de la sesión

Escribir el primer servidor y el primer cliente de red, con UDP, y comprobar en pantalla sus dos rasgos: **no hay conexión** (se envía sin saber si alguien escucha) y **no hay garantía de entrega** (se pierden datagramas sin ningún error).

## Antes de empezar (preparación del profesor)

- Proyecto compilado: `cmake -S . -B build && cmake --build build --target 01_servidor_udp_eco 02_cliente_udp_eco 03_receptor_udp_sin_garantias 04_emisor_udp_sin_conexion`.
- Dos terminales visibles a la vez en el proyector (uno a cada lado de la pantalla).
- Opcional: un segundo portátil en la misma red para repetir la ráfaga entre dos equipos.
- Recordar a quien use Windows con MinGW: estos ejemplos necesitan Linux, WSL o macOS.

## Pizarra

1. Recuperar la analogía: **UDP = cartas**. Dibujo un buzón, un sobre con remitente (IP y puerto del cliente) y destinatario (IP y puerto del servidor).
2. Columna **servidor**: `socket` , `bind` , bucle { `recvfrom` , `sendto` } , `close`.
3. Columna **cliente**: `socket` , `sendto` , `recvfrom` , `close`. Rodeo con un círculo que el cliente **no hace bind** ni `connect`.
4. Debajo: `recvfrom` nos dice **quién** escribió; `sendto` necesita que digamos **a quién**. Cada datagrama lleva su dirección, como cada carta.
5. Recuadro "no se cuelga": `SO_RCVTIMEO` = tiempo máximo de espera. Sin él, `recvfrom` esperaría para siempre.
6. Al final: un embudo (el búfer del sistema) lleno y sobres cayendo al suelo. "Si el receptor no da abasto, el sistema tira cartas y no avisa a nadie".

## Cómo lanzarlo en dos terminales

Desde la carpeta del proyecto, después de compilar:

```
# Terminal 1 (arrancar SIEMPRE primero el servidor)
./build/bin/01_servidor_udp_eco

# Terminal 2 (se puede lanzar dos veces: el servidor atiende 6 mensajes)
./build/bin/02_cliente_udp_eco
```

```
# Terminal 1
./build/bin/03_receptor_udp_sin_garantias
# Terminal 2
./build/bin/04_emisor_udp_sin_conexion
```

En CLion: ejecutar el servidor con el botón Run; después elegir el cliente en el desplegable y pulsar Run otra vez. Cada programa abre su propia pestaña en la ventana Run. Si aparece el aviso "ya se está ejecutando", activar en *Edit Configurations* la opción *Allow multiple instances*. También sirve el Terminal integrado (View, Tool Windows, Terminal), dividido en dos.

Desde CLion los ejecutables están en `cmake-build-debug/bin/`.

Prueba rápida sin cliente escrito: `nc -u 127.0.0.1 5100`, escribir texto y pulsar Enter.

Para dos ordenadores: en el servidor, `IP_ESCUCHA = "0.0.0.0"`; en el cliente, `IP_SERVIDOR` = la IP de la wifi del servidor (`ip -4 a`).

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 0 | Pizarra | 15 | Cartas, columnas servidor y cliente, `SO_RCVTIMEO`. |
| 1 | `01_servidor_udp_eco.cpp` | 30 | Paso a paso: `socket`, `setsockopt`, rellenar `sockaddr_in` con `htons` e `inet_pton` (carpeta 10), `bind` y el cast `(sockaddr*)`. Bucle acotado con bandera `seguir` y contador. `recvfrom` no pone el `'\0'`. Lo arranco y lo dejo esperando. |
| 2 | `02_cliente_udp_eco.cpp` | 20 | Sin `bind`. `sendto` con destino. Ejecutar dos veces: en el servidor aparecen dos puertos de cliente distintos. Después, parar el servidor y ejecutar el cliente solo. |
| 3 | `03_receptor_udp_sin_garantias.cpp` | 20 | Mismo esqueleto que el 01. El receptor "trabaja" 1 ms por datagrama. Contamos recibidos, huecos y desorden. |
| 4 | `04_emisor_udp_sin_conexion.cpp` | 15 | Ráfaga de 2000. Primero SIN receptor: "2000 enviados sin error". Luego con receptor: recibe unos 220. Cambiar `MS_TRABAJO` a 0 y repetir. |
| 5 | Cierre | 10 | Cuándo compensa UDP (voz, juegos, DNS) y cuándo no (un chat). Enlace con TCP. |

## Preguntas a la clase antes de ejecutar

- **01:** "Si ejecuto el servidor y no lanzo ningún cliente, ¿qué pasa?" Espera 30 s y termina (sin el timeout se quedaría colgado para siempre).
- **01:** "¿Puedo arrancar dos veces el servidor a la vez?" No: el segundo `bind` falla con "Address already in use".
- **02:** "Si lanzo el cliente dos veces, ¿verá el servidor el mismo puerto de cliente?" No, el sistema asigna uno libre en cada ejecución.
- **02 sin servidor:** "¿Dará error `sendto`?" No. Devuelve 4, como si todo fuera bien. Solo el timeout de la respuesta nos avisa. Es la carta echada a un buzón que nadie mira.
- **04 sin receptor:** "¿Cuántos dirá que ha enviado sin error?" Los 2000.
- **03 y 04 juntos:** "Estamos en el mismo ordenador, sin cables. ¿Cuántos llegarán?" Casi todos dicen 2000. Llegan unos 220: el búfer del sistema se llena mientras el receptor trabaja y el resto se descarta sin avisar.
- **03 con `MS_TRABAJO = 0`:** "¿Y ahora?" Llegan casi todos, a veces se pierde alguno. Entre dos equipos por wifi, además, pueden llegar desordenados.

## Errores típicos de los alumnos

- Arrancar el cliente antes que el servidor y pensar que el código está mal (con UDP ni siquiera hay error).
- Olvidar `htons` en el puerto o poner la IP como número en vez de usar `inet_pton`.
- Imprimir el buffer recibido sin añadir `'\0'` en la posición `n`: aparece basura al final.
- Pasar `sizeof(buffer)` a `recvfrom` y luego escribir `buffer[n] = '\0'` con `n == sizeof(buffer)` (desbordamiento): por eso se pasa `sizeof(buffer) - 1`.
- Contestar con `sendto` a una dirección fija en vez de a la que devolvió `recvfrom`.
- No poner `SO_RCVTIMEO` y quedarse con el programa colgado en `recvfrom`.
- Relanzar el servidor al instante y chocar con el puerto ocupado por olvidar `SO_REUSEADDR` o por tener aún el anterior en marcha (`ss -ulpn` lo muestra).
- Creer que "en localhost UDP es fiable" porque con un receptor rápido llegan todos.

## Cierre y enlace con la siguiente carpeta

UDP es ligero y rápido, pero el programador tiene que asumir que los mensajes pueden perderse o llegar desordenados. Para un chat o una transferencia queremos que llegue todo y en orden: eso es TCP, la llamada telefónica, que empieza en la carpeta 12. Veremos que TCP garantiza la entrega pero trae su propia sorpresa: no respeta los límites de los mensajes.

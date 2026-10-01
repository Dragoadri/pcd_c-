# Apuntes de redes para programar sockets

Lo imprescindible para entender las carpetas 11, 12 y 13. No es una asignatura de redes: es el vocabulario mínimo para que un programa hable con otro.

La analogía que usamos en todo el bloque: **Internet es una ciudad de edificios con buzones**.

| Red | Ciudad |
|---|---|
| Ordenador (host) | Edificio |
| Dirección IP | Dirección postal del edificio |
| Puerto | Número de buzón dentro del edificio |
| Programa servidor | Vecino que ha puesto su nombre en un buzón y lo mira |
| DNS | Guía telefónica: nombre del edificio a dirección |
| Router con NAT | Portería de una urbanización con una sola dirección hacia fuera |
| UDP | Cartas: se echan al buzón y no sabes si llegan |
| TCP | Llamada telefónica: primero se establece la llamada, luego se habla en orden |

---

## 1. Dirección IP

Una **dirección IP** identifica una interfaz de red de un equipo dentro de una red. Es lo que un programa necesita para saber *a qué máquina* enviar datos.

- **IPv4**: 4 números de 0 a 255 separados por puntos, por ejemplo `192.168.1.10`. Son 4 bytes (32 bits), unos 4 300 millones de direcciones: se agotaron hace años.
- **IPv6**: 128 bits escritos en hexadecimal, por ejemplo `2001:db8::1`. Existe porque IPv4 se quedó corto.
- En la asignatura usamos **solo IPv4**: es más fácil de leer y es lo que usan los ejemplos (`AF_INET`).

Un ordenador tiene **varias IPs**, una por interfaz (cable, wifi, interfaces virtuales de Docker o de máquinas virtuales). Lo comprobamos con `01_mis_direcciones_ip.cpp` y con `ip -4 a`.

## 2. Máscara y red local

La **máscara** dice qué parte de la IP identifica a la red y qué parte al equipo.

- IP `192.168.1.10` con máscara `255.255.255.0` (también escrito `/24`): la red es `192.168.1.x` y el equipo es el `10`.
- Dos equipos con la misma parte de red se hablan **directamente** (misma red local). Si no, los paquetes van al **router** (puerta de enlace), que los reenvía.

Consecuencia práctica en clase: dos portátiles conectados a la misma wifi de la universidad pueden probar cliente y servidor entre ellos si la red lo permite (algunas wifis aíslan a los clientes entre sí; si no conecta, no es culpa de vuestro código).

## 3. 127.0.0.1 y localhost

- `127.0.0.1` es la **interfaz de bucle local** (loopback, `lo`): una red de mentira dentro del propio equipo. Lo que envías a `127.0.0.1` nunca sale del ordenador.
- `localhost` es el **nombre** de esa dirección.
- Todos los ejemplos del curso usan `127.0.0.1` por defecto: servidor y cliente en el mismo portátil, en dos terminales. Para probar entre dos ordenadores, se cambia la constante de IP del cliente por la IP de la wifi o el cable del servidor, y el servidor escucha en `0.0.0.0` (todas sus interfaces).

## 4. Puerto

La IP lleva los datos al edificio; el **puerto** los lleva al buzón del programa correcto. Es un número de 16 bits (0 a 65535).

- Un **servidor** reserva un puerto con `bind` y se queda esperando en él. Dos programas no pueden reservar el mismo puerto a la vez: el segundo recibe el error `EADDRINUSE` ("Address already in use").
- Un **cliente** normalmente no elige puerto: el sistema le asigna uno libre y temporal (puerto efímero, típicamente por encima de 32768). Por eso el servidor ve un puerto distinto en cada conexión de cliente.
- **Puertos bien conocidos** (0 a 1023): reservados a servicios estándar y hace falta ser administrador para usarlos. Algunos: 22 SSH, 53 DNS, 80 HTTP, 443 HTTPS. La lista está en `/etc/services`.
- En el curso usamos puertos de 5100 a 5399, fuera de la zona reservada y fáciles de recordar.

Un **socket** queda identificado por la pareja IP y puerto. Una **conexión TCP**, por cuatro datos: IP y puerto del cliente, IP y puerto del servidor.

## 5. DNS

Las personas recordamos nombres (`www.universidadeuropea.com`); los programas conectan a IPs. El **DNS** (Domain Name System) es el servicio que traduce nombres a IPs.

- `localhost` y los nombres de `/etc/hosts` los resuelve el propio equipo, sin red.
- El resto se pregunta a un servidor DNS (normalmente el que da el router).
- Un nombre puede tener **varias IPs** (reparto de carga) y cambiar con el tiempo.
- En C++ se resuelve con `getaddrinfo` (ver `02_resolver_nombre_dns.cpp`). Si no hay red, falla con un mensaje y el programa debe seguir funcionando.

## 6. NAT y el router de casa

En casa tienes muchos dispositivos, pero tu proveedor te da **una sola IP pública**. El router hace **NAT** (traducción de direcciones):

- Dentro de casa, IPs **privadas**: `192.168.x.x`, `10.x.x.x`, `172.16.x.x` a `172.31.x.x`. No son alcanzables desde Internet.
- Cuando un equipo sale a Internet, el router cambia la IP privada por la pública y apunta la conversación para devolver la respuesta al equipo correcto.
- Consecuencia: **un cliente de fuera no puede conectar a un servidor que tengas en tu portátil** en casa, salvo que configures el router (redirección de puertos). Por eso en clase probamos en la misma red local o en `127.0.0.1`.

## 7. Modelo cliente/servidor

- **Servidor**: arranca primero, reserva un puerto conocido y espera peticiones. No sabe quién va a llegar ni cuándo.
- **Cliente**: sabe la IP y el puerto del servidor, inicia la comunicación, pide algo y recibe la respuesta.
- Un mismo programa puede ser cliente y servidor a la vez (la práctica del WhatsApp lo roza).
- El orden importa: **si arrancas el cliente antes que el servidor, falla**. Con TCP falla al conectar ("Connection refused"); con UDP ni siquiera se entera (ver carpeta 11).

Aquí aparece otra vez la concurrencia: un servidor con varios clientes es un **problema concurrente por naturaleza** (UA1_T01): los clientes llegan cuando quieren y no puedes predecir el orden. Por eso este bloque viene después de hilos, candados y semáforos.

## 8. TCP frente a UDP (idea)

| | UDP | TCP |
|---|---|---|
| Analogía | Cartas | Llamada telefónica |
| Conexión previa | No | Sí (`connect` y `accept`) |
| Llega todo | No garantizado | Sí, o te enteras del error |
| Llega en orden | No garantizado | Sí |
| Límites de mensaje | Sí: cada datagrama llega entero o no llega | No: es un flujo de bytes continuo |
| Coste | Muy ligero | Más pesado (confirmaciones, retransmisiones) |
| Usos típicos | DNS, streaming, juegos, voz | Web, correo, SSH, chats, transferencias |

Se ve en detalle y con código en `12-sockets-tcp/GUION.md`.

## 9. Orden de bytes de red

Un número de más de un byte se puede guardar en memoria de dos maneras:

- **Little endian**: primero el byte menos significativo. Es lo que hacen los PC Intel y AMD y los Mac actuales.
- **Big endian**: primero el byte más significativo. Es el **orden de red**, el que exigen las cabeceras de los protocolos.

Por eso, antes de meter un puerto en una dirección de socket, se convierte:

- `htons` (host to network short): puerto de la máquina a orden de red.
- `ntohs` (network to host short): al revés, para imprimir el puerto de un cliente.
- `inet_pton` / `inet_ntop`: IP de texto a binario (ya en orden de red) y de vuelta.

Error clásico: olvidar `htons(5000)`. El servidor acaba escuchando en el puerto 34835 y el cliente no lo encuentra. Lo muestra `03_ip_y_puerto_en_binario.cpp`.

## 10. Comandos útiles en el terminal (Linux)

| Comando | Para qué |
|---|---|
| `hostname` | Nombre del equipo |
| `ip -4 a` | Interfaces y sus IPv4 |
| `ip route` | Puerta de enlace (router) |
| `ping -c 3 127.0.0.1` | ¿Responde ese equipo? (usa ICMP, no puertos) |
| `getent hosts www.universidadeuropea.com` | Resolver un nombre como lo hace un programa |
| `host www.universidadeuropea.com` | Preguntar al DNS directamente (paquete `bind9-host`) |
| `ss -tulpn` | Qué puertos TCP (t) y UDP (u) están escuchando (l) y qué programa los tiene (p) |
| `ss -tn` | Conexiones TCP establecidas ahora |
| `nc -l 127.0.0.1 5200` | Servidor TCP de prueba que imprime lo que le llega |
| `nc 127.0.0.1 5200` | Cliente TCP de prueba: lo que escribes se envía |
| `nc -u 127.0.0.1 5100` | Cliente UDP de prueba |

`nc` (netcat) es la navaja suiza: sirve para probar un servidor antes de tener escrito el cliente, o al revés.

## 11. Llamadas POSIX que vamos a usar

| Llamada | Quién | Qué hace |
|---|---|---|
| `socket` | ambos | Crea el socket (UDP: `SOCK_DGRAM`, TCP: `SOCK_STREAM`) |
| `setsockopt` | ambos | Opciones: `SO_REUSEADDR`, `SO_RCVTIMEO` (tiempo máximo de espera) |
| `bind` | servidor | Reserva IP y puerto |
| `listen` | servidor TCP | Marca el socket como "acepto llamadas" |
| `accept` | servidor TCP | Espera un cliente y devuelve un socket nuevo para hablar con él |
| `connect` | cliente TCP | Llama al servidor |
| `sendto` / `recvfrom` | UDP | Enviar o recibir un datagrama indicando o averiguando el otro extremo |
| `send` / `recv` | TCP | Enviar o recibir bytes por la conexión |
| `close` | ambos | Cierra el socket (colgar el teléfono) |

Todas devuelven `-1` si fallan y dejan el motivo en `errno`; `perror("nombre")` lo imprime en lenguaje humano. **Comprobar siempre el valor devuelto.**

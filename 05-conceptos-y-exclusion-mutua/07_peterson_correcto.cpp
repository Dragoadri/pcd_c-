// =====================================================================
// Tema 5.7  |  Algoritmo de Peterson: banderas + turno (LECTURA, NO EXAMINABLE)
// En este programa he protegido un int normal con el algoritmo de Peterson (banderas quiere y turno), para ver que el contador sale exacto.
// Requisitos previos: 05/05, 05/06, LECTURA_cuatro_intentos.md
// Qué observar al ejecutar: contador exacto (40000) y 0 veces con dos hilos dentro, pese a usar un int normal.
// Salida determinista: SI (el resultado final; el orden interno de entradas varía)
// Compilar con: -O0 (el contador es un int normal, igual que en 04)
// =====================================================================
// quiere[yo] señaliza la intención; turno desempata cediendo al otro.
// Ceder el turno rompe la simetría: evita el interbloqueo del intento 3 y
// el livelock del intento 4.
// Espera activa: gasta CPU mientras espera. El yield() dentro del bucle es
// cortesía con el planificador (en un solo núcleo, sin él, cada espera se
// comería la rodaja entera); sigue siendo espera activa.
// TRAMPA 3: sin atomic y seq_cst el algoritmo deja de ser válido.
// Por eso en C++ real se usa mutex (carpeta 06): bloquea sin gastar CPU.
#include <iostream>   // cout
#include <thread>     // thread y this_thread::yield
#include <atomic>     // atomic y memory_order_seq_cst

using namespace std;

const int VECES = 20000;
const long MAX_VUELTAS = 10000000;   // no debería alcanzarse nunca
atomic<bool> quiere[2];              // quiere[i] = true si el proceso i quiere entrar
atomic<int> turno(0);                // a quién le toca cuando los dos quieren a la vez
atomic<int> dentro(0);               // cuántos hilos hay en la sección crítica ahora mismo
atomic<int> violaciones(0);
atomic<int> esperas_agotadas(0);
int contador = 0;                    // int normal: lo protege Peterson

void proceso(int yo) {
    int otro = 1 - yo;   // si yo soy 0 el otro es 1, y al revés
    for (int i = 0; i < VECES; i++) {
        // store escribe la variable atómica; memory_order_seq_cst obliga a
        // que los dos hilos vean las escrituras en el orden real.
        quiere[yo].store(true, memory_order_seq_cst);    // PREPROTOCOLO
        turno.store(otro, memory_order_seq_cst);         // cedo la preferencia
        long vueltas = 0;
        // Espero SOLO si se cumplen las tres cosas: el otro quiere entrar, el
        // turno es suyo y aún no he agotado las vueltas. Si el otro no quiere,
        // o el turno me lo ha cedido él, entro.
        while (quiere[otro].load(memory_order_seq_cst)
               && turno.load(memory_order_seq_cst) == otro && vueltas < MAX_VUELTAS) {
            this_thread::yield();   // yield: cedo la CPU a otro hilo, pero sigo esperando
            vueltas++;
        }
        if (vueltas == MAX_VUELTAS) {   // he salido por agotar vueltas: Peterson no ha dejado progresar
            esperas_agotadas++;
        }
        dentro++;                                             // SECCIÓN CRÍTICA (empieza)
        if (dentro.load() > 1) {   // load lee el valor: más de uno dentro sería un fallo
            violaciones++;
        }
        contador++;   // lo único compartido que no es atomic
        dentro--;                                             // (acaba la sección crítica)
        quiere[yo].store(false, memory_order_seq_cst);   // POSTPROTOCOLO
    }
}

int main() {
    thread p1(proceso, 0);   // el 0 llega a proceso como el parámetro yo
    thread p2(proceso, 1);
    p1.join();   // join: main espera a que termine cada proceso
    p2.join();
    cout << "Contador: " << contador << " (esperado " << 2 * VECES << ")\n";
    cout << "Veces con dos hilos dentro: " << violaciones << "\n";
    cout << "Esperas agotadas (progreso bloqueado): " << esperas_agotadas << "\n";
    return 0;   // 0 = el programa ha terminado bien
}

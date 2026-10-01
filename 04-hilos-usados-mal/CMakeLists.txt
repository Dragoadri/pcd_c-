# 04-hilos-usados-mal: el problema antes que la solución.
# Todos a -O0 (pcd_ejemplo_sin_optimizar): con -O2 el compilador puede
# convertir un bucle de un millón de incrementos en una sola suma y la
# carrera deja de verse, aunque el programa siga estando mal.
pcd_ejemplo_sin_optimizar(01_cout_entremezclado)
pcd_ejemplo_sin_optimizar(02_contador_compartido_roto)
pcd_ejemplo_sin_optimizar(03_trampa_1_sin_std_ref_parece_que_funciona)
pcd_ejemplo_sin_optimizar(04_transferencias_bancarias_rotas)
pcd_ejemplo_sin_optimizar(05_esperar_con_sleep_parece_funcionar)
pcd_ejemplo_sin_optimizar(06_esperar_con_sleep_falla)
pcd_ejemplo_sin_optimizar(07_espera_activa_satura_la_cpu)
pcd_ejemplo_sin_optimizar(08_referencia_colgante)
# Excepción deliberada: el mismo código que 02 compilado a -O2 para ver cómo
# la optimización esconde la carrera.
pcd_ejemplo_optimizado(09_la_optimizacion_esconde_la_carrera)

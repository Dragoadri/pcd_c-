# 06-mutex: candados tomados y soltados a mano con lock() y unlock().
# Los ejemplos de contador y de taquilla van a -O0: con -O2 la carrera de la
# versión sin candado puede desaparecer, y su pareja con candado se compila
# igual para comparar tiempos en las mismas condiciones.
pcd_ejemplo_sin_optimizar(01_contador_sin_mutex)
pcd_ejemplo_sin_optimizar(02_contador_con_mutex)
pcd_ejemplo(03_seccion_critica_demasiado_grande)
pcd_ejemplo(04_cout_con_mutex)
pcd_ejemplo(05_olvidar_unlock)
pcd_ejemplo(06_interbloqueo_dos_mutex)
pcd_ejemplo(07_interbloqueo_resuelto_mismo_orden)
pcd_ejemplo_sin_optimizar(08_taquilla_cine_sin_mutex)
pcd_ejemplo_sin_optimizar(09_taquilla_cine_con_mutex)

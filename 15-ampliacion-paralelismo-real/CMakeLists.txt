# =====================================================================
# 15-ampliacion-paralelismo-real (AMPLIACIÓN, no entra en examen)
# Sustituye al Tema 4 original (GIL, threading y multiprocessing en Python).
# Los dos primeros miden rendimiento: se compilan SIEMPRE a -O2.
# Los de fork solo funcionan en Linux y macOS.
# =====================================================================
pcd_ejemplo_optimizado(01_suma_paralela_speedup)
pcd_ejemplo_optimizado(02_falso_compartido)
pcd_ejemplo(03_async_y_future)
pcd_ejemplo(04_fork_primer_proceso)
pcd_ejemplo(05_fork_variables_se_copian)
pcd_ejemplo(06_fork_esperar_hijos_waitpid)

# 05-conceptos-y-exclusion-mutua
# Vocabulario (entra en examen) y tres intentos de exclusión mutua como lectura.
# Los ejemplos con int compartido sin proteger van a -O0: con -O2 la carrera
# puede desaparecer. Los que solo usan atomic dan igual con cualquier nivel.
pcd_ejemplo(01_interfoliacion_enumerar_mezclas)
pcd_ejemplo_sin_optimizar(02_atomicidad_incremento_en_tres_pasos)
pcd_ejemplo_sin_optimizar(03_seguridad_invariante_roto)
pcd_ejemplo(04_vivacidad_inanicion_simulada)

# Material de lectura, no examinable (ver LECTURA_cuatro_intentos.md)
pcd_ejemplo(05_intento2_banderas_viola_exclusion)
pcd_ejemplo(06_intento3_interbloqueo)
pcd_ejemplo_sin_optimizar(07_peterson_correcto)

# 08-barreras: latch (C++20 estándar) y barreras.
# Se imparte en clase invertida; no entra en el examen del parcial 1 (se permite usarlas).
pcd_ejemplo(01_salida_desigual_sin_barrera)
pcd_ejemplo(02_latch_pistoletazo_de_salida)
pcd_ejemplo(03_latch_esperar_a_n_listos)
pcd_ejemplo(04_barrera_artesanal)
# AVISO: <barrier> es una extensión de libstdc++, NO es C++20 estándar.
# Compila con GCC y con Clang en Linux; en macOS (libc++) y MSVC no existe.
pcd_ejemplo(05_std_barrier_de_libstdcxx)

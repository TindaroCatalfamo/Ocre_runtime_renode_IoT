# Logica di Funzionamento dello Script
Lo script esegue automaticamente una pipeline complessa in pochi secondi:

1. Input Dinamico: Lo script accetta come argomento qualsiasi file C. Non è necessario rinominare il file sorgente.

2. Cross-Compilation: Utilizza clang (fornito da WASI-SDK) per compilare il codice C in bytecode WebAssembly (.wasm), applicando i flag di ottimizzazione per sistemi embedded (-O3, -nostdlib).

3. Standardizzazione ("Camuffamento"): Questa è la fase chiave. Il file .wasm generato viene convertito in un header C (test_wasm.h). Durante questa conversione, lo script forza il nome della variabile interna dell'array a wasm_test_file.

Perché? Questo permette a Zephyr di "credere" di caricare sempre lo stesso file standard, mentre in realtà il contenuto logico (il bytecode) cambia dinamicamente in base al file C scelto.

4. Deployment: L'header test_wasm.h viene spostato automaticamente nella cartella dei sorgenti di Zephyr (.../platforms/zephyr/simple/src), sovrascrivendo la logica precedente.

5. Esecuzione: Infine, lo script lancia west build -b native_sim -t run, che ricompila il kernel Zephyr con il nuovo modulo WASM "incorporato" e avvia la simulazione.

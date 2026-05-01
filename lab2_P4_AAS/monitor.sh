#!/bin/bash

# File di output
OUTPUT_FILE="risultati_kathara.txt"

# Durata in secondi
DURATION=60

# Calcola il tempo di fine
END_TIME=$((SECONDS + DURATION))

echo "Avvio monitoraggio Kathara per $DURATION secondi..."
echo "I risultati verranno salvati in $OUTPUT_FILE"

# Pulisce o crea il file all'inizio
echo "--- Inizio Log: $(date) ---" > "$OUTPUT_FILE"

while [ $SECONDS -lt $END_TIME ]; do
    # Scrive un timestamp per ogni esecuzione
    echo "" >> "$OUTPUT_FILE"
    echo "Timestamp: $(date '+%H:%M:%S')" >> "$OUTPUT_FILE"
    echo "-----------------------------------" >> "$OUTPUT_FILE"
    
    # Esegue il comando e appende l'output al file
    kathara linfo >> "$OUTPUT_FILE"
    
    # Attende 1 secondo prima del prossimo controllo
    sleep 1
done

echo "Monitoraggio completato."

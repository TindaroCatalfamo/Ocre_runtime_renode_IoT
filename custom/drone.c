#include <stdio.h>
#include <stdlib.h>

// Definizione Stati
typedef enum {
    OFF,        // Spento (Stato Iniziale e Finale)
    IDLE,       // Acceso, fermo a terra (Motori disarmati o al minimo)
    TAKEOFF,    // Decollo
    HOVER,      // Volo stazionario
    LANDING     // Atterraggio
} DroneState;

// Dati Telemetria
typedef struct {
    int altitude;
    int battery;
    int vertical_speed;
} Telemetry;

void send_telemetry(const char* state_name, Telemetry t) {
    printf("[WASM DRONE] Stato: %-10s | Alt: %3dm | V.Speed: %2dm/s | Batt: %3d%%\n", 
           state_name, t.altitude, t.vertical_speed, t.battery);
}

int main() {
    printf("================================================\n");
    printf("   FSM DRONE AVANZATA: CICLO DI VITA COMPLETO   \n");
    printf("================================================\n");

    // 1. Stato Iniziale: OFF
    DroneState state = OFF;
    Telemetry data = {0, 100, 0}; 
    int target_altitude = 60;
    int simulation_step = 0;
    int system_running = 1; // Variabile per tenere vivo il loop
    int mission_complete = 0; // Flag per sapere se abbiamo già volato

    // Aumentiamo i passi per vedere tutto il ciclo
    while (system_running && simulation_step < 50) {
        
        // Consumo batteria (solo se non è OFF)
        if (state != OFF && data.battery > 0) {
            data.battery -= 4; // Consumo un po' meno per durare di più
        }
        // Clamp batteria (non va sotto 0)
        if (data.battery < 0) data.battery = 0;

        switch (state) {
            case OFF:
                if (mission_complete == 0) {
                    // FASE DI ACCENSIONE
                    printf("[SYSTEM] Booting sistema... Check sensori OK.\n");
                    state = IDLE; 
                } else {
                    // FASE DI SPEGNIMENTO DEFINITIVO
                    printf("[SYSTEM] Shutdown completato. Goodbye.\n");
                    system_running = 0; // Esce dal while
                }
                break;

            case IDLE:
                data.vertical_speed = 0;
                send_telemetry("IDLE", data);

                if (mission_complete == 0) {
                    // PRE-MISSIONE: Armiamo i motori e partiamo
                    printf("[LOGIC] Motori armati. Pronto al decollo.\n");
                    state = TAKEOFF;
                } else {
                    // POST-MISSIONE: Siamo atterrati, disarmiamo e spegniamo
                    printf("[LOGIC] Atterraggio confermato. Disarmo motori.\n");
                    printf("[LOGIC] Passaggio a stato OFF.\n");
                    state = OFF;
                }
                break;

            case TAKEOFF:
                data.vertical_speed = 15; // Sale veloce
                data.altitude += data.vertical_speed;
                send_telemetry("TAKEOFF", data);
                
                if (data.altitude >= target_altitude) {
                    data.altitude = target_altitude;
                    printf("[LOGIC] Quota raggiunta (%dm). Stabilizzazione in HOVER.\n", target_altitude);
                    state = HOVER;
                }
                break;

            case HOVER:
                data.vertical_speed = 0;
                // Piccola oscillazione casuale
                data.altitude += ((simulation_step % 3) - 1);
                send_telemetry("HOVER", data);

                // CONTROLLO BATTERIA
                if (data.battery < 30) {
                    printf("[ALERT] BATTERIA BASSA (%d%%)! Inizio discesa d'emergenza.\n", data.battery);
                    state = LANDING;
                }
                break;

            case LANDING:
                data.vertical_speed = -10; // Scende
                data.altitude += data.vertical_speed;
                
                if (data.altitude <= 0) {
                    data.altitude = 0; // Toccato terra
                    data.vertical_speed = 0;
                    mission_complete = 1; // Segniamo che la missione è finita
                    printf("[LOGIC] Contatto col suolo (Touchdown).\n");
                    state = IDLE; // TORNAMO IN IDLE PRIMA DI SPEGNERE
                } else {
                    send_telemetry("LANDING", data);
                }
                break;
        }

        simulation_step++;
        // Un piccolo ritardo fittizio per pulizia log (opzionale)
    }

    printf("================================================\n");
    return 0;
}

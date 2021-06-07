#include <iostream>
#include "portmidi.h"

using namespace std;

int main()
{
    int count, i;
    const PmDeviceInfo *info;

    Pm_Initialize();  // Inizializza PortMIDI

    if (count = Pm_CountDevices()) {  // Numero di device
        cout << "Dispositivi di OUTPUT: " << endl;
        for (i = 0; i < count; i++) {
            info = Pm_GetDeviceInfo(i); // Informazione sull'i-esimo device
            if (info->output)      // è 1 se il device è di output
                cout << i << ": " << info->name << endl; // stampa il numero e il nome del device
        }
        cout << endl << "Dispositivi di INPUT: " << endl;
        for (i = 0; i < count; i++) {
            info = Pm_GetDeviceInfo(i); // Informazione sull'i-esimo device
            if (info->input)     // è 1 se il device è di input
                cout << i << ": " << info->name << endl;
        }
    }
    else
        cout << "ERRORE: nessun device presente!" << endl;

    getchar();
    Pm_Terminate();     // Si termina PortMIDI

    return 0;
}

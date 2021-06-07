#include <iostream>
#include "portmidi.h"
#include "porttime.h"

#define MD_NOTEON  0x90
#define MD_NOTEOFF 0x80
#define MD_PRG     0xC0
#define SBYTE(mess,chan) mess | chan

using namespace std;

int main()
{
    int i, dev;
    PmError err;
    PortMidiStream *mstream;

    Pm_Initialize();    // Si inizializza PortMIDI
    dev = Pm_GetDefaultOutputDeviceID();  // Si utilizza il device di default
    Pt_Start(1, NULL, NULL);
    err = Pm_OpenOutput(&mstream, dev, NULL,512,NULL,NULL,0); // Si apre lo stream
    if (err != pmNoError)
        cout << "Errore: " << Pm_GetErrorText( err ) << endl;
    else {
        char chan = 0;
        int prg = 0;
        long time = 0;
        for(i=60; i < 72; prg+=4, i++){

            /* Si imposta lo strumento */
            Pm_WriteShort(mstream, 0, Pm_Message(SBYTE(MD_PRG, chan), prg, 0));
            time = Pt_Time();

            /* Si invia il messaggio di NOTE ON */
            Pm_WriteShort(mstream, 0, Pm_Message(SBYTE(MD_NOTEON, chan), i, 120));
            while(Pt_Time() - time < 1000);  // Si attende 1 secondo

            /* Si invia il messaggio di NOTE OFF */
            Pm_WriteShort(mstream, 0, Pm_Message(SBYTE(MD_NOTEOFF, chan), i, 120));
        }
    }

    Pm_Close(mstream);  // Si chiude lo stream
    Pm_Terminate();     // Si termina PortMIDI

    return 0;
}

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
    int count, i, dev[2], j, outcount=0, end=60000, chan=0;
    PmError err[2];
    PmEvent msg[128], msgout[1024];
    PortMidiStream *mstream[2];

    Pm_Initialize();  // Si inizializza PortMIDI
    dev[0] = Pm_GetDefaultInputDeviceID();
    dev[1] = Pm_GetDefaultOutputDeviceID();
    Pt_Start(1, NULL, NULL);
    err[0] = Pm_OpenInput(&mstream[0], dev[0], NULL, 512L, NULL, NULL);
    err[1] = Pm_OpenOutput(&mstream[1], dev[1], NULL, 512L, NULL, NULL, 0L);

    if(err[0] != pmNoError && err[1] != pmNoError)
        cout << "Errore in input (" << Pm_GetErrorText(err[0]) << ") o in output (" << Pm_GetErrorText(err[1]) << ")" <<  endl;
    else {
        while(Pt_Time() < end){  // Per 1 minuto

            /* Verifico gli input */
            if(Pm_Poll(mstream[0])) {

                /* Se ci sono eventi MIDI in attesa, li leggo */
                count = Pm_Read(mstream[0], msg, 32);
                for(i=0; i<count; i++) {

                    /* Si crea l'arpeggiatore nel buffer msgout */
                    if((Pm_MessageStatus(msg[i].message) & 0xF0) == MD_NOTEON) {
                        for(j=0;j<4; j++){

                            /* NOTE ON con il timestamp corretto */
                            msgout[outcount].message = Pm_Message(SBYTE(MD_NOTEON,chan), Pm_MessageData1(msg[i].message)+j*4, Pm_MessageData2(msg[i].message));
                            msgout[outcount].timestamp = msg[i].timestamp+100*j;
                            if (++outcount == 1024)
                                outcount = 0;

                            /* NOTE OFF */
                            msgout[outcount].message = Pm_Message(SBYTE(MD_NOTEOFF,chan), Pm_MessageData1(msg[i].message)+j*4, Pm_MessageData2(msg[i].message));
                             msgout[outcount].timestamp = msg[i].timestamp+100*j+100;
                             if (++outcount == 1024)
                                 outcount = 0;
                         }
                     }
                 }
             }

             /* Ora msgout può avere eventi MIDI, li scrivo */
             for(i=0; i < outcount; i++){
                 /* Se i timestamp sono validi li invio */
                 if(Pt_Time() >= msgout[i].timestamp && msgout[i].timestamp >= 0){
                     Pm_WriteShort(mstream[1], 0, msgout[i].message);
                     msgout[i].timestamp = -1;  // Li segno come suonati
                 }
             }
        }
    }

    Pm_Close(mstream[0]);  // Si chiude lo stream
    Pm_Close(mstream[1]);

    Pm_Terminate();  // Si termina PortMIDI

    return 0;
}

#include <fstream>
#include <iostream>
#include "allegro.h"
#include "porttime.h"
#include "portmidi.h"
#include "midicode.h"
#include "seq2midi.h"

using namespace std;


int main(void)
{
    PortMidiStream *mstream;
    PmDeviceID dev;
    PmError err;

    /* Si inizializza PortMIDI */
    Pm_Initialize();
    dev = Pm_GetDefaultOutputDeviceID();

    /* Si apre il file midi */
    ifstream file("my_file.mid", ios::in | ios::binary);
    if (!file) {
        cout << "Errore: impossibile aprire my_file.mid in lettura." << endl;
        return -1;
    }

    /* Si legge il file midi */
    Alg_seq_ptr seq = new Alg_seq(file, true);
    file.close();

    /* Si apre lo stream midi e si suona il file */
    err = Pm_OpenOutput(&mstream, dev, NULL, 256, (PmTimestamp(*)(void *))&Pt_Time, NULL, 100);
    if (err == pmNoError)
    {
        seq2midi(*seq, mstream);
        wait_until(time_elapsed() + 1);
        Pm_Close(mstream);
        cout << endl << "File suonato con successo." << endl;
    }

    Pm_Terminate();
    delete seq;

    return 0;
}

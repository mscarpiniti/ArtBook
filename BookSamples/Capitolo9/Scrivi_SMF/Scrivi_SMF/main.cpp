#include <iostream>
#include <fstream>
#include "allegro.h"

using namespace std;

int main(void)
{
    Alg_seq_ptr seq = new Alg_seq;

    /* Si creano le note */
    Alg_note_ptr note = seq->create_note(0, 0, 63, 63, 105, 0.8);
    seq->add_event(note, 0);

    note = seq->create_note(0.8, 0, 65, 65, 107, 0.4);
    seq->add_event(note, 0);

    note = seq->create_note(1.2, 0, 67, 67, 107, 0.6);
    seq->add_event(note, 0);

    note = seq->create_note(1.8, 0, 69, 69, 109, 0.2);
    seq->add_event(note, 0);

    /* Si usa il tempo anziche' i beats */
    seq->convert_to_seconds();
    seq->get_time_map()->insert_beat(0.8, 1.0);
    seq->get_time_map()->insert_beat(1.2, 2.0);
    seq->get_time_map()->insert_beat(1.8, 3.0);
    seq->get_time_map()->insert_beat(2.0, 4.0);

    /* Si scrive il file midi */
    ofstream file("my_file.mid", ios::out | ios::binary);
    if (!file) {
        cout << "Impossibile aprire my_file.mid in scrittura" << endl;
        return -1;
    }
    seq->smf_write(file);
    file.close();
    delete seq;

    cout << "Scrittura terminata." << endl;

    return 0;
}

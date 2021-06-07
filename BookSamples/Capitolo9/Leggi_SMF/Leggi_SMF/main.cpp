#include <iostream>
#include <fstream>
#include "allegro.h"

using namespace std;

int main(void)
{
    void *buffer;
    long bytes;

    /* Si apre il file midi */
    ifstream file("my_file.mid", ios::in | ios::binary);
    if (!file) {
        cout << "Errore: impossibile aprire my_file.mid in lettura." << endl;
        return -1;
    }

    /* Si legge il file midi */
    Alg_seq_ptr seq = new Alg_seq(file, true);
    file.close();

    /* Si stampano le informazioni sul contenuto del file */
    for (int i = 0; i < seq->tracks(); i++) {
        cout << "TRACK " << i << endl;
        for (int j = 0; j < seq->track(i)->length(); j++) {
            (*seq->track(i))[j]->show();
        }
    }

    /* Si serializza il file */
    seq->serialize(&buffer, &bytes);
    cout << endl << "Serializzati " << bytes << " byte." << endl;

    delete seq;
    cout << "Lettura terminata." << endl;

    cout << "Premi ENTER per terminare." << endl;
    getchar();

    return 0;
}

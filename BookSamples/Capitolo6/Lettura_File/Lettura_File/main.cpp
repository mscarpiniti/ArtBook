#include <iostream>
#include "audioIO.h"
#include "sndfile.h"

using namespace std;


int printError(PaError p_err)
{
    Pa_Terminate();
    cout << "Si è verificato un errore" << endl
        << "Codice di errore: " << p_err << endl
        << "Messaggio di errore: " << Pa_GetErrorText(p_err) << endl;
    return -1;
}


int main(void)
{
    PaStreamParameters  outputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    float               *buffer;
    int                 sampleRate, numCanali;

    SNDFILE *file;
    SF_INFO sfinfo;

    file = sf_open("Test_file.wav", SFM_READ, &sfinfo); /* Si legge il file */

    sampleRate = sfinfo.samplerate;  /* Frequenza di campionamento */
    numCanali  = sfinfo.channels;    /* Numero di canali */

    if (file == NULL)
    {
        cout << "Impossibile aprire 'Test_file.wav'." << endl;
        return -1;
    }

    cout << "Frequenza di campionamento = " << sampleRate << " Hz" << endl;
    cout << "Numero di canali = " << numCanali << endl;

    buffer = (float *) calloc(FRAMES_PER_BUFFER*numCanali, sizeof(float));

    err = Pa_Initialize();
    if (err != paNoError)
        return(printError(err));

    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = numCanali;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
             &stream,
             NULL,               /* <== Niente input */
             &outputParameters,
             sampleRate,
             FRAMES_PER_BUFFER,
             paClipOff,
             NULL,
             NULL );
    if (err != paNoError)
        return(printError(err));

    err = Pa_StartStream(stream);
    if (err != paNoError)
        return(printError(err));
    cout << "Sto suonando il file....." << endl;

    while (sf_readf_float(file, buffer, FRAMES_PER_BUFFER) > 0)
    {
        err = (int)Pa_WriteStream(stream, buffer, FRAMES_PER_BUFFER);
        if (err != paNoError)
            printError(err);
    }

    err = Pa_StopStream(stream);
    if (err != paNoError)
        return(printError(err));

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        return(printError(err));

    sf_close(file);
    cout << "Dati letti da 'Test_file.wav'." << endl;

    Pa_Terminate();

    return 0;
}

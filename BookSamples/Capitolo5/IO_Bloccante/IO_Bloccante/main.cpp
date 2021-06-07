#include <iostream>
#include <ctime>
#include "audioIO.h"

using namespace std;


int printError(PaError p_err)
{
    Pa_Terminate();
    cout << "Si è verificato un errore" << endl
         << "Codice di errore: " << p_err << endl
         << "Messaggio di errore: " << Pa_GetErrorText( p_err ) << endl;
    return -1;
}


int main(void) {
    PaError err;
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream;
    int i;
    float inbuffer[FRAMES_PER_BUFFER], outbuffer[FRAMES_PER_BUFFER];
    const PaDeviceInfo *info;
    const PaHostApiInfo *api;
    time_t start = 0.0, end = 0.0;


    err = Pa_Initialize();
    if (err != paNoError)
        return( printError(err) );

    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 1;             /* mono */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;  /* 32 bit floating point */
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowOutputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = 1;             /* mono */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;  /* 32 bit floating point */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    // Stampa informazione del device di output utilizzato
    info = Pa_GetDeviceInfo(outputParameters.device);
    api  = Pa_GetHostApiInfo(info->hostApi);
    if (info->maxOutputChannels > 0)
        cout << "Dispositivo di output: ";
    cout <<  "[" << api->name << "] " << info->name << endl;

    err = Pa_OpenStream( &stream,
                         &inputParameters,
                         &outputParameters,
                         SAMPLE_RATE,
                         FRAMES_PER_BUFFER,
                         paNoFlag,
                         NULL,    /* <== Non è presente la callback */
                         NULL );
    if (err != paNoError)
            return( printError(err) );

    err = Pa_StartStream(stream);
    if (err != paNoError)
        return( printError(err) );

    cout << "Lo stream sarà attivo per 5 secondi" << endl;
    time(&start);  // Start del timer

    while (difftime(end, start) <= NUM_SECONDI) {
        err = Pa_ReadStream(stream, inbuffer, FRAMES_PER_BUFFER);
        if (err != paNoError)
            return( printError(err) );

        for (i = 0; i < FRAMES_PER_BUFFER; i++)
            outbuffer[i] = inbuffer[i];

        err = (int)Pa_WriteStream(stream, outbuffer, FRAMES_PER_BUFFER);
        if (err != paNoError)
            return( printError(err) );

        time(&end);
    }

    err = Pa_StopStream(stream);
    if (err != paNoError)
        return( printError(err) );

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        return( printError(err) );

    Pa_Terminate();
    cout << "Test Completato." << endl;

    return 0;
}

// Sinusoide_ASIO.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include <iostream>
#include "audioIO.h"
#include "pa_asio.h"

#define TABLE_SIZE   (512)

using namespace std;

typedef struct
{
    float sine[TABLE_SIZE];
    float phase;
} sineData;


/* Funzione di callback */
static int sine_Callback(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    sineData *data = (sineData*)userData;
    float *out = (float*)outputBuffer, incr;
    unsigned long i;

    /* Evitiamo warning per variabili non utilizzate. */
    (void)timeInfo;
    (void)statusFlags;
    (void)inputBuffer;

    incr = 440 * (float)TABLE_SIZE / (float)SAMPLE_RATE;  // 440 Hz

    for (i = 0; i<framesPerBuffer; i++)
    {
        *out++ = data->sine[(int)data->phase];

        data->phase += incr;
        if (data->phase >= TABLE_SIZE)
            data->phase -= TABLE_SIZE;
    }

    return paContinue;
}


/* Funzione printError */
int printError(PaError p_err)
{
    Pa_Terminate();
    cout << "Si è verificato un errore" << endl
        << "Codice di errore: " << p_err << endl
        << "Messaggio di errore: " << Pa_GetErrorText(p_err) << endl;
    return -1;
}


/* Funzione principale */
int main(int argc, char *argv[])
{
    PaStreamParameters outputParameters;
    PaAsioStreamInfo asioOutputInfo;
    PaStream *stream;
    PaError err;
    sineData data;
    int i, outputChannelSelectors[1];
    (void)argc;
    (void)argv;

    cout << "Sinusoide con SR = " << SAMPLE_RATE << "e Buffer_Size = " << FRAMES_PER_BUFFER << endl;

    /* Inizializzazione della tabella dei campioni */
    for (i = 0; i<TABLE_SIZE; i++)
    {
        data.sine[i] = (float)sin( 2 * M_PI * i / TABLE_SIZE );
    }
    data.phase = 0.0;

    /* Inizializzazione di PortAudio*/
    err = Pa_Initialize();
    if (err != paNoError)
        return(printError(err));

    /* Scelta dei parametri della scheda */
    outputParameters.device = 8; // Pa_GetDefaultOutputDevice(); /* Device ASIO */
    outputParameters.channelCount = 1;                  /* Mono */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;     /* 32 bit floating point */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)
        ->defaultLowOutputLatency;

    /* Uso di una specifica struttura per ASIO. ATTENZIONE - solo per Windows. */
    asioOutputInfo.size = sizeof(PaAsioStreamInfo);
    asioOutputInfo.hostApiType = paASIO;
    asioOutputInfo.version = 1;
    asioOutputInfo.flags = paAsioUseChannelSelectors;
    outputChannelSelectors[0] = 1;  /* Salta il canale 0 e usa il canale ASIO 1 (destro) */
    asioOutputInfo.channelSelectors = outputChannelSelectors;
    outputParameters.hostApiSpecificStreamInfo = &asioOutputInfo;

    /* Apertura dello stream e notifica della funzione di callback */
    err = Pa_OpenStream(&stream,
        NULL,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        sine_Callback,
        &data);
    if (err != paNoError)
        return( printError(err) );

    cout << "Avvio lo stream" << endl;
    err = Pa_StartStream( stream );
    if (err != paNoError)
        return( printError(err) );

    cout << "Lo stream sarà attivo per 5 secondi" << endl;

    /* Attendi 5 secondi prima di procedere*/
    Pa_Sleep( NUM_SECONDI * 1000 );

    err = Pa_StopStream( stream );
    if (err != paNoError)
        return( printError(err) );

    err = Pa_CloseStream( stream );
    if (err != paNoError)
        return( printError(err) );

    Pa_Terminate();
    cout << "Test Completato." << endl;
    getchar();

    return 0;
}

#include <iostream>
#include "audioIO.h"

#define TABLE_SIZE   (200)

using namespace std;

typedef struct
{
    float sine[TABLE_SIZE];
    int left_phase;
    int right_phase;
    char message[20];
} sineData;


static int sine_Callback( const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData )
{
    sineData *data = (sineData*)userData;
    float *out = (float*)outputBuffer;
    unsigned long i;

    /* Evitiamo warning per variabili non utilizzate. */
    (void) timeInfo;
    (void) statusFlags;
    (void) inputBuffer;

    for( i=0; i<framesPerBuffer; i++ )
    {
        /* Il buffer contiene campioni dei due canali in modo interlacciato, */
        *out++ = data->sine[data->left_phase];  /* sinistro */
        *out++ = data->sine[data->right_phase];  /* destro */
        data->left_phase += 1;
        if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
        data->right_phase += 3; /* Vedere spiegazione alla fine del codice.*/
        if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
    }

    return paContinue;
}


int printError(PaError p_err)
{
    Pa_Terminate();
    cout << "Si è verificato un errore" << endl
         << "Codice di errore: " << p_err << endl
         << "Messaggio di errore: " << Pa_GetErrorText( p_err ) << endl;
    return -1;
}


int main(int argc, char *argv[])
{
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    sineData data;
    int i;

    (void)argc;
    (void)argv;

    cout << "PortAudio Test: output sine wave. SR = " << SAMPLE_RATE << "BufSize = " << FRAMES_PER_BUFFER << endl;

    /* inizializzazione della tabella dei campioni */
    for( i=0; i<TABLE_SIZE; i++ )
    {
        data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
    }
    data.left_phase = data.right_phase = 0;

    /* inizializzazione di PortAudio*/
    err = Pa_Initialize();
    if (err != paNoError)
        return( printError(err) );

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* device di default */
    outputParameters.channelCount = NUM_CANALI;           /* stereo */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;   /* 32 bit floating point */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )
            ->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    /* Apertura dello stream e notifica della funzione di callback */
    err = Pa_OpenStream( &stream,
                         NULL,
                         &outputParameters,
                         SAMPLE_RATE,
                         FRAMES_PER_BUFFER,
                         paClipOff,
                         sine_Callback,
                         &data );
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

    return 0;
}

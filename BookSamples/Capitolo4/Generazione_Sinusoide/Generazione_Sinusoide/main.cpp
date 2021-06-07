#include <iostream>
#include "audioIO.h"

using namespace std;

typedef struct
{
    float sample_freq;
    float left_freq;
    float right_freq;
    int left_phase;
    int right_phase;
    float left_gain;
    float right_gain;
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
        /* Le seguenti funzioni sono illustrate di seguito */
        *out++ = data->left_gain*sin(2*M_PI*(++data->left_phase)*(data->left_freq/data->sample_freq));
        *out++ = data->right_gain*sin(2*M_PI*(++data->right_phase)*(data->right_freq/data->sample_freq));
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

    (void)argc;
    (void)argv;

    cout << "PortAudio Test: output sine wave. SR = "
         << SAMPLE_RATE << "BufSize = " << FRAMES_PER_BUFFER << endl;

    data.left_freq = 440;  // 440Hz
    data.left_phase = 0;
    data.right_freq = 220; // 220Hz
    data.right_phase = 0;
    data.sample_freq = 44100;
    data.left_gain = 1;
    data.right_gain = 1;

    /* Inizializzazione di PortAudio*/
    err = Pa_Initialize();
    if( err != paNoError )
        return( printError(err) );

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* device di default */
    outputParameters.channelCount = NUM_CANALI;           /* stereo */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;   /* 32 bit floating point */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    cout << "Apertura dello stream e notifica della funzione di callback" << endl;
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

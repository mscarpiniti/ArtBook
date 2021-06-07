#include <iostream>
#include <random>
#include "audioIO.h"

using namespace std;


static int whitenoise_Callback( const void* inputBuffer, void* outputBuffer,
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void* userData )
{
   float *out = (float*)outputBuffer;
   (void) inputBuffer;
   (void) timeInfo;
   (void) statusFlags;
   (void) userData;
   unsigned long i;

   for( i=0; i<framesPerBuffer; i++ )
   {
       *out++ = ((float) rand()/RAND_MAX) * 2.0f - 1.0f;
       *out++ = ((float) rand()/RAND_MAX) * 2.0f - 1.0f;
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

    (void)argv;
    (void)argc;

    /* Inizializzazione di PortAudio*/
    err = Pa_Initialize();
    if( err != paNoError )
        return( printError(err) );

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* device di default in input */
    outputParameters.channelCount = NUM_CANALI;       /* stereo */
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
                         whitenoise_Callback,
                         NULL );
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

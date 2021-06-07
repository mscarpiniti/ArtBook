#include <iostream>
#include "audioIO.h"

using namespace std;

typedef struct
{
    float A_left;
    float A_right;
} amplificazione;


static int stream_Callback( const void* inputBuffer, void* outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData )
{
   float  *in = (float*)inputBuffer;
   float *out = (float*)outputBuffer;
   amplificazione  *amp = (amplificazione*)userData;  // Valore dell'amplificazione
   (void) timeInfo;
   (void) statusFlags;
   unsigned long i;

   for( i=0; i<framesPerBuffer; i++ )
   {
       *out++ = amp->A_left*(*in++);
       *out++ = amp->A_right*(*in++);
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
    PaStreamParameters inputParameters;
    PaStream *stream;
    PaError err;
    amplificazione amp;

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

    inputParameters.device = Pa_GetDefaultInputDevice(); /* device di default in output */
    inputParameters.channelCount = NUM_CANALI;       /* stereo */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;   /* 32 bit floating point */
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    cout << "Inserisci l'amplificazione del canale sinistro e destro: ";
    cin  >> amp.A_left >> amp.A_right;
    cout << endl;

    cout << "Apertura dello stream e notifica della funzione di callback" << endl;
    err = Pa_OpenStream( &stream,
                         &inputParameters,
                         &outputParameters,
                         SAMPLE_RATE,
                         FRAMES_PER_BUFFER,
                         paClipOff,
                         stream_Callback,
                         &amp );
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

#include <iostream>
#include "audioio.h"

using namespace std;

typedef struct
{
    float left_freq;
    float left_gain;
    int left_phase;

    float right_freq;
    int right_phase;
    float right_gain;
} sineData;

typedef struct
{
    float left_freq;
    float left_period_size;
    float current_left_phase;
    float left_gain;

    float right_freq;
    float right_period_size;
    float current_right_phase;
    float right_gain;
} sawData;

typedef struct
{
    sawData sawParams;
    sineData sineParams;
}
myDataStruct;


static int sineSaw_Callback( const void* inputBuffer, void* outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void* userData)
{
   float *out = (float*)outputBuffer;
   (void) inputBuffer;
   (void) timeInfo;
   (void) statusFlags;
   myDataStruct* data = (myDataStruct*)userData;
   unsigned long i;

   sineData* sine = &data->sineParams;
   sawData*   saw = &data->sawParams;

   for( i=0; i<framesPerBuffer; i++ )
   {
       float l_yIncr = 2/saw->left_period_size;

       float sawL = (l_yIncr + ((int)(++saw->current_left_phase) % ((int)saw->left_period_size))) - 1;
       float sawR = (l_yIncr + ((int)(++saw->current_right_phase) % ((int)saw->right_period_size))) - 1;

       float sineL = sin(2*M_PI*(++sine->left_phase)*(sine->left_freq/SAMPLE_RATE));
       float sineR = sin(2*M_PI*(++sine->right_phase)*(sine->right_freq/SAMPLE_RATE));

       *out++ = saw->left_gain * sawL + sine->left_gain * sineL;
       *out++ = saw->right_gain * sawR + sine->right_gain * sineR;
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
    myDataStruct l_sData;

    (void)argv;
    (void)argc;

    l_sData.sawParams.left_freq = 440;
    l_sData.sawParams.left_period_size = SAMPLE_RATE/l_sData.sawParams.left_freq;
    l_sData.sawParams.current_left_phase = l_sData.sawParams.left_period_size/2;
    l_sData.sawParams.left_gain = 0.3;

    l_sData.sawParams.right_freq = 110;
    l_sData.sawParams.right_period_size = SAMPLE_RATE/l_sData.sawParams.right_freq;
    l_sData.sawParams.current_right_phase = l_sData.sawParams.right_period_size/2;
    l_sData.sawParams.right_gain = 1;

    l_sData.sineParams.left_freq = 440;
    l_sData.sineParams.left_phase = 0;
    l_sData.sineParams.right_freq = 220;
    l_sData.sineParams.right_phase = 0;
    l_sData.sineParams.left_gain = 1;
    l_sData.sineParams.right_gain = 1;

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
                         sineSaw_Callback,
                         &l_sData );
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

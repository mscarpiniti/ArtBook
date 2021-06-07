#include <iostream>
#include "audioIO.h"

#define LIN   1  // 1 = lineare;  0 = a potenza costante

using namespace std;

typedef struct
{
    float A_left;       // Amplificazione canale sinistro
    float A_right;      // Amplificazione canale destro
    float posizione;    // Posizione dell'ascoltatore: -1 = L, +1 = R
    unsigned long idx;  // Indice della sinusoide
} pan;


// Stereo panning di tipo lineare
void linear_pan(float position, pan *pos)
{
    position *= 0.5;
    pos->A_left  = position - 0.5;
    pos->A_right = position + 0.5;
}


// Stereo panning a potenza costante
void power_pan(float position, pan *pos)
{
    float angle = 0.25 * M_PI * position;

    pos->A_left  = 0.5 * sqrt(2.0) * (cos(angle) - sin(angle));
    pos->A_right = 0.5 * sqrt(2.0) * (cos(angle) + sin(angle));
}



// Callback per lo streo panning
static int stereoPan_Callback( const void* inputBuffer, void* outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData )
{
   float *out = (float*)outputBuffer;
   pan  *amp = (pan*)userData;  // Valore del panning
   (void) inputBuffer;
   (void) timeInfo;
   (void) statusFlags;
   unsigned long i;
   float f0 = 220, incr = 2.0 * (float)FRAMES_PER_BUFFER / (float)(NUM_SECONDI * SAMPLE_RATE);

   // Aggiornamento ogni FRAME_PER_BUFFER
   if (LIN)
       linear_pan(amp->posizione, amp);
   else
       power_pan(amp->posizione, amp);

   for( i=0; i<framesPerBuffer; i++ )
   {
       *out++ = amp->A_left  * sin(2*M_PI * amp->idx * f0/SAMPLE_RATE);
       *out++ = amp->A_right * sin(2*M_PI * amp->idx * f0/SAMPLE_RATE);

       amp->idx += 1;
   }
   amp->posizione += incr;

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
    pan amp;

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

    amp.A_left  = 0.0;
    amp.A_right = 0.0;
    amp.posizione = -1.0;
    amp.idx = 0;

    cout << "Apertura dello stream e notifica della funzione di callback" << endl;
    err = Pa_OpenStream( &stream,
                         &inputParameters,
                         &outputParameters,
                         SAMPLE_RATE,
                         FRAMES_PER_BUFFER,
                         paClipOff,
                         stereoPan_Callback,
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

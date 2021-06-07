#include <iostream>
#include "audioIO.h"
#include "sndfile.h"

using namespace std;

typedef struct
{
    int     frameIndex;        // Indice dell'array
    int     maxFrameIndex;     // Dimensioni dell'array
    float   *recordedSamples;  // Dati registrati
} waveData;


int printError(PaError p_err)
{
    Pa_Terminate();
    cout << "Si è verificato un errore" << endl
         << "Codice di errore: " << p_err << endl
         << "Messaggio di errore: " << Pa_GetErrorText( p_err ) << endl;
    return -1;
}


static int record_Callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    waveData *data = (waveData*)userData;
    const float *in = (const float*)inputBuffer;
    float *out = &data->recordedSamples[data->frameIndex * NUM_CANALI];
    long i, j, framesToCalc;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

    /* Evita un warning per variabile non usata */
    (void) outputBuffer;
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if( framesLeft < framesPerBuffer ) // Se è quasi finita la memoria
    {
        framesToCalc = framesLeft;
        finished = paComplete;
    }
    else
    {
        framesToCalc = framesPerBuffer;
        finished = paContinue;
    }

    if( inputBuffer == NULL )  // Se non ci sono dati in input
    {
    for( i=0; i<framesToCalc; i++ )
      for( j=0; j<NUM_CANALI; j++)
        *out++ = SAMPLE_SILENCE;
    }
    else
    {
    for( i=0; i<framesToCalc; i++ )
      for( j=0; j<NUM_CANALI; j++ )
        *out++ = *in++;
    }
    data->frameIndex += framesToCalc;
    return finished;
}


int main(void)
{
    PaStreamParameters  inputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    waveData            data;
    int                 i, totalFrames, numSamples, numBytes;

    data.maxFrameIndex = totalFrames = NUM_SECONDI * SAMPLE_RATE; // Registra per pochi secondi
    data.frameIndex = 0;
    numSamples = totalFrames * NUM_CANALI;
    numBytes = numSamples * sizeof(float);
    data.recordedSamples = (float *)malloc(numBytes);  // Alloco la memoria necessaria
    if( data.recordedSamples == NULL )
    {
        cout << "Impossibile allocare la memoria necessaria" << endl;
        return -1;
    }
    for( i=0; i<numSamples; i++ )      // Inizializzo la memoria
        data.recordedSamples[i] = 0;

    err = Pa_Initialize();
    if( err != paNoError )
        return( printError(err) );

    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = NUM_CANALI;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                 // Niente output
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,
              record_Callback,
              &data );
    if( err != paNoError )
        return( printError(err) );

    err = Pa_StartStream( stream );
    if( err != paNoError )
        return( printError(err) );
    cout << "Sto registrando....." << endl;

    while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    {
        Pa_Sleep(1000);
        cout << "indice = " << data.frameIndex << endl; // Stampo l'indice dell'array
    }
    if( err != paNoError )
        return( printError(err) );

    err = Pa_StopStream( stream );
    if( err != paNoError )
        return( printError(err) );

    err = Pa_CloseStream( stream );
    if( err != paNoError )
        return( printError(err) );


    SNDFILE *file;
    SF_INFO sfinfo;

    sfinfo.samplerate = SAMPLE_RATE ;
    sfinfo.frames	  = totalFrames ;
    sfinfo.channels	  = NUM_CANALI ;
    if (WAVFILE)
        sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_FLOAT) ;  // Formato WAV
    else if (AIFFILE)
        sfinfo.format = (SF_FORMAT_AIFF | SF_FORMAT_FLOAT) ; // Formato AIFF
    else
        cout << "Errore: tipo di file non definito" << endl;

    file = sf_open("Test_file.wav", SFM_WRITE, &sfinfo);

    if( file == NULL )
       cout << "Impossibile aprire 'Test_file.wav'." << endl;
    else
    {
        sf_write_float(file, data.recordedSamples, totalFrames * NUM_CANALI);
        sf_close(file);
        cout << "Dati scritti su 'Test_file.wav'." << endl;
    }

    free(data.recordedSamples);
    Pa_Terminate();

    return 0;
}

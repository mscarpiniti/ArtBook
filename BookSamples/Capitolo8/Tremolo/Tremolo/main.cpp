#include <iostream>
#include "audioIO.h"

#define F0  15     // in Hz
#define A   0.85

using namespace std;


typedef struct {
    float f0;            /* Frequenza del tremolo */
    float alpha;         /* Ampiezza del tremolo */
    unsigned long idx;   /* Indice per la sinusoide dell'LFO */
} myData;


/* Tremolo CallBack function -------------------------------------------- */
static int tremolo_Callback( const void* inputBuffer, void* outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void* userData )
{
    float *in = (float *)inputBuffer, *out = (float *)outputBuffer;
    myData *data = (myData *)userData;
    (void) timeInfo;
    (void) statusFlags;
    float f0 = data->f0 / (float)SAMPLE_RATE, alpha = data->alpha;
    unsigned long i, idx = data->idx;

    for (i = 0; i < framesPerBuffer; i++)
    {
        *out++ = (1 + alpha*cos(2*M_PI*f0*idx)) * (*in++)/2;
        *out++ = (1 + alpha*cos(2*M_PI*f0*idx)) * (*in++)/2;
        ++idx;
    }

    data->idx = idx;

    return paContinue;
}


/* PRITERROR ------------------------------------------------------ */
int printError(PaError p_err)
{
    Pa_Terminate();
    cout << "Si è verificato un errore" << endl
         << "Codice di errore: " << p_err << endl
         << "Messaggio di errore: " << Pa_GetErrorText( p_err ) << endl;
    return -1;
}


/* MAIN ----------------------------------------------------------- */
int main()
{
    PaError err;
    PaDeviceIndex devin, devout;
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream;
    myData *data = (myData *)calloc(sizeof(myData), 1);

    err = Pa_Initialize();
    if (err != paNoError)
        return( printError(err) );

    devin  = Pa_GetDefaultInputDevice();
    devout = Pa_GetDefaultOutputDevice();

    inputParameters.device = devin;
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowOutputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = (PaDeviceIndex)devout;
    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    data->f0 = (float)F0;
    data->alpha = (float)A;
    data->idx = 0;

    err = Pa_OpenStream(&stream,
                        &inputParameters,
                        &outputParameters,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paClipOff,
                        tremolo_Callback,
                        data);
    if (err != paNoError)
        return( printError(err));

    err = Pa_StartStream(stream);
    if (err != paNoError)
        return( printError(err));

    cout << "Premi ENTER per terminare il programma." << endl;
    getchar();

    err = Pa_StopStream(stream);
    if (err != paNoError)
        return( printError(err));

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        return( printError(err));

    Pa_Terminate();
    free(data);

    return 0;
}

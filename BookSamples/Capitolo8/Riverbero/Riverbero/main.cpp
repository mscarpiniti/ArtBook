#include <iostream>
#include "audioIO.h"

#define FRAMES_PER_BUFFER 1024
#define LB 2048
#define G  0.7

using namespace std;

typedef struct {
    float buffer[(int)LB];  /* Buffer circolare */
    float g;                /* Guadagno dell'effetto */
    unsigned long pt;       /* Puntatore in lettura */
} myBuffer;


/* riverbero CallBack function -------------------------------------------- */
static int riverbero_Callback( const void* inputBuffer, void* outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void* userData )
{
    float *in = (float *)inputBuffer, *out = (float *)outputBuffer;
    myBuffer *data = (myBuffer *)userData;
    float temp, *buffer = data->buffer, g = data->g;
    unsigned long i, pt = data->pt;
    (void) timeInfo;
    (void) statusFlags;

    for (i = 0; i < framesPerBuffer; i++)
    {
        temp = buffer[pt];
        buffer[pt++] = *in - g*temp;
        *out++ = temp + g * (*in++);
        if (pt >= LB)
            pt = 0;
    }

    data->pt = pt;

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
int main(void)
{
    PaError err;
    PaDeviceIndex devin, devout;
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream;
    myBuffer *data = (myBuffer *)calloc(sizeof(myBuffer), 1);

    err = Pa_Initialize();
    if (err != paNoError)
        return( printError(err) );

    devin = Pa_GetDefaultInputDevice();
    devout = Pa_GetDefaultOutputDevice();

    inputParameters.device = devin;
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowOutputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = (PaDeviceIndex)devout;
    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    data->pt = 0;
    data->g  = (float)G;

    err = Pa_OpenStream(&stream,
                        &inputParameters,
                        &outputParameters,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paClipOff,
                        riverbero_Callback,
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

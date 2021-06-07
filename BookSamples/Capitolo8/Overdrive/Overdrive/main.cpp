#include <iostream>
#include <cmath>
#include "audioIO.h"

#define TH   0.333333
#define MIX  0.85

using namespace std;


typedef struct {
    float soglia;   /* Soglia overdrive */
    float mix;      /* Mix con il segnale originale */
} myData;


/* Funzione SEGNO ------------------------------------------------- */
float sign(float x)
{
    if (x > 0)
        return 1.0;
    else if (x < 0)
        return -1.0;
    else
        return 0.0;
}


/* Overdrive CallBack function -------------------------------------------- */
static int overdrive_Callback( const void* inputBuffer, void* outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void* userData )
{
    float *in = (float *)inputBuffer, *out = (float *)outputBuffer;
    myData *data = (myData *)userData;
    (void) timeInfo;
    (void) statusFlags;
    float soglia = data->soglia, alpha = data->mix;
    float out_dist;
    unsigned long i;

    for (i = 0; i < framesPerBuffer; i++)
    {
        /* Canale SINISTRO */
        if (abs(*in) <= soglia)
        {
            out_dist = 2 * (*in);
        }
        else {
            if ((abs(*in) > soglia) & (abs(*in) < 2 * soglia))
            {
                out_dist = sign(*in)*(3 - pow(2 - 3 * abs(*in), 2)) / 3;
            }
            else
            {
                out_dist = (float)(sign(*in));
            }
        }
        *out++ = (1 - alpha) * (*in++) + alpha*out_dist;

        /* Canale DESTRO */
        if (abs(*in) <= soglia)
        {
            out_dist = 2 * (*in);
        }
        else {
            if ((abs(*in) > soglia) & (abs(*in) < 2 * soglia))
            {
                out_dist = sign(*in)*(3 - pow(2 - 3 * abs(*in), 2)) / 3;
            }
            else
            {
                out_dist = (float)(sign(*in));
            }
        }
        *out++ = (1 - alpha)*(*in++) + alpha*out_dist;
    }

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

    data->soglia = (float)TH;
    data->mix = (float)MIX;

    err = Pa_OpenStream(&stream,
                        &inputParameters,
                        &outputParameters,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paNoFlag,
                        overdrive_Callback,
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

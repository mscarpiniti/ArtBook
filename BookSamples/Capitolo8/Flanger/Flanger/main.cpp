#include <iostream>
#include "audioIO.h"

#define LB 8192
#define MODFREQ 3  // in Hz
#define WIDTH   9  // in ms
#define G 0.4

using namespace std;

typedef struct {
    float buffer[(int)LB];  /* Buffer circolare */
    unsigned long pt_l;     /* Puntatore in lettura */
    unsigned long pt_s;     /* Puntatore in scrittura */
    float D1;               /* Profondità dell'effetto */
    float g;                /* Coefficiente wet/dry */
    unsigned long idx;      /* Indice per la sinusoide dell'LFO */
} myBuffer;



/* Flanger CallBack function -------------------------------------- */
static int flanger_Callback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
    float *in = (float *)inputBuffer, *out = (float *)outputBuffer;
    myBuffer *data = (myBuffer *)userData;
    float f_D, rp, alpha, temp;
    float *buffer = data->buffer, D1 = data->D1, g = data->g;
    unsigned long i, idx = data->idx;
    int rpi, pt_l = data->pt_l, pt_s = data->pt_s;
    float f0 = (float)MODFREQ / (float)SAMPLE_RATE;
    (void)timeInfo;
    (void)statusFlags;


    for (i = 0; i < framesPerBuffer; i++)
    {
        buffer[pt_s++] = *in;

        f_D = (float)sin(2 * M_PI * f0 * idx++);
        rp = (pt_l++) + D1 * f_D;

        if (rp >= 0) {
            if (rp >= LB)
                rp = rp - (int)LB;
        }
        else {
            rp = rp + (int)LB;
        }
        rpi = (int)rp;
        alpha = rp - rpi;

        if (rpi != LB - 1)
            temp = buffer[rpi] + alpha*(buffer[rpi + 1] - buffer[rpi]);
        else
            temp = buffer[rpi] + alpha*(buffer[0] - buffer[rpi]);

        *out++ = (1 - g) * (*in++) + g*temp;

        if (pt_s >= LB)
            pt_s = 0;

        if (pt_l >= LB)
            pt_l = 0;
    }

    data->pt_l = pt_l;
    data->pt_s = pt_s;
    data->idx = idx;

    return paContinue;
}




/* PRITERROR ------------------------------------------------------ */
int printError(PaError p_err)
{
    Pa_Terminate();
    cout << "Si è verificato un errore" << endl
        << "Codice di errore: " << p_err << endl
        << "Messaggio di errore: " << Pa_GetErrorText(p_err) << endl;
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
        return(printError(err));

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

    float D0 = round((float)WIDTH*SAMPLE_RATE / 1000);
    data->D1 = round((float)WIDTH*SAMPLE_RATE / 1000);
    data->pt_l = 0;
    data->pt_s = 1 + (int)D0;
    data->g = (float)G;
    data->idx = 0;
    cout << "Profondita' effetto in campioni: " << data->D1 << endl;

    err = Pa_OpenStream(&stream,
        &inputParameters,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        flanger_Callback,
        data);
    if (err != paNoError)
        return(printError(err));

    err = Pa_StartStream(stream);
    if (err != paNoError)
        return(printError(err));

    cout << "Premi ENTER per terminare il programma." << endl;
    getchar();

    err = Pa_StopStream(stream);
    if (err != paNoError)
        return(printError(err));

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        return(printError(err));

    Pa_Terminate();
    free(data);

    return 0;
}

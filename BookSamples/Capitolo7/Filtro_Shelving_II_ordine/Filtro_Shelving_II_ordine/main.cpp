#include <iostream>
#include <string.h>
#include "audioIO.h"

#define F0   (3000)
#define FB   (1000)

using namespace std;

typedef struct
{
    float f0;
    float fb;
    float G;
    float wL[2];
    float wR[2];
} filtro;



/* Parametri del filtro Shelving del II ordine ----------------------- */
float shelving_II_ordine(float x, float f0, float fb, float G, float *xw)
{
    float k, d, V0, H0, a, xh;
    float y1, y;

    V0 = pow(10, G/20.0);   // G è in dB
    H0 = V0 - 1;
    k = tan(M_PI * fb / (float)SAMPLE_RATE);
    d = -cos(2 * M_PI * f0 / (float)SAMPLE_RATE);

    if (G >= 0)
        a = (k - 1) / (k + 1);     // Peak
    else
        a = (k - V0) / (k + V0);   // Notch

    xh = x - d * (1 - a) * xw[0] + a * xw[1];
    y1 = -a * xh + d * (1 - a) * xw[0] + xw[1];
    xw[1] = xw[0];
    xw[0] = xh;

    y = 0.5 * H0 * (x - y1) + x;   // Peak/Notch

    return y;
}


/* Stream Callback ------------------------------------------------------ */
static int stream_Callback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
    float  *in = (float*)inputBuffer;
    float *out = (float*)outputBuffer;
    filtro *data = (filtro*)userData;
    float f0 = data->f0, fb = data->fb, G = data->G;
    float *wL = data->wL, *wR = data->wR;
    unsigned long i;
    (void)timeInfo;
    (void)statusFlags;

    for (i = 0; i<framesPerBuffer; i++)
    {
        *out++ = shelving_II_ordine(*in++, f0, fb, G, wL);
        *out++ = shelving_II_ordine(*in++, f0, fb, G, wR);
    }

    return paContinue;
}


/* Print Error ------------------------------------------------------ */
int printError(PaError p_err)
{
    Pa_Terminate();
    cout << "Si è verificato un errore" << endl
        << "Codice di errore: " << p_err << endl
        << "Messaggio di errore: " << Pa_GetErrorText(p_err) << endl;
    return -1;
}


/* Funzione MAIN ------------------------------------------------------ */
int main(int argc, char *argv[])
{
    PaStreamParameters outputParameters;
    PaStreamParameters inputParameters;
    PaStream *stream;
    PaError err;
    filtro *h;
    (void)argv;
    (void)argc;

    /* Inizializzazione di PortAudio*/
    err = Pa_Initialize();
    if (err != paNoError)
        return(printError(err));

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

    h = (filtro*)malloc(sizeof(filtro));
    h->f0 = F0;
    h->fb = FB;
    h->G  = 20;
    h->wL[0] = 0.0;
    h->wL[1] = 0.0;
    h->wR[0] = 0.0;
    h->wR[1] = 0.0;

    err = Pa_OpenStream(&stream,
        &inputParameters,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        stream_Callback,
        h );
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
    free(h);

    return 0;
}

#include <iostream>
#include <string.h>
#include "audioIO.h"

#define FT   (4000)
#define TIPO ("passa_basso")  //("passa_alto")

using namespace std;

typedef struct
{
    float ft;
    float G;
    float wL;
    float wR;
} filtro;



/* Parametri del filtro Shelving del I ordine ----------------------- */
float shelving_I_ordine(float x, float ft, float G, float *xw, char *tipo)
{
    float k, V0, H0, a=0, xh;
    float y1, y;

    k = tan(M_PI * ft / (float)SAMPLE_RATE);
    V0 = pow(10, G/20.0);   // G è in dB
    H0 = V0 - 1;

    if (G >= 0)
        a = (k - 1) / (k + 1);     // Boost
    else
        if (strcmp(tipo, "passa_basso") == 0)
            a = (k - V0) / (k + V0);   // Cut
        else if (strcmp(tipo, "passa_alto") == 0)
            a = (V0 * k - 1) / (V0 * k + 1);   // Cut
        else
            cout << "Tipo di filtro non supportato!" << endl;

    y = 0.0;
    xh = x - a * (*xw);
    y1 = a * xh + (*xw);
    *xw = xh;

    if (strcmp(tipo, "passa_basso") == 0)
        y = 0.5 * H0 * (x + y1) + x;
    else if (strcmp(tipo, "passa_alto") == 0)
        y = 0.5 * H0 * (x - y1) + x;
    else
        cout << "Tipo di filtro non supportato!" << endl;

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
    float ft = data->ft, G = data->G;
    float wL = data->wL, wR = data->wR;
    unsigned long i;
    (void)timeInfo;
    (void)statusFlags;

    for (i = 0; i<framesPerBuffer; i++)
    {
        *out++ = shelving_I_ordine(*in++, ft, G, &wL, (char*)TIPO);
        *out++ = shelving_I_ordine(*in++, ft, G, &wR, (char*)TIPO);
    }

    data->wL = wL;
    data->wR = wR;

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
    h->ft = FT;
    h->G = 10.0;
    h->wL = 0.0;
    h->wR = 0.0;

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

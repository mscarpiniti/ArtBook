#include <iostream>
#include <string.h>
#include "audioIO.h"

#define F1   (400)
#define F2   (4000)
#define TIPO ("passa_banda")  //("arresta_banda")

using namespace std;

typedef struct
{
    float *a;
    float *b;
    float *wL;
    float *wR;
    int N;
} filtro;


/* Filtro IIR ------------------------------------------------------ */
float IIR(float x, float *a, float *b, float *buffer, int N)
{
    int i;
    float y;

    buffer[0] = x;        /* Lettura dell'ingresso */

    for (i = 1; i <= N; i++)
        buffer[0] -= a[i] * buffer[i];  /* Accumulo delle uscite passate */

    y = b[N] * buffer[N];

    for (i = N - 1; i >= 0; i--)
    {
        buffer[i + 1] = buffer[i];   /* Scorrimento del buffer */
        y += b[i] * buffer[i];       /* Accumulo degli ingressi passati */
    }

    return y;
}



/* Parametri del filtro del II ordine ----------------------- */
void filtro_II_ordine(float *a, float *b, float f1, float f2, char *tipo)
{
    float k, Q, f0, fb, den;

    f0 = (f1 + f2) / 2;
    fb = f2 - f1;
    k = tan(M_PI * f0 / (float)SAMPLE_RATE);

    if (strcmp(tipo, "passa_basso") == 0)
    {
        Q = 1 / sqrt(2);
        den = k*k*Q + k + Q;
        a[0] = 1.0;
        a[1] = 2 * Q*(k*k - 1) / den;
        a[2] = (k*k*Q - k + Q) / den;
        b[0] = k*k*Q / den;
        b[1] = 2*k*k*Q / den;
        b[2] = k*k*Q / den;
    }
    else if (strcmp(tipo, "passa_alto") == 0)
    {
        Q = 1 / sqrt(2);
        den = k*k*Q + k + Q;
        a[0] = 1.0;
        a[1] = 2 * Q*(k*k - 1) / den;
        a[2] = (k*k*Q - k + Q) / den;
        b[0] = Q / den;
        b[1] = -2*Q / den;
        b[2] = Q / den;
    }
    else if (strcmp(tipo, "passa_banda") == 0)
    {
        Q = f0 / fb;
        den = k*k*Q + k + Q;
        a[0] = 1.0;
        a[1] = 2 * Q*(k*k - 1) / den;
        a[2] = (k*k*Q - k + Q) / den;
        b[0] = k / den;
        b[1] = 0.0;
        b[2] = -k / den;
    }
    else if (strcmp(tipo, "arresta_banda") == 0)
    {
        Q = f0 / fb;
        den = k*k*Q + k + Q;
        a[0] = 1.0;
        a[1] = 2 * Q*(k*k - 1) / den;
        a[2] = (k*k*Q - k + Q) / den;
        b[0] = Q*(1 + k*k) / den;
        b[1] = 2 * Q*(k*k - 1) / den;
        b[2] = Q*(1 + k*k) / den;
    }
    else
    {
        a[0] = 1.0;
        a[1] = 0.0;
        a[2] = 0.0;
        b[0] = 1.0;
        b[1] = 0.0;
        b[2] = 0.0;
        cout << "Tipo di filtro non supportato!" << endl;
    }
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
    float *a = data->a, *b = data->b, *wL = data->wL, *wR = data->wR;
    int N = data->N;
    unsigned long i;
    (void)timeInfo;
    (void)statusFlags;


    for (i = 0; i<framesPerBuffer; i++)
    {
        *out++ = IIR(*in++, a, b, wL, N - 1);
        *out++ = IIR(*in++, a, b, wR, N - 1);
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
    float f1, f2;

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

    f1 = F1;
    f2 = F2;
    h = (filtro*)malloc(sizeof(filtro));
    h->N = 3;
    h->a = (float*)calloc(h->N, sizeof(float));
    h->b = (float*)calloc(h->N, sizeof(float));
    h->wL = (float*)calloc(h->N, sizeof(float));
    h->wR = (float*)calloc(h->N, sizeof(float));

    filtro_II_ordine(h->a, h->b, f1, f2, (char*)TIPO);
    for (int i = 0; i < h->N; i++)
    {
        h->wL[i] = 0.0;
        h->wR[i] = 0.0;
    }

    err = Pa_OpenStream(&stream,
        &inputParameters,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        stream_Callback,
        h);
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

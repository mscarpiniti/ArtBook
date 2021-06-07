#include <iostream>
#include <string.h>
#include "audioIO.h"

#define FT   (4000)
#define TIPO ("passa_basso")  //("passa_alto")

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



/* Parametri del filtro del I ordine ----------------------- */
void filtro_I_ordine(float *a, float *b, float ft, char *tipo)
{
    float k;

    k = tan(M_PI * ft / (float)SAMPLE_RATE);

    a[0] = 1.0;
    a[1] = (k - 1) / (k + 1);

    if (strcmp(tipo, "passa_basso") == 0)
    {
        b[0] = k / (k + 1);
        b[1] = k / (k + 1);
    }
    else if (strcmp(tipo, "passa_alto") == 0)
    {
        b[0] = 1 / (k + 1);
        b[1] = -1 / (k + 1);
    }
    else
    {
        b[0] = 1;
        b[1] = 0;
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
    float ft;

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

    ft = FT;
    h = (filtro*)malloc(sizeof(filtro));
    h->N = 2;
    h->a = (float*)calloc(h->N, sizeof(float));
    h->b = (float*)calloc(h->N, sizeof(float));
    h->wL = (float*)calloc(h->N, sizeof(float));
    h->wR = (float*)calloc(h->N, sizeof(float));

    filtro_I_ordine(h->a, h->b, ft, (char*)TIPO);
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

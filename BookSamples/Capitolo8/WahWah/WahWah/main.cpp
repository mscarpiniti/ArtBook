#include <iostream>
#include "audioIO.h"

#define ALPHA  0.5
#define M      3
#define FW     400.0

using namespace std;


typedef struct {
    unsigned long idx;    /* Indice della sinusoide per la variazione dell'angolo */
    double alpha;         /* Coefficiente wet/dry */
    double a[(int)M];     /* Coefficienti del denominatore del filtro */
    double b[(int)M];     /* Coefficienti del numeratore del filtro */
    double wL[(int)M];    /* Stato del filtro (canale sinistro) */
    double wR[(int)M];    /* Stato del filtro (canale destro) */
} WahData;


/* Filtro FIR ------------------------------------------------------ */
double FIR(float x, double *h, double *buffer, int N)
{
    int i;
    double y;

    buffer[0] = x;    /* Lettura dell'ingresso */
    y = h[N] * buffer[N];
    for (i = N - 1; i >= 0; i--)
    {
        buffer[i + 1] = buffer[i];    /* Scorrimento del buffer */
        y += h[i] * buffer[i];        /* Accumulo dell'uscita */
    }

    return y;
}


/* Filtro IIR ------------------------------------------------------ */
double IIR(float x, double *a, double *b, double *buffer, int N)
{
    int i;
    double y;

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


/* WahWah CallBack function --------------------------------------- */
int wahwah_Callback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
    float *in = (float *)inputBuffer, *out = (float *)outputBuffer;
    WahData *data = (WahData *)userData;
    (void)timeInfo;
    (void)statusFlags;
    unsigned long i, idx = data->idx;
    double alpha = data->alpha, theta;
    double *a = data->a, *b = data->b, *wL = data->wL, *wR = data->wR;
    //double *h = data->h, *w1 = data->w1, *w2 = data->w2, *w3 = data->w3;

    double pi_norm = (double)M_PI / SAMPLE_RATE;
    theta = 0.5 * (sin(2 * pi_norm * FW * idx++) + 1.0);   /* Variazione sinusoidale dell'angolo */

    double  g = 0.1*pow(4.0, theta);
    double f0 = 450.0 * pow(2.0, 2.3*(theta));
    double  Q = pow(2.0, 1.0 + 2.0 * (1.0 - (theta)));
    double a1 = -2.0 * (1.0 - pi_norm * f0 / Q)*cos(2.0 * pi_norm * f0);
    double a2 = (1.0 - pi_norm * f0 / Q) * (1.0 - pi_norm * f0 / Q);

    b[0] = g;
    a[1] = a1;
    a[2] = a2;

    for (i = 0; i < 2*framesPerBuffer; i+=2)
    {
        *out++ = (float)(alpha * IIR(in[i], a, b, wL, M - 1) + (1.0 - alpha) * in[i])/5.;
        *out++ = (float)(alpha * IIR(in[i+1], a, b, wR, M - 1) + (1.0 - alpha) * in[i+1])/5.;
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
        << "Messaggio di errore: " << Pa_GetErrorText(p_err) << endl;
    return -1;
}


/* MAIN ----------------------------------------------------------- */
int main()
{
    PaError err;
    PaDeviceIndex devin, devout;
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream;
    WahData *data = (WahData *)calloc(sizeof(WahData), 1);

    err = Pa_Initialize();
    if (err != paNoError)
        return(printError(err));

    devin = Pa_GetDefaultInputDevice();
    devout = Pa_GetDefaultOutputDevice();

    inputParameters.device = devin;
    inputParameters.channelCount = NUM_CANALI;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowOutputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = (PaDeviceIndex)devout;
    outputParameters.channelCount = NUM_CANALI;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    data->idx = 0;
    data->alpha = (double)ALPHA;
    data->a[0] = 1.0;
    data->b[0] = 1.0;
    data->wL[0] = 0.0;
    data->wR[0] = 0.0;
    for (int i = 1; i < M; i++)
    {
        data->a[i] = 0.0;
        data->b[i] = 0.0;
        data->wL[i] = 0.0;
        data->wR[i] = 0.0;
    }

    err = Pa_OpenStream(&stream,
        &inputParameters,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        wahwah_Callback,
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

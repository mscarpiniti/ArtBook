#include <iostream>
#include <cmath>
#include "audioIO.h"

#define LF 150

#define S_NG -45.0
#define S_E  -30.0
#define S_C  -10.0
#define S_L    0.0

#define R_E  0.25
#define R_C  4.0

#define T_a  0.75
#define T_r  0.50
#define T_m  0.75

#define db2mag(X) pow(10.0, X/20.0)   /* Macro per convertire dB in valori naturali */

using namespace std;

typedef struct {
    float livello;         /* Livello precedente */
    float w[(int)LF];      /* Buffer per il filtro FIR */
    float h[(int)LF];      /* Coefficienti del filtro FIR */
} myData;


/* Calcolo valore di picco ---------------------------------------- */
float valore_picco(float x, float p_old)
{
    float tau_a = T_a, tau_r = T_r;
    float x_p, x_p_old, x_abs;

    x_p_old = (float)pow(10, p_old / 20.0);
    x_abs = abs(x);
    if (x_abs > x_p_old)
        x_p = tau_a * x_abs + (1 - tau_a) * x_p_old;
    else
        x_p = tau_r * x_abs + (1 - tau_r) * x_p_old;

    return 20 * (log10f(x_p));
}


/* Calcolo del valore efficace RMS -------------------------------- */
float valore_rms(float x, float rms_old)
{
    float tau_m = T_m;
    float x_rms, x_rms_old;

    x_rms_old = (float)pow(10, rms_old / 10.0);
    x_rms = tau_m * x * x + (1 - tau_m) * x_rms_old;

    return 10 * (log10f(x_rms));
}


/* Filtro FIR ------------------------------------------------------ */
float FIR(float x, float *h, float *buffer, int N)
{
    int i;
    float y;

    buffer[0] = x;    /* Lettura dell'ingresso */
    y = h[N] * buffer[N];
    for (i = N - 1; i >= 0; i--)
    {
        buffer[i + 1] = buffer[i];    /* Scorrimento del buffer */
        y += h[i] * buffer[i];        /* Accumulo dell'uscita */
    }

    return y;
}


/* Dynamic CallBack function -------------------------------------- */
int dynamic_Callback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
    (void)timeInfo;
    (void)statusFlags;
    float *in = (float *)inputBuffer, *out = (float *)outputBuffer;
    myData *data = (myData *)userData;
    float g, G, IN, livello = data->livello;
    float *h = data->h, *w = data->w;
    unsigned long i;


    for (i = 0; i < framesPerBuffer; i++) {
        IN = valore_picco(*in, livello);    /* Valore di picco */
        //IN = valore_rms(*in, livello);        /* Valore RMS */
        livello = IN;

        if ((IN < (float)S_C) && (IN >= (float)S_E))              /* Zona lineare */
            G = (float)0.0;
        else if (IN < (float)S_NG)                                /* Noise Gate */
            G = -1.0 * (float)1e5;
        else if ((IN < (float)S_E) && (IN >= (float)S_NG))        /* Espansore */
            G = (float)(1.0 - 1.0/(float)R_E)*((float)S_E - IN);
        else if ((IN < (float)S_L) && (IN >= (float)S_C))         /* Compressore */
            G = (float)(1.0 - 1.0/(float)R_C)*((float)S_C - IN);
        else                                                      /* Limiter */
            G = (float)S_L - IN;

        g = (float)db2mag(G);
        g = FIR(g, h, w, (int)LF-1);    /* Filtro passa-basso di tipo FIR */

        *out++ = g * (*in++);
    }

    data->livello = livello;

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
    myData *data = (myData *)calloc(sizeof(myData), 1);

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

    data->livello = 0.0;
    for (int i = 0; i < LF; i++)
    {
        data->h[i] = 1.0/(float)LF;
        data->w[i] = 0.0;
    }

    err = Pa_OpenStream(&stream,
        &inputParameters,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        dynamic_Callback,
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

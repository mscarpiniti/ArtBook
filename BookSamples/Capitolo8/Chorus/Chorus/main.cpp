#include <iostream>
#include "audioIO.h"

#define MAX(a,b) ((a) > (b) ? a : b)

#define LB 8192
#define MODFREQ1 4   // in Hz
#define MODFREQ2 3   // in Hz
#define WIDTH1   9   // in ms
#define WIDTH2   9   // in ms
#define G1 0.35
#define G2 0.35

using namespace std;


typedef struct {
    float buffer[(int)LB];
    int pt_l1;                 /* Puntatore in lettura ramo 1 */
    int pt_l2;                 /* Puntatore in lettura ramo 2 */
    int pt_s;                  /* Puntatore in scrittura */
    unsigned long idx;         /* Indice per la sinusoide dell'LFO */
    float D1_1;                /* Profondità dell'effetto ramo 1 */
    float D1_2;                /* Profondità dell'effetto ramo 2 */
    float g1;                  /* Guadagno ramo 1 */
    float g2;                  /* Guadagno ramo 2 */
} myBuffer;



/* Chorus CallBack function --------------------------------------------- */
static int chorus_Callback(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData)
{
    float *in = (float *)inputBuffer, *out = (float *)outputBuffer;
    myBuffer *data = (myBuffer *)userData;
    float f_D1, rp1, alpha1, temp1, g1 = data->g1;
    float f_D2, rp2, alpha2, temp2, g2 = data->g2;
    float *buffer = data->buffer, D1_1 = data->D1_1, D1_2 = data->D1_2;
    unsigned long i, idx = data->idx;
    int rpi1, rpi2, pt_l1 = data->pt_l1, pt_l2 = data->pt_l2, pt_s = data->pt_s;
    float f1 = (float)MODFREQ1 / (float)SAMPLE_RATE;
    float f2 = (float)MODFREQ2 / (float)SAMPLE_RATE;
    (void)timeInfo;
    (void)statusFlags;


    for (i = 0; i < framesPerBuffer; i++)
    {
        buffer[pt_s++] = *in;

        /* Primo ramo */
        // LFO SINUSOIDALE
        f_D1 = (float)sin(2 * M_PI * f1 * idx);
        rp1 = (pt_l1++) + D1_1 * f_D1;

        if (rp1 >= 0) {
            if (rp1 >= LB)
                rp1 = rp1 - (int)LB;
        }
        else {
            rp1 = rp1 + (int)LB;
        }
        rpi1 = (int)rp1;
        alpha1 = rp1 - rpi1;

        if (rpi1 != LB - 1)
            temp1 = buffer[rpi1] + alpha1*(buffer[rpi1 + 1] - buffer[rpi1]);
        else
            temp1 = buffer[rpi1] + alpha1*(buffer[0] - buffer[rpi1]);


        /* Secondo ramo */
        // LFO SINUSOIDALE
        f_D2 = (float)sin(2 * M_PI * f2 * idx++);
        rp2 = (pt_l2++) + D1_2 * f_D2;

        if (rp2 >= 0) {
            if (rp2 >= LB)
                rp2 = rp2 - (int)LB;
        }
        else {
            rp2 = rp2 + (int)LB;
        }
        rpi2 = (int)rp2;
        alpha2 = rp2 - rpi2;

        if (rpi2 != LB - 1)
            temp2 = buffer[rpi2] + alpha2*(buffer[rpi2 + 1] - buffer[rpi2]);
        else
            temp2 = buffer[rpi2] + alpha2*(buffer[0] - buffer[rpi2]);

        /* Uscita del Chorus */
        *out++ = (1 - g1 - g2) * (*in++) + g1*(float)temp1 + g2*(float)temp2;

        if (pt_l1 >= LB)
            pt_l1 = 0;

        if (pt_l2 >= LB)
            pt_l2 = 0;

        if (pt_s >= LB)
            pt_s = 0;
    }

    data->pt_l1 = pt_l1;
    data->pt_l2 = pt_l2;
    data->pt_s = pt_s;
    data->idx  = idx;

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

    float D0_1 = round((float)WIDTH1*SAMPLE_RATE / 1000);
    float D0_2 = round((float)WIDTH2*SAMPLE_RATE / 1000);
    data->D1_1 = round((float)WIDTH1*SAMPLE_RATE / 1000);
    data->D1_2 = round((float)WIDTH2*SAMPLE_RATE / 1000);
    data->idx = 0;
    data->pt_l1 = 0;
    data->pt_l2 = 0;
    data->pt_s = 1 + (int)MAX(D0_1, D0_2);
    data->g1 = (float)G1;
    data->g2 = (float)G2;
    cout << "Profondita' effetto in campioni: " << data->pt_s - 1 << endl;

    err = Pa_OpenStream(&stream,
        &inputParameters,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        chorus_Callback,
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

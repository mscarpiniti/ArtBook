#include <iostream>
#include <math.h>
#include "audioIO.h"
#include "fftw3.h"

#define M    584 //732 = (N + 440)/2
#define N   1024
#define LB  8192

using namespace std;


typedef struct {
    fftw_complex *inCpx;
    fftw_complex *outCpx;
    fftw_complex *tempCpx;
    fftw_plan p1;
    fftw_plan p2;
} myCpxData;

typedef struct {
    float inBuffer[(int)LB];
    float outBuffer[(int)LB];
    int pt_in;
    int pt_out;
} myBuffer;

typedef struct {
    myCpxData fftData;
    myBuffer bufferData;
} myData;



/* Robotic CallBack function con STFT ------------------- */
static int robotic_Callback( const void* inputBuffer, void* outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void* userData )
{
    (void)timeInfo;
    (void)statusFlags;
    float *inp = (float*)inputBuffer, *outp = (float*)outputBuffer;
    myData *data = (myData *)userData;
    myCpxData* fft = &data->fftData;
    myBuffer*  buf = &data->bufferData;
    fftw_complex *in = fft->inCpx, *outt = fft->tempCpx;
    fftw_complex *out = fft->outCpx;
    fftw_plan p1 = fft->p1, p2 = fft->p2;
    float *inBuffer = buf->inBuffer, *outBuffer = buf->outBuffer, w;
    int ptIn_s = buf->pt_in, ptOut_s = buf->pt_out, ptIn_l, ptOut_l;
    unsigned int L = N - M;
    unsigned long i;


    // Riporto indietro il puntatore di lettura
    ptIn_l = ptIn_s - L;
    if (ptIn_l < 0)
        ptIn_l = ptIn_l + LB;

    // Inserisco i nuovi campioni nel buffer
    for (i = 0; i < framesPerBuffer; i++) {
        inBuffer[ptIn_s++] = *inp++;
        if (ptIn_s >= LB)
            ptIn_s = 0;
    }

    // Seleziono i campioni da trasformare
    for (i = 0; i < N; i++) {
        w = 0.5 * (1.0 - cos(2 * M_PI * i / (N+1)));
        in[i][0] = (float)(w * inBuffer[ptIn_l++]);
        in[i][1] = 0.0;
        if (ptIn_l >= LB)
            ptIn_l = 0;
    }

    // Eseguo la FFT
    fftw_execute(p1);

    // Impongo la fase a zero
    for (i = 0; i < N; i++) {
        out[i][0] = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1])/N;
        out[i][1] = 0.0;
    }

    // Eseguo la IFFT
    fftw_execute(p2);

    // Riporto indietro il puntatore di scrittura
    ptOut_s = ptOut_s - L;
    if (ptOut_s < 0)
        ptOut_s = ptOut_s + LB;

    // Overlap and Add
    for (i = 0; i < L; i++) {
        w = 0.5 * (1.0 - cos(2 * M_PI * i / (N+1)));
        outBuffer[ptOut_s] = outBuffer[ptOut_s] + (float)(w * outt[i][0]);
        ptOut_s++;
        if (ptOut_s >= LB)
            ptOut_s = 0;
    }

    for (i = L; i < N; i++) {
        w = 0.5 * (1.0 - cos(2 * M_PI * i / (N+1)));
        outBuffer[ptOut_s++] = (float)(w * outt[i][0]);
        if (ptOut_s >= LB)
            ptOut_s = 0;
    }

    // Riporto indietro il puntatore di lettura
    ptOut_l = ptOut_s - N;
    if (ptOut_l < 0)
        ptOut_l = ptOut_l + LB;

    // Seleziono i primi campioni
    for (i = 0; i < framesPerBuffer; i++) {
        outp[i] = outBuffer[ptOut_l++];
        if (ptOut_l >= LB)
            ptOut_l = 0;
    }

    // Salvo la posizione del puntatore
    buf->pt_in  = ptIn_s;
    buf->pt_out = ptOut_s;

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
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowOutputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = (PaDeviceIndex)devout;
    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    data->fftData.inCpx   = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    data->fftData.tempCpx = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    data->fftData.outCpx  = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    data->fftData.p1 = fftw_plan_dft_1d(N, data->fftData.inCpx, data->fftData.outCpx, FFTW_FORWARD, FFTW_ESTIMATE);
    data->fftData.p2 = fftw_plan_dft_1d(N, data->fftData.outCpx, data->fftData.tempCpx, FFTW_BACKWARD, FFTW_ESTIMATE);
    data->bufferData.pt_in = N - M;
    data->bufferData.pt_out = 0;

    for (int i = 0; i < LB; i++) {
        data->bufferData.inBuffer[i]  = 0.0;
        data->bufferData.outBuffer[i] = 0.0;
    }

    err = Pa_OpenStream(&stream,
                        &inputParameters,
                        &outputParameters,
                        SAMPLE_RATE,
                        M,
                        paClipOff,
                        robotic_Callback,
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

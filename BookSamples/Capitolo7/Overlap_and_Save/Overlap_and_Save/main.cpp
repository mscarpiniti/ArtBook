#include <iostream>
#include <fstream>
#include <math.h>
#include "audioIO.h"
#include "fftw3.h"

#define M    512
#define N   1024
#define LB  8192

using namespace std;


typedef struct {
    fftw_complex *inCpx;
    fftw_complex *outCpx;
    fftw_complex *tempCpx;
    fftw_complex *H;
    fftw_plan p1;
    fftw_plan p2;
} myCpxData;

typedef struct {
    float buffer[(int)LB];
    int pt;
} myBuffer;

typedef struct {
    myCpxData fftData;
    myBuffer bufferData;
} myData;



/* OVERLAP AND SAVE CallBack function ------------------- */
static int OLS_Callback( const void* inputBuffer, void* outputBuffer,
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
    fftw_complex *H = fft->H;
    fftw_complex temp;
    fftw_plan p1 = fft->p1, p2 = fft->p2;
    float *buffer = buf->buffer;
    int pt_s = buf->pt, pt_l;
    int L = N - M;
    unsigned long i;


    // Riporto indietro il puntatore di lettura
    pt_l = pt_s - L;
    if (pt_l < 0)
        pt_l = pt_l + LB;

    // Inserisco i nuovi campioni nel buffer
    for (i = 0; i < framesPerBuffer; i++) {
        buffer[pt_s++] = *inp++;

        if (pt_s >= LB)
            pt_s = 0;
    }

    // Seleziono i campioni da trasformare
    for (i = 0; i < N; i++) {
        in[i][0] = buffer[pt_l++];
        in[i][1] = 0.0;

        if (pt_l >= LB)
            pt_l = 0;
    }

    // Eseguo la FFT
    fftw_execute(p1);

    // Teorema della convoluzione: Y = H*X
    for (i = 0; i < N; i++) {
        temp[0] = (H[i][0] * out[i][0] - H[i][1] * out[i][1])/N;
        temp[1] = (H[i][0] * out[i][1] + H[i][1] * out[i][0])/N;
        out[i][0] = temp[0];
        out[i][1] = temp[1];
    }

    // Eseguo la IFFT
    fftw_execute(p2);

    // Seleziono gli ultimi campioni
    for (i = 0; i < framesPerBuffer; i++) {
        outp[i] = outt[i + L][0];
    }

    // Salvo la posizione del puntatore
    buf->pt = pt_s;

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


/* FILEREADER ----------------------------------------------------- */
void fileReader(char *file_name, float *h)
{
    fstream fs;
    int length;

    fs.open(file_name, fstream::in | fstream::binary);
    if (!fs)
    {
        cout << "File " << file_name << " non trovato!\n" << endl;
        fflush(stdin);
        exit(1);
    }

    fs.seekg(0, ios::end);
    length = fs.tellg();
    fs.seekg(0, ios::beg);

    float q[length];
    fs.read((char *)q, length);
    fs.close();
    cout << length << "dati letti dal file " << file_name << endl;

    for(int i=0; i<M; i++)
        h[i] = (float)q[i];
}


/* MAIN ----------------------------------------------------------- */
int main()
{
    PaError err;
    PaDeviceIndex devin, devout;
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream;
    myData *data = (myData *)calloc(sizeof(myData), 1);
    float *h = (float *)calloc(sizeof(float), M);
    char file_name[] = "FiltroPB.dat";
    fftw_plan myplan;
    fftw_complex *H = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex *hh = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    myplan = fftw_plan_dft_1d(N, hh, H, FFTW_FORWARD, FFTW_ESTIMATE);

    fileReader(file_name, h);

    for (int i = 0; i < M; i++) {
        hh[i][0] = 0.0;
        hh[i][1] = 0.0;
    }

    for (int i = M; i < N; i++) {
        hh[i][0] = 0.0;
        hh[i][1] = 0.0;
    }

    // Eseguo la FFT
    fftw_execute(myplan);

    /* Inizializzazione di PortAudio*/
    err = Pa_Initialize();
    if (err != paNoError)
        return( printError(err));

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
    data->bufferData.pt = N - M;
    data->fftData.H = H;

    for (int i = 0; i < LB; i++)
        data->bufferData.buffer[i] = 0.0;

    err = Pa_OpenStream(&stream,
                        &inputParameters,
                        &outputParameters,
                        SAMPLE_RATE,
                        M,
                        paClipOff,
                        OLS_Callback,
                        data);
    if (err != paNoError)
        return( printError(err));

    err = Pa_StartStream(stream);
    if (err != paNoError)
        return( printError(err));

    printf("Premi ENTER per terminare il programma.\n");
    getchar();

    err = Pa_StopStream(stream);
    if (err != paNoError)
        return( printError(err));

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        return( printError(err));

    Pa_Terminate();
    fftw_destroy_plan(myplan);
    free(data);

    return 0;
}

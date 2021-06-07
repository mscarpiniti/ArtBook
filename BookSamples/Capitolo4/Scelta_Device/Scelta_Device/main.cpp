#include <iostream>
#include "portaudio.h"

#define SAMPLE_RATE (44100)
#define FRAMES_PER_BUFFER (256)

using namespace std;

typedef float SAMPLE;


static int My_Callback(const void *inputBuffer,
                       void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void* userData)
{
    SAMPLE *out = (SAMPLE*)outputBuffer;
    const SAMPLE *in = (const SAMPLE*)inputBuffer;
    unsigned int i;
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if (inputBuffer == NULL)
        for (i = 0; i < framesPerBuffer; i++)
        {
            *out++ = 0;
            *out++ = 0;
        }
    else
        for (i = 0; i < framesPerBuffer; i++)
        {
            *out++ = *in++;
            *out++ = *in++;
        }

    return paContinue;
}


int printError(PaError p_err)
{
    Pa_Terminate();
    cout << "Si è verificato un errore" << endl
         << "Codice di errore: " << p_err << endl
         << "Messaggio di errore: " << Pa_GetErrorText( p_err ) << endl;
    return -1;
}


int main(void)
{
    PaError err;
    PaDeviceIndex devin, devout;
    const PaDeviceInfo *info;
    const PaHostApiInfo *api;
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream;
    int i, ndev;

    err = Pa_Initialize();
    if (err != paNoError)
        return( printError(err) );

    /* Scegliere un device */
    ndev = Pa_GetDeviceCount();
    for (i = 0; i<ndev; i++) {
        info = Pa_GetDeviceInfo(i);
        api = Pa_GetHostApiInfo(info->hostApi);
        if (info->maxOutputChannels > 0)
            cout << "Dispositivo di output: ";
        if (info->maxInputChannels > 0)
            cout << "Dispositivo di input: ";
        cout <<  i << ": [" << api->name << "] " << info->name << endl;
    }
    cout << "Scegli un dispositivo di input: ";
    cin >> devin;
    cout << endl << "Scegli un dispositivo di output: ";
    cin >> devout;

    inputParameters.device = devin;
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = devout;
    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowInputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paNoFlag,
              My_Callback,
              NULL );
    if (err != paNoError)
        return( printError(err) );

    Pa_StartStream(stream);
    if (err != paNoError)
        return( printError(err) );

    cout << "Parla per 5 secondi." << endl;
    Pa_Sleep( 5000 );

    Pa_StopStream(stream);
    if (err != paNoError)
        return( printError(err) );

    Pa_CloseStream(stream);
    if (err != paNoError)
        return( printError(err) );

    Pa_Terminate();

    return 0;
}

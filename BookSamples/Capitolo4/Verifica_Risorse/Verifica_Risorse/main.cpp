#include <iostream>
#include <fstream>
#include "portaudio.h"

using namespace std;


int printError(PaError p_err)
{
    Pa_Terminate();
    cout << "Si è verificato un errore" << endl
        << "Codice di errore: " << p_err << endl
        << "Messaggio di errore: " << Pa_GetErrorText( p_err ) << endl;
    return -1;
}


void printDevice(int numDevice, const PaDeviceInfo *deviceInfo)
{
    cout << "------------ DEVICE N. " << numDevice << " ------------" << endl
        << "Nome: " << deviceInfo->name << endl
        << "Massimo numero di canali di input: " << deviceInfo->maxInputChannels << endl
        << "Massimo numero di canali di output: " << deviceInfo->maxOutputChannels << endl
        << "Minima latenza di input: " << deviceInfo->defaultLowInputLatency << endl
        << "Minima latenza di output: " << deviceInfo->defaultLowOutputLatency << endl
        << "Frequenza di campionamento predefinita: " << deviceInfo->defaultSampleRate << endl
        << "Host API predefinita: " << Pa_GetHostApiInfo(deviceInfo->hostApi)->name << endl<< endl;
}


void printDeviceFile(int numDevice, const PaDeviceInfo *deviceInfo, fstream &fs)
{
    fs << "------------ DEVICE N. " << numDevice << " ------------" << endl
        << "Nome: " << deviceInfo->name << endl
        << "Massimo numero di canali di input: " << deviceInfo->maxInputChannels << endl
        << "Massimo numero di canali di output: " << deviceInfo->maxOutputChannels << endl
        << "Minima latenza di input: " << deviceInfo->defaultLowInputLatency << endl
        << "Minima latenza di output: " << deviceInfo->defaultLowOutputLatency << endl
        << "Frequenza di campionamento predefinita: " << deviceInfo->defaultSampleRate << endl
        << "Host API predefinita: " << Pa_GetHostApiInfo(deviceInfo->hostApi)->name << endl<< endl;
}


int main(void)
{
    const PaDeviceInfo  *deviceInfo, *deviceInputInfo, *deviceOutputInfo;
    PaError err = paNoError;
    int     i, numDevice, defInputDevice, defOutputDevice;
    fstream fstr;

    cout << "Test delle schede audio................" << endl;

    err = Pa_Initialize();
    if (err != paNoError)
        return(printError(err));

    numDevice = Pa_GetDeviceCount();
    if (numDevice < 0)
        return(printError(numDevice));

    cout << "Sono presenti " << numDevice << " dispositivi audio" << endl;

    fstr.open("DEVICE.txt", fstream::in | fstream::out | fstream::app);

    for (i = 0; i < numDevice; i++)
    {
        deviceInfo = Pa_GetDeviceInfo(i);
        printDevice(i, deviceInfo);
        printDeviceFile(i, deviceInfo, fstr);
    }

    /* Device di input di default */
    defInputDevice = Pa_GetDefaultInputDevice();
    deviceInputInfo = Pa_GetDeviceInfo(defInputDevice);
    cout << "Dispositivo predefinito di input: " << endl << endl;
    printDevice(defInputDevice, deviceInputInfo);
    fstr << "Dispositivo predefinito di input: " << endl << endl;
    printDeviceFile(defInputDevice, deviceInputInfo, fstr);

    /* Device di output di default */
    defOutputDevice = Pa_GetDefaultOutputDevice();
    deviceOutputInfo = Pa_GetDeviceInfo(defOutputDevice);
    cout << "Dispositivo predefinito di output: " << endl << endl;
    printDevice(defOutputDevice, deviceOutputInfo);
    fstr << "Dispositivo predefinito di output: " << endl << endl;
    printDeviceFile(defOutputDevice, deviceOutputInfo, fstr);

    cout << "..................Fine." << endl << endl;

    fstr.close();

    err = Pa_Terminate();
    if (err != paNoError)
    {
        cout << "Si è verificato un errore" << endl
            << "Codice di errore: " << err << endl
            << "Messaggio di errore: " << Pa_GetErrorText(err) << endl;
    }

    getchar();

    return 0;
}

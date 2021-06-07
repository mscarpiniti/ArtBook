// Oscillatore_Tabellare.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include <iostream>
#include "audioIO.h"

#define TABLE_SIZE   (2000)   /* Lunghezza della tabella */

#define SINE   (1)   /* Per selezionare l'onda sinusoidale */
#define SQUARE (2)   /* Per selezionare l'onda quadra */
#define SAW    (3)   /* Per selezionare l'onda a dente di sega */
#define TRIANG (4)   /* Per selezionare l'onda triangolare */


using namespace std;

typedef struct
{
    float table[TABLE_SIZE];  /* Tabella dei campioni */
    float freq;               /* Frequenza dell'oscillazione */
    float left_phase;         /* Fase del canale sinistro */
    float right_phase;        /* Fase del canale destro */
    float gain_left;          /* Guadagno del canale sinistro */
    float gain_right;         /* Guadagno del canale destro */
} tabData;


/* Funzione di callback per l'oscillatore */
static int oscill_Callback(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    tabData *data = (tabData*)userData;
    float *out = (float*)outputBuffer;
    float incr, f0 = data->freq;
    unsigned long i;

    /* Evitiamo warning per variabili non utilizzate. */
    (void)timeInfo;
    (void)statusFlags;
    (void)inputBuffer;

    incr = f0 * (float)TABLE_SIZE / (float)SAMPLE_RATE;

    for (i = 0; i<framesPerBuffer; i++)
    {
        /* Il buffer contiene campioni dei due canali in modo interlacciato, */
        *out++ = data->gain_left * data->table[(int)data->left_phase];     /* canale sinistro */
        *out++ = data->gain_right * data->table[(int)data->right_phase];   /* canale destro */

        data->left_phase += incr;
        if (data->left_phase >= TABLE_SIZE)
            data->left_phase -= TABLE_SIZE;

        data->right_phase += incr;
        if (data->right_phase >= TABLE_SIZE)
            data->right_phase -= TABLE_SIZE;
    }

    return paContinue;
}


/* Funzione printError per la stampa dei messaggi di errore */
int printError(PaError p_err)
{
    Pa_Terminate();
    cout << "Si è verificato un errore" << endl
        << "Codice di errore: " << p_err << endl
        << "Messaggio di errore: " << Pa_GetErrorText(p_err) << endl;
    return -1;
}


/* Funzione init_sine */
void init_sine(float *tabella)
{
    int i;

    for (i = 0; i<TABLE_SIZE; i++)
        tabella[i] = (float)sin(((float)i / (float)TABLE_SIZE) * M_PI * 2.0);
}


/* Funzione init_square */
void init_square(float *tabella)
{
    int i;

    for (i = 0; i<TABLE_SIZE/2; i++)
        tabella[i] = 0.8f;
    for (i = TABLE_SIZE / 2; i < TABLE_SIZE; i++)
        tabella[i] = -0.8f;
}


/* Funzione init_saw */
void init_saw(float *tabella)
{
    int i;

    for (i = 0; i<TABLE_SIZE; i++)
        tabella[i] = 1.0f - ( 2.0f * (float)i / (float)TABLE_SIZE );
}


/* Funzione init_triangle */
void init_triangle(float *tabella)
{
    int i;

    for (i = 0; i<TABLE_SIZE / 2; i++)
        tabella[i] = 2.0f * (float)i / (float)(TABLE_SIZE/2) - 1.0f;
    for (i = TABLE_SIZE / 2; i < TABLE_SIZE; i++)
        tabella[i] = 1.0f - (2.0f * (float)(i - TABLE_SIZE/2) / (float)(TABLE_SIZE/2) );
}




/* Funzione principale */
int main(int argc, char *argv[])
{
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    tabData data;
    int scelta;
    float f0;

    (void)argc;
    (void)argv;

    cout << "PortAudio Test: output sine wave. SR = " << SAMPLE_RATE << "BufSize = " << FRAMES_PER_BUFFER << endl << endl;
    cout << "Scegliere una forma d'onda, digitando:" << endl;
    cout << "1: per SINUSOIDALE" << endl;
    cout << "2: per QUADRA" << endl;
    cout << "3: per DENTE DI SEGA" << endl;
    cout << "4: per TRIANGOLARE" << endl << endl;
    cin >> scelta;

    cout << endl << "Inserire il valore della frequenza: " << endl << endl;
    cin >> f0;

    /* Inizializzazione della tabella dei campioni */
    switch (scelta)	{
    case SINE:
        init_sine(data.table);
        break;
    case SQUARE:
        init_square(data.table);
        break;
    case SAW:
        init_saw(data.table);
        break;
    case TRIANG:
        init_triangle(data.table);
        break;
    default:
        cout << "Scelta del tipo di forma d'onda non valida" << endl;
        break;
    }

    data.left_phase = data.right_phase = 0.0f;
    data.gain_left = data.gain_right = 1.0f;
    data.freq = f0;

    /* Inizializzazione di PortAudio*/
    err = Pa_Initialize();
    if (err != paNoError)
        return(printError(err));

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* device di default */
    outputParameters.channelCount = NUM_CANALI;            /* stereo */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;        /* 32 bit floating point */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)
        ->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    /* Apertura dello stream e notifica della funzione di callback */
    err = Pa_OpenStream(&stream,
        NULL,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        oscill_Callback,
        &data);
    if (err != paNoError)
        return( printError(err) );

    cout << "Avvio lo stream" << endl;
    err = Pa_StartStream( stream );
    if (err != paNoError)
        return( printError(err) );

    cout << "Lo stream sarà attivo per 5 secondi" << endl;

    /* Attendi 5 secondi prima di procedere*/
    Pa_Sleep( NUM_SECONDI * 1000 );

    err = Pa_StopStream( stream );
    if (err != paNoError)
        return( printError(err) );

    err = Pa_CloseStream( stream );
    if (err != paNoError)
        return( printError(err) );

    Pa_Terminate();
    cout << "Programma completato." << endl;
    getchar();

    return 0;
}

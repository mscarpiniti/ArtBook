#ifndef AUDIOIO_H
#define AUDIOIO_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "portaudio.h"

#define SAMPLE_RATE         (44100) //(16000)
#define FRAMES_PER_BUFFER   (128)   //(256)
#define NUM_SECONDI         (5)
#define NUM_CANALI          (2)
#define	SAMPLE_COUNT        (SAMPLE_RATE * NUM_SECONDI)
#ifndef M_PI
#define M_PI                (3.141592654)
#endif

/* Per selezionare il formato dei file */
#define RAWFILE             (0)
#define AIFFILE             (0)
#define WAVFILE             (1)

/* Per selezionare il formato dei campioni */
#if 1
   #define PA_SAMPLE_TYPE   paFloat32
   #define SAMPLE_SILENCE   (0.0f)
   #define PRINTF_S_FORMAT  "%.8f"
   typedef float SAMPLE;
#elif 0
   #define PA_SAMPLE_TYPE   paInt16
   #define SAMPLE_SILENCE   (0)
   #define PRINTF_S_FORMAT  "%d"
   typedef short SAMPLE;
#elif 0
   #define PA_SAMPLE_TYPE   paInt8
   #define SAMPLE_SILENCE   (0)
   #define PRINTF_S_FORMAT  "%d"
   typedef char SAMPLE;
#else
   #define PA_SAMPLE_TYPE   paUInt8
   #define SAMPLE_SILENCE   (128)
   #define PRINTF_S_FORMAT  "%d"
   typedef unsigned char SAMPLE;
#endif

#endif // AUDIOIO_H

#include <iostream>
#include <math.h>

#define SAMPLE_RATE 44100

using namespace std;


/* Ritardo Frazionario ---------------------------------------------------------- */
float FD_CircBuffer(float *buffer, float D, int *pt, int LB, float x)
{
    int rp, M;
    float rit, alpha;

    buffer[*pt] = x;
    M = (int)D;
    alpha = D - M;
    rp = *pt - M;
    rp = (rp >= 0 ? (rp < LB ? rp : rp - LB) : rp + LB);

    if (rp != LB-1)
        rit = (1 - alpha)*buffer[rp] + alpha*buffer[rp+1];
    else
        rit = (1 - alpha)*buffer[rp] + alpha*buffer[0];

    (*pt)++;
    if (*pt >= LB)
        *pt = 0;

    return rit;
}




int main()
{
    int i, D0, f0 = 2, *pt;
    const int N = 3000, LB = 1024;
    float rit = 0.005, *buffer, *y, x[N], D[N];

    D0 = (int)(rit*SAMPLE_RATE);
    for (i = 0; i < N; i++){
        x[i] = 2 * ((float)rand() / (float)RAND_MAX) - 1;
        D[i] = D0 * cos(2*M_PI*f0*i);
    }

    buffer = (float *)calloc(LB, sizeof(float));
    y = (float *)calloc(N, sizeof(float));
    pt = (int *)malloc(sizeof(int));

    for (i = 0; i < LB; i++)
        buffer[i] = 0;
    *pt = (int)0;

    for (i = 0; i < N; i++)
        y[i] = FD_CircBuffer(buffer, D[i], pt, LB, x[i]);

    cout << "Posizione     x   y: " << endl << endl;
    for (i = 0; i < N; i++)
        cout << i + 1 << ":    " << x[i] << "       " << y[i] << endl;

    return 0;
}

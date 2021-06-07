#include <iostream>

#define SAMPLE_RATE 44100

using namespace std;


/* Buffer Circolare ---------------------------------------------------------- */
float D_CircBuffer(float *buffer, int D, int *pt, float x)
{
    buffer[(*pt)++] = x;
    if (*pt >= D)
        *pt = 0;

    return buffer[*pt];
}



int main()
{
    int i, D, *pt;
    const int N = 2000;
    float rit = 0.005, *buffer, *y, x[N];

    for (i = 0; i < N; i++)
        x[i] = 2 * ((float)rand() / (float)RAND_MAX) - 1;

    D = (int)(rit*SAMPLE_RATE);
    buffer = (float *)calloc(D, sizeof(float));
    y = (float *)calloc(N, sizeof(float));
    pt = (int *)malloc(sizeof(int));

    for (i = 0; i < D; i++)
        buffer[i] = 0;
    *pt = (int)0;

    for (i = 0; i < N; i++)
        y[i] = D_CircBuffer(buffer, D, pt, x[i]);

    cout << "Posizione     x   y: " << endl << endl;
    for (i = 0; i < N; i++)
        cout << i + 1 << ":    " << x[i] << "       " << y[i] << endl;

    return 0;
}

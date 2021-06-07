#include <iostream>

#define SAMPLE_RATE 44100

using namespace std;


/* Filtro All-Pass ---------------------------------------------------------- */
float *all_pass(float *x, float rit, float g, float *buffer, int *pt, int N)
{
    int i, D;
    float temp, *y;
    D = (int)(rit*SAMPLE_RATE);
    y = (float *)calloc(N, sizeof(float));

    for (i = 0; i < N; i++)
    {
        temp = buffer[*pt];
        buffer[(*pt)++] = x[i] - g*temp;
        y[i] = temp + g*x[i];
        if (*pt >= D)
            *pt = 0;
    }

    return y;
}



int main()
{
    int i, D, *pt;
    const int N = 2000;
    float rit = 0.005, g = 0.7, *buffer, *y, x[N];

    for (i = 0; i < N; i++)
        x[i] = 2 * ((float)rand() / (float)RAND_MAX) - 1;

    D = (int)(rit*SAMPLE_RATE);
    buffer = (float *)calloc(D, sizeof(float));
    pt = (int *)malloc(sizeof(int));

    for (i = 0; i < D; i++)
        buffer[i] = 0;
    *pt = (int)0;

    y = all_pass(x, rit, g, buffer, pt, N);

    cout << "Risultato filtro All-Pass: " << endl << endl;
    for (i = 0; i < N; i++)
        cout << i + 1 << ": " << y[i] << endl;

    return 0;
}

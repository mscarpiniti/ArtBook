#include <iostream>

#define SAMPLE_RATE 44100

using namespace std;


/* Buffer Lineare ---------------------------------------------------------- */
float D_LinBuffer(float *buffer, int D, float x)
{
   int i;
   for(i = D-1; i >= 1; i--)
         buffer[i] = buffer[i-1];
   buffer[0] = x;

   return buffer[D-1];
}



int main()
{
    int i, D;
    const int N = 2000;
    float rit = 0.005, *buffer, *y, x[N];

    for (i = 0; i < N; i++)
        x[i] = 2 * ((float)rand() / (float)RAND_MAX) - 1;

    D = (int)(rit*SAMPLE_RATE);
    buffer = (float *)calloc(D, sizeof(float));
    y = (float *)calloc(N, sizeof(float));

    for (i = 0; i < D; i++)
        buffer[i] = 0;

    for (i = 0; i < N; i++)
        y[i] = D_LinBuffer(buffer, D, x[i]);

    cout << "Posizione     x   y: " << endl << endl;
    for (i = 0; i < N; i++)
        cout << i + 1 << ":    " << x[i] << "       " << y[i] << endl;

    return 0;
}

#include <iostream>

using namespace std;


/* Convoluzione ------------------------------------------------------ */
float *conv(float *x, float *h, int N, int M)
{
    int  L, n, k, j;
    float  temp, *y;

    /* Allocazione dell'uscita */
    L = N + M - 1;
    y = (float*)calloc(L, sizeof(float));

    /* Somma di convoluzione */
    for (n = 0; n < L; n++)
    {
        j = n;
        temp = 0.0;
        for (k = 0; k < N; k++)
        {
            if (j >= 0 && j < M)
                temp = temp + (x[k] * h[j]);

            j = j - 1;
            y[n] = temp;
        }
    }

    return y;
}



int main()
{
    int i;
    const int N = 5, M = 7;
    float x[N] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    float y[M] = { 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0 };
    float *z;

    z = conv(x, y, N, M);

    cout << "Il risultato della convoluzione tra la sequenza: ";
    for (i = 0; i < N; i++)
        cout << x[i] << " ";
    cout << endl << "e la sequenza: ";
    for (i = 0; i < M; i++)
        cout << y[i] << " ";
    cout << endl << "e' la sequenza: " << endl << endl;
    for (i = 0; i < N+M-1; i++)
        cout << i + 1 << ": " << z[i] << endl;

    return 0;
}

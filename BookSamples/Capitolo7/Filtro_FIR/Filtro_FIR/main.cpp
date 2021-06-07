#include <iostream>

using namespace std;


/* Filtro FIR ------------------------------------------------------ */
float FIR(float x, double *h, double *buffer, int N)
{
    int i;
    double y;

    buffer[0] = x;    /* Lettura dell'ingresso */
    y = h[N] * buffer[N];
    for (i = N - 1; i >= 0; i--)
    {
        buffer[i + 1] = buffer[i];    /* Scorrimento del buffer */
        y += h[i] * buffer[i];        /* Accumulo dell'uscita */
    }

    return (float)y;
}



int main()
{
    int i;
    const int N = 5, M = 20;
    double b[N], w[N];

    double B[N] = { 1.0, 2.0, -1.0, 0.5, 0.1 };

    for (i = 0; i < N; i++)
    {
        b[i] = B[i];
        w[i] = 0.0;
    }

    float x[M] = { 1.0, 2.0, 3.0, 4.0, 5.0,
        6.0, 7.0, 8.0, 9.0, 10.0,
        11.0, 12.0, 13.0, 14.0, 15.0,
        16.0, 17.0, 18.0, 19.0, 20.0 };
    float y[M];

    for (i = 0; i < M; i++)
        y[i] = FIR(x[i], b, w, N - 1);

    cout << "Risultato filtro FIR: " << endl << endl;
    for (i = 0; i < M; i++)
        cout << i + 1 << ": " << y[i] << endl;

    return 0;
}

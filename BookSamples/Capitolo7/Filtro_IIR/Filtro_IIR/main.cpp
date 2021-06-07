#include <iostream>

using namespace std;


/* Filtro IIR ------------------------------------------------------ */
float IIR(float x, double *a, double *b, double *buffer, int N)
{
    int i;
    double y;

    buffer[0] = (double)x;        /* Lettura dell'ingresso */

    for (i = 1; i <= N; i++)
        buffer[0] -= a[i] * buffer[i];  /* Accumulo delle uscite passate */

    y = b[N] * buffer[N];

    for (i = N - 1; i >= 0; i--)
    {
        buffer[i + 1] = buffer[i];   /* Scorrimento del buffer */
        y += b[i] * buffer[i];       /* Accumulo degli ingressi passati */
    }

    return (float)y;
}


int main()
{
    int i;
    const int N = 5, M = 20;
    double a[N], b[N], w[N];

    double A[N] = { 1.0000, -3.9918, 5.9754, -3.9754, 0.9918 };
    double B[N] = { 0.0606 / 10000000000,
        0.2425 / 10000000000,
        0.3638 / 10000000000,
        0.2425 / 10000000000,
        0.0606 / 10000000000 };

    for (i = 0; i < N; i++)
    {
        a[i] = A[i];
        b[i] = B[i];
        w[i] = 0.0;
    }

    float x[M] = {1.0, 2.0, 3.0, 4.0, 5.0,
        6.0, 7.0, 8.0, 9.0, 10.0,
        11.0, 12.0, 13.0, 14.0, 15.0,
        16.0, 17.0, 18.0, 19.0, 20.0};
    float y[M];

    for (i = 0; i < M; i++)
        y[i] = IIR(x[i], a, b, w, N-1);

    cout << "Risultato filtro IIR: " << endl << endl;
    for (i = 0; i < M; i++)
        cout << i + 1 << ": " << y[i] << endl;

    return 0;
}

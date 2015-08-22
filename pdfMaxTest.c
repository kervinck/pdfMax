
#include <math.h>
#include <stdio.h>

#include "pdfMax.h"

static int test(double pdfList[][2], int n, double significance)
{
        double mu, sigma, odds[n];
        int nrSegments = pdfMax(pdfList, n, significance, &mu, &sigma, odds);

        double refMu, refSigma, refOdds[n];
        pdfMax(pdfList, n, 1.0e-12, &refMu, &refSigma, refOdds);

        double sumOdds = 0.0;
        for (int i=0; i<n; i++) {
                printf("pdfList[%d] = { %17.17f, %17.17f }, odds[%d] = %17.17f +/- %17.17f\n",
                        i, pdfList[i][0], pdfList[i][1], i, odds[i], fabs(odds[i] - refOdds[i]));
                sumOdds += odds[i];
        }
        printf("signifiance = %17.17f     %17.17f\n", significance, significance);
        printf("mu          = %17.17f +/- %17.17f\n", mu, fabs(mu - refMu));
        printf("sigma       = %17.17f +/- %17.17f\n", sigma, fabs(sigma - refSigma));
        printf("sumOdds     = %17.17f\n", sumOdds);
        printf("nrSegments  = %d\n", nrSegments);
        printf("\n");

        return nrSegments;
}

int main(void)
{
        #define Test(...) do{\
                double _pdfList[][2] = {__VA_ARGS__};\
                int _n = sizeof(_pdfList) / sizeof(_pdfList[0]);\
                for (int _i=0; _i<=10; _i++) {\
                        int _s = test(_pdfList, _n, pow(10.0, -_i));\
                        if (_s == 0) break;\
                }\
        }while(0)

        Test({0.123, 0.456});

        Test({1.0,  1.0},
             {0.75, 0.25});

        Test({0.6412027167691333, 0.03163264836021567},
             {0.6281166488299745, 0.013541425967055945},
             {0.6072673296242264, 0.03225156507575445});

        return 0;
}


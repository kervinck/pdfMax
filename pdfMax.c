
/*
 *  pdfMax.c -- Fast maximum of multiple Gaussians
 *  Copyright (C) 2015 Marcel van Kervinck
 *  All rights reserved
 *
 *  TODO: BSD 3-clause licence?
 */

// Standard includes
#include <assert.h>
#include <float.h>
#include <math.h>
#include <string.h>

// Own include
#include "pdfMax.h"

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      PDF / PDF1                                                      |
 +----------------------------------------------------------------------*/

#if 0
/*
 *  Probability density function of the normal distribution with mu = 0
 */
static double PDF(double x, double sigma)
{
        double s2 = sigma * sigma;
        return exp(-0.5 * x * x / s2) / sqrt(2.0 * M_PI * s2);
}
#endif

/*
 *  Probability density function of the standard normal distribution
 */
static double PDF1(double x)
{
        static const double c = -0.91893853320467267; // -log(sqrt(2.0 * M_PI))
        return exp(c - 0.5 * x * x);
}

/*----------------------------------------------------------------------+
 |      CDF / CDF1                                                      |
 +----------------------------------------------------------------------*/

/*
 *  Cumulative normal distribution for mu = 0
 */
static double CDF(double x, double sigma)
{
        return 0.5 * erfc(-x / (sigma * M_SQRT2));
}

/*
 *  Cumulative standard normal distribution
 */
static double CDF1(double x)
{
        return 0.5 * erfc(-x / M_SQRT2);
}

/*----------------------------------------------------------------------+
 |      pdfMax                                                          |
 +----------------------------------------------------------------------*/

/*
 *  Helper to perform numeric integration for pdfMax
 */
static int calcMomentsAndOdds(
        double pdfList[][2], int n, double significance,
        double a, double Ca[], double b, double Cb[],
        double moments[3], double odds[])
{
        assert(a < b);

        double m = 0.5 * (a + b);
        double Cm[n];
        double sumErrors = 0.0;

        for (int i=0; i<n; i++) {
                Cm[i] = CDF(m - pdfList[i][0], pdfList[i][1]);

                // Use sum as good estimate of "1 - prod(1 - e for e errors)"
                sumErrors += fabs(0.5 * (Ca[i] + Cb[i]) - Cm[i]);
        }

        if (sumErrors < significance) {
                double S[n];
                double productS = 1.0;
                for (int i=0; i<n; i++) {
                        S[i] = (Ca[i] + 4.0 * Cm[i] + Cb[i]) / 6.0; // Simpson's rule
                        productS *= S[i];
                }

                double Ex0 = 0.0;
                for (int i=0; i<n; i++) {
                        /*
                         *  Calculate
                         *  p = PDF(m - mu_i, sigma_i)
                         *    * prod(CDF(m - mu_j, sigma_j) for j != i)
                         *    * (b - a)
                         */
                        double p = (Cb[i] - Ca[i]) * productS / S[i];
                        odds[i] += p;
                        Ex0 += p;
                }
                moments[0] += Ex0;
                moments[1] += Ex0 * m;
                moments[2] += Ex0 * m * m;
                return 1;
        } else {
                // Recursion on the split interval
                return calcMomentsAndOdds(pdfList, n, significance, a, Ca, m, Cm, moments, odds)
                     + calcMomentsAndOdds(pdfList, n, significance, m, Cm, b, Cb, moments, odds);
        }
}

/*
 *  Calculate the PDF of the max and the odds that each item is best.
 *  Give an exact result for n<=2, and use numerical integration for within the
 *  given significance for n>=3.
 *  Return the number of segments used (0 means exact result). This can be
 *  used to learn about the runtime.
 */
int pdfMax(double pdfList[][2], int n, double significance,
           double *mu, double *sigma, double odds[])
{
        assert(n > 0);
        assert(significance > 0.0);

        if (n == 1) {
                /*
                 *  The most basic case
                 */

                *mu    = pdfList[0][0];
                *sigma = pdfList[0][1];
                odds[0] = 1.0;

                return 0;
        }

        if (n == 2) {
                /*
                 *  Clark's two-value exact formula, see also
                 *  http://www.eecs.berkeley.edu/~alanmi/research/timing/papers/clark1961.pdf
                 */

                double mu1    = pdfList[0][0];
                double sigma1 = pdfList[0][1];
                double mu2    = pdfList[1][0];
                double sigma2 = pdfList[1][1];

                double theta = sqrt(sigma1 * sigma1 + sigma2 * sigma2);
                double alpha = (mu1 - mu2) / theta;

                double Ex1 = mu1 * CDF1( alpha)
                           + mu2 * CDF1(-alpha)
                           + theta * PDF1(alpha);

                double Ex2 = (sigma1 * sigma1 + mu1 * mu1) * CDF1( alpha)
                           + (sigma2 * sigma2 + mu2 * mu2) * CDF1(-alpha)
                           + (mu1 + mu2) * theta * PDF1(alpha);

                *mu = Ex1;
                *sigma = sqrt(Ex2 - Ex1 * Ex1);
                odds[0] = CDF(mu1 - mu2, theta); // First odds is simply P(A-B) > 0
                odds[1] = 1.0 - odds[0];

                return 0;
        }

        /*
         *  Estimate required relative size of integration range for given significance
         */

        double k = 2.0 + (log(n) - log(significance)) / 5.0;
        if (k < 1.0) k = 1.0;
        assert(n * CDF1(-k) < significance);

        /*
         *  Determine the boundaries of the integration intervals
         */

        double a = -DBL_MAX, b = -DBL_MAX, m = -DBL_MAX;

        for (int i=0; i<n; i++) {
                double kSigma = k * pdfList[i][1];

                // Lower bound
                double ai = pdfList[i][0] - kSigma;
                if (ai > a) a = ai;

                // Upper bound
                double bi = pdfList[i][0] + kSigma;
                if (bi > b) b = bi;

                // Rough center of mass
                double mi = pdfList[i][0];
                if (mi > m) m = mi;
        }

        /*
         *  Shift pdfList by -m to reduce the error of moments[2]
         */

        double shifted[n][2];

        for (int i=0; i<n; i++) {
                shifted[i][0] = pdfList[i][0] - m;
                shifted[i][1] = pdfList[i][1];
        }

        /*
         *  CDFs at the interval ends
         */

        double Ca[n], Cb[n], Cm[n];

        for (int i=0; i<n; i++) {
                Ca[i] = CDF(a - pdfList[i][0], pdfList[i][1]);
                Cb[i] = CDF(b - pdfList[i][0], pdfList[i][1]);
                Cm[i] = CDF(m - pdfList[i][0], pdfList[i][1]);
        }

        /*
         *  Reset the result accumulators
         */

        double moments[3] = {0.0, };
        memset(odds, 0, n * sizeof(odds[0]));

        /*
         *  Calculate the integral numerically
         *  Pre-splitting at max(mu) avoids some of the trivial early termination
         *  problems due to the point symmetry of the CDF function around 0
         */

        int nrSegments =
                calcMomentsAndOdds(shifted, n, significance, a - m, Ca,   0.0, Cm, moments, odds) +
                calcMomentsAndOdds(shifted, n, significance,   0.0, Cm, b - m, Cb, moments, odds);

        /*
         *  Convert the outputs and return
         */

        *mu = moments[1] / moments[0] + m;
        *sigma = sqrt(moments[2] - moments[1] * moments[1]);

        for (int i=0; i<n; i++) {
                odds[i] /= moments[0];
        }

        return nrSegments;
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/


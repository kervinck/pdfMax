pdfMax
======

Python module interface:
------------------------

```
NAME
    pdfmax - Fast maximum of multiple Gaussians

FUNCTIONS
    pdfMax(...)
        pdfMax(pdfList, significance) -> mu, sigma, odds
        
        Maximum of several Gaussians
        
        Input arguments:
               pdfList         List of n (n > 0) Gaussians, given as (mu, sigma) tuples
               sigificance     Desired significance for the result when n > 2, meaning
                               that either the max or the mean+3sigma of the absolute
                               error stays within this bound, whichever is lowest.
        
        Output arguments:
               mu, sigma       Distribution of the maximum (estimated for n > 2)
               odds            List of odds that each input variable is the maximum
        
        Notes:
             - Although the max distribution is generally not exactly normal, the
               returned Gaussian preserves its first two moments.
             - An exact result is returned for n <= 2 and `significance' ignored.
             - For n > 2, mu, sigma and odds[] are approximated numerically within
               the given signicance. Below 5e-12 this calculation may become unstable.
```

Example:
--------

```
>>> import pdfmax
>>> pdfmax.pdfMax([ (6.41, 0.316), (6.28, 0.135), (6.07, 0.323) ], 1e-4)

Gives:

(6.518802854048699, 0.22606903184250005, [0.5833763495772353, 0.2743540663894621, 0.14226958403330284])
```



from math import sqrt

class Stats:
        def __init__(self):
                self.z = 0.0 # location parameter
                self.sum = 0.0
                self.sumSq = 0.0
                self.n = 0
                self.min = float('inf')
                self.max = float('-inf')

        def add(self, x):
                if self.n < 2:
                        if self.n == 0:
                                self.z = x
                        else:
                                # get closer to center of distribution
                                self.z = 0.5 * (self.z + x)
                                self.sumSq = 0.5 * x * x
                else:
                        xz = x - self.z # shift around location
                        self.sum += xz
                        self.sumSq += xz * xz
                self.n += 1
                self.min = min(self.min, x)
                self.max = max(self.max, x)

        def calcMean(self):
                mean, sigma, error = self.z, 0.0, 0.0
                if self.n > 0:
                        mean = self.sum / self.n + self.z
                        variance = (self.sumSq - self.sum * self.sum / self.n) / self.n
                        sigma = sqrt(variance)
                        error = sigma / sqrt(self.n)
                return mean, sigma, error

        def __str__(self):
                mean, sigma, error = self.calcMean()
                return '%.15f +/- %.15f [%d]' % (mean, error, self.n)


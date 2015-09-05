#!/usr/bin/env python

# Python imports
import json
import sys

# Own imports
import pdfmax
import stats

significance = float(sys.argv[1])

nStats = {}

for line in sys.stdin:
        testcase = json.loads(line)

        pdfList = [(item['mu'], item['sigma']) for item in testcase['input']]

        refMu    = testcase['expect']['mu']
        refSigma = testcase['expect']['sigma']
        refOdds  = testcase['expect']['odds']

        n = len(pdfList)

        mu, sigma, odds = pdfmax.pdfMax(pdfList, significance)

        muError = abs(mu - refMu)
        sigmaError = abs(sigma - refSigma)
        oddsErrors = [abs(o - oref) for o, oref in zip(odds, refOdds)]

        if n not in nStats:
                nStats[n] = [stats.Stats(), stats.Stats(), stats.Stats()]

        nStats[n][0].add(muError)
        nStats[n][1].add(sigmaError)
        for oddsError in oddsErrors:
                nStats[n][2].add(oddsError)

maxList = [ float('-inf') ] * 9
count1, count2 = 0, 0

print '  n',
for label in ['Mu', 'Sigma', 'Odds']:
        print '',
        print '%-14.14s' % ('%s err avg' % label),
        print '%-14.14s' % ('%s err m3s' % label),
        print '%-14.14s' % ('%s err max' % label),
print 'counts'

def printDiv():
        print '---',
        for i in range(3):
                print ' %s %s %s' % ('-' * 14, '-' * 14, '-' * 14),
        print '-' * 12

printDiv()

for n, s in sorted(nStats.items()):
        print ' %2d' % n,
        col = 0
        for t in s:
                meanError, sigmaError, errorError = t.calcMean()
                maxError = t.max
                m3s = meanError + 3.0 * sigmaError

                print ' %.12f %.12f %.12f' % (meanError, m3s, maxError),

                maxList[col  ] = max(maxList[col  ], meanError)
                maxList[col+1] = max(maxList[col+1], m3s)
                maxList[col+2] = max(maxList[col+2], maxError)
                col += 3

        print s[0].n, s[2].n
        count1 += s[0].n
        count2 += s[2].n

printDiv()

print 'max',
for i in range(3):
        print ' %.12f %.12f %.12f' % (maxList[3*i], maxList[3*i+1], maxList[3*i+2]),
print count1, count2

print '   ',
for i in range(3):
        print ' %.12f %.12f %.12f' % (significance, significance, significance),
print

avgError = max([maxList[3*i  ] for i in range(3)])
m3sError = max([maxList[3*i+1] for i in range(3)])
maxError = max([maxList[3*i+2] for i in range(3)])

testResult = min(m3sError, maxError)

print 'Needed significance: %.2e' % significance
print 'Result significance: %.2e' % testResult

ok = testResult <= significance
print 'Test result: %s' % ('OK' if ok else 'FAILED')

sys.exit(0 if ok else 1)


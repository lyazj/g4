#!/usr/bin/env python3

import re
import builtins
import mplhep as hep
import uproot
import numpy as np
import awkward as ak
import matplotlib.pyplot as plt

logfile = open(re.sub(r'\.py$', '.log', __file__), 'w')
def print(*args, **kwargs): builtins.print(*args, **kwargs); builtins.print(*args, **{**kwargs, 'file': logfile})

tree = uproot.open('USphere-8.500000.root:tree')
gt, gen = tree.arrays(('NeutronGlobalTime', 'NeutronGeneration'), how=tuple)
n = len(gt)
print('%d events loaded' % n)

plt.style.use(hep.styles.CMS)

plt.figure(figsize=(10, 8))
number, bins, _ = plt.hist(ak.flatten(gt), np.linspace(0, 1250, 51), weights=np.ones_like(ak.flatten(gt)) / n, histtype='step', label='simulation')
time = (bins[:-1] + bins[1:]) / 2.0

range_to_fit = np.arange(400 * len(time) // bins[-1], 1000 * len(time) // bins[-1], dtype='int')
number_to_fit = number[range_to_fit]
time_to_fit = time[range_to_fit]
plt.plot([bins[range_to_fit[ 0]    ]] * 2, [number_to_fit.min() / 1.2, number_to_fit.max() * 1.2], 'k--')
plt.plot([bins[range_to_fit[-1] + 1]] * 2, [number_to_fit.min() / 1.2, number_to_fit.max() * 1.2], 'k--')
plt.plot([], [], 'k--', label='fitting boundary')

log_number_to_fit = np.log(number_to_fit)
k, b = np.polyfit(time_to_fit, log_number_to_fit, 1)
cov = np.cov(time_to_fit, log_number_to_fit)
r = cov[0,1] / np.sqrt(cov[0,0] * cov[1,1])
number_fit = np.exp(k * time_to_fit + b)
plt.plot(time_to_fit, number_fit, label='fit (r=%.6f)' % r)
sigma_k = k * np.sqrt((1.0 / (r*r) - 1.0) / (len(time_to_fit) - 2))
print('k = %.6f (%.6f)' % (k, sigma_k))
print('exp(k) = %.6f (%.6f)' % (np.exp(k), np.exp(k) * sigma_k))

plt.xlabel('Global Time [ns]')
plt.ylabel('Neutron Number')
plt.yscale('log')
plt.ylim(number_to_fit.min() / 10.0, number_to_fit.max() * 1.2)
plt.legend()
plt.tight_layout()
plt.savefig('NeutronGlobalTime-8.5.pdf')
plt.savefig('NeutronGlobalTime-8.5.png')
plt.close()

# ------------------------------------------------------------

plt.figure(figsize=(10, 8))
number, bins, _ = plt.hist(ak.flatten(gen), np.linspace(0, 300, 51), weights=np.ones_like(ak.flatten(gen)) / n, histtype='step', label='simulation')
generation = (bins[:-1] + bins[1:]) / 2.0

range_to_fit = np.arange(54 * len(time) // bins[-1], 156 * len(time) // bins[-1], dtype='int')
number_to_fit = number[range_to_fit]
generation_to_fit = generation[range_to_fit]
plt.plot([bins[range_to_fit[ 0]    ]] * 2, [number_to_fit.min() / 1.2, number_to_fit.max() * 1.2], 'k--')
plt.plot([bins[range_to_fit[-1] + 1]] * 2, [number_to_fit.min() / 1.2, number_to_fit.max() * 1.2], 'k--')
plt.plot([], [], 'k--', label='fitting boundary')

log_number_to_fit = np.log(number_to_fit)
k, b = np.polyfit(generation_to_fit, log_number_to_fit, 1)
cov = np.cov(generation_to_fit, log_number_to_fit)
r = cov[0,1] / np.sqrt(cov[0,0] * cov[1,1])
number_fit = np.exp(k * generation_to_fit + b)
plt.plot(generation_to_fit, number_fit, label='fit (r=%.6f)' % r)
sigma_k = k * np.sqrt((1.0 / (r*r) - 1.0) / (len(generation_to_fit) - 2))
print('k = %.6f (%.6f)' % (k, sigma_k))
print('exp(k) = %.6f (%.6f)' % (np.exp(k), np.exp(k) * sigma_k))

plt.xlabel('Neutron Generation')
plt.ylabel('Neutron Number')
plt.yscale('log')
plt.legend()
plt.tight_layout()
plt.savefig('NeutronGeneration-8.5.pdf')
plt.savefig('NeutronGeneration-8.5.png')
plt.close()

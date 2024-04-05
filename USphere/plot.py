#!/usr/bin/env python3

import mplhep as hep
import uproot
import numpy as np
import awkward as ak
import matplotlib.pyplot as plt

tree = uproot.open('USphere.root:tree')
gt, gen = tree.arrays(('NeutronGlobalTime', 'NeutronGeneration'), how=tuple)
n = len(gt)
print('%d events loaded' % n)

plt.style.use(hep.styles.CMS)

plt.figure(figsize=(10, 8))
plt.hist(ak.flatten(gt), np.linspace(0, 1200, 51), weights=np.ones_like(ak.flatten(gt)) / n, histtype='step')
plt.xlabel('Global Time [ns]')
plt.ylabel('Neutron Number')
plt.yscale('log')
plt.grid()
plt.tight_layout()
plt.savefig('NeutronGlobalTime.pdf')
plt.close()

plt.figure(figsize=(10, 8))
plt.hist(ak.flatten(gen), np.linspace(0, 300, 51), weights=np.ones_like(ak.flatten(gen)) / n, histtype='step')
plt.xlabel('Neutron Generation')
plt.ylabel('Neutron Number')
plt.yscale('log')
plt.grid()
plt.tight_layout()
plt.savefig('NeutronGeneration.pdf')
plt.close()

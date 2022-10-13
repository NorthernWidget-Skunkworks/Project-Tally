from matplotlib import pyplot as plt
import pandas as pd
from scipy.optimize import curve_fit
import numpy as np

df = pd.read_csv('WindTunnelCalib.csv')

u = df['Measured Pitot']
rps = df['Rotations per second']

def linfit(x, a):
    return a*x
    
def linfit_yint(x, a, b):
    return a*x + b

def calc_r_squared(fitfcn, x, y, *popt):
    residuals = y - fitfcn(x, *popt)
    ss_res = np.sum(residuals**2)
    ss_tot = np.sum( (y - np.mean(y))**2 )
    _r_squared = 1 - (ss_res / ss_tot)
    return _r_squared
    
popt, pcov = curve_fit(linfit_yint, rps, u)

print("Fit parameters:", popt)
print("R squared =", '%.3f' %calc_r_squared(linfit_yint, u, rps, *popt))

_x = np.array([0,12])
_y = linfit_yint(_x, *popt)
plt.plot(_x, _y, 'k-', linewidth=2)
plt.plot(rps, u, 'o', color='.5')

plt.show()


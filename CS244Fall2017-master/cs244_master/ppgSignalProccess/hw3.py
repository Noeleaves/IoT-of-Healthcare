import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as sig
from scipy.interpolate import interp1d


# BandPass Filter
def BandPass(S, SmpRate, lowcut, highcut):
    N = S.shape[0]
    Sfft = np.fft.rfft(S) / N
    freqs = np.linspace(0, SmpRate / 2, N / 2 + 1)
    Sfft[freqs < lowcut] = 0
    Sfft[freqs > highcut] = 0

    SBandPass = np.fft.irfft(Sfft)
    return SBandPass

def heart_rate(time, filteredData) -> (np.array, np.array):
	# 50 samples per second, a peak every 50 samples = 1 hz
	peakTime = time[sig.argrelmax(filteredData, order=25)]
	# sig.argrelmax(filteredData,order = 25)
	#indexes value of peak time, stored in a tuple(numpy array, )

	# calculate the heartrate given time between two peaks
	# = 60 (seconds / minute) / (seconds / beat) = (beats / minute)
	return np.column_stack((peakTime[:-1], (60 / (peakTime[1:] - peakTime[:-1]))))

def respiration(time, filteredData) -> (np.array, np.array):
    peakTime = time[sig.argrelmax(filteredData, order=25)]
    return np.column_stack((peakTime[:-1], (60 / (peakTime[1:] - peakTime[:-1]))))

def spo2(time, filteredIR, filteredRED):
    peakIR = filteredIR[sig.argrelmax(filteredIR, order=25)]
    peakTime = time[sig.argrelmax(filteredIR, order=25)]
    minPeakTime = time[sig.argrelmin(filteredIR, order=25)]
    interpldPoint = interp1d(minPeakTime, filteredIR[sig.argrelmin(filteredIR, order=25)])

    DCIR = interpldPoint(peakTime[1:])
    ACIR = peakIR[1:]-DCIR

    peakRED = filteredRED[sig.argrelmax(filteredRED, order=25)]
    peakTimeRED = time[sig.argrelmax(filteredRED, order=25)]
    minPeakTimeRED = time[sig.argrelmin(filteredRED, order=25)]
    interpldPoint = interp1d(minPeakTimeRED, filteredRED[sig.argrelmin(filteredRED, order=25)])

    DCRED = interpldPoint(peakTimeRED[2:-2])
    ACRED = peakRED[2:-2] - DCRED

    R = (ACRED * DCIR) / (ACIR * DCRED)
    spo2 = (R*R*(-45.06))+(R*30.354) + 94.845
    
    return np.column_stack((peakTime[:-1],spo2))

def expand_data(time, calculated_rate):
	out = np.column_stack((time, np.zeros(time.shape)))
	for i in range(calculated_rate.shape[0]-1):
		locations = np.where(
				np.all([(calculated_rate[i, 0] <= time),(time < calculated_rate[i+1,0])], axis=0))
		out[locations, 1] = calculated_rate[i, 1]
	out[np.where(time < calculated_rate[0,0]), 1] = calculated_rate[0, 1]
	out[np.where(time >= calculated_rate[-1,0]), 1] = calculated_rate[-1, 1]
	return out

def write_file(data, fname = 'foo.txt'):
	with open(fname,'w') as f:
		for row in data:
			f.write(', '.join(map(str, row)))
			f.write('\n')

def plot_signal(time, IR, RED, newTime, filteredIR):
    plt.figure()
    plt.clf()
    plt.plot(time, RED, label='RED')
    plt.plot(time, IR, label='IR')
    plt.plot(newTime, filteredIR, label='filteredIR')

    plt.xlabel('Time (Seconds)')
    plt.grid(True)
    plt.axis('tight')
    plt.legend(loc='upper left')
    plt.show(False)

if __name__ == '__main__':
    data = np.genfromtxt("takashin_Homework_sample.csv",
                         delimiter=',', skip_header=1, usecols=(0, 1, 2))
    RED = data[:, 1]
    IR = data[:, 2]
    time = data[:, 0]

    SmpRate=50
    newTime = np.linspace(0, 2 * (time.shape[0] - 1) / SmpRate, 2 * (time.shape[0] - 1))

    filteredIR = BandPass(IR, 50, 1, 2)
    calculated_HR = heart_rate(time, filteredIR)
    #print(calculated_HR)
    write_file(expand_data(time, calculated_HR), 'heart.csv')

    filteredIR = BandPass(IR, 50, 0.16, 0.33)
    calculated_RS = respiration(time,filteredIR)
    write_file(expand_data(time, calculated_RS), 'resp.csv')

    filteredIR = BandPass(IR, 50, 1, 2)
    filteredRED = BandPass(RED, 50, 1, 2)
    calculated_SP = spo2(time,IR,RED)
    #print(calculated_SP)
    write_file(expand_data(time, calculated_SP), 'spo2.csv')

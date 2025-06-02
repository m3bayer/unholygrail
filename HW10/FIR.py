import csv
import matplotlib.pyplot as plt # for plotting
import numpy as np # for sine function

files = ['sigA', 'sigB', 'sigC', 'sigD']
filter_name = 'Blackman'
cutoff_freq = 100     # Hz (just for title display, not used in h)
bandwidth = 2700      # Hz (just for title display, not used in h)
h = np.array([
    -0.000000000000000002,
    0.001457826841675081,
    0.006549295164930914,
    0.016882484264206098,
    0.033750668331437424,
    0.056803882125556170,
    0.083290418346488163,
    0.108358278141449257,
    0.126410562133999788,
    0.132993169300514108,
    0.126410562133999788,
    0.108358278141449299,
    0.083290418346488204,
    0.056803882125556170,
    0.033750668331437444,
    0.016882484264206136,
    0.006549295164930921,
    0.001457826841675083,
    -0.000000000000000002,
])

X = len(h)

for file in files:

    t = [] # column 0
    data1 = [] # column 1

    with open(file + '.csv') as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column time
            data1.append(float(row[1])) # second column values


    Fs = len(t) / (t[-1] - t[0]) # sample rate
    Ts = 1.0/Fs; # sampling interval
    print(Fs)
    ts = np.arange(0,t[-1],Ts) # time vector
    y = data1 # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]


    #averaging
    padded_data = np.concatenate((np.zeros(X-1), data1))
    filtered = np.convolve(padded_data, h, mode='valid')


    Y_filtered = np.fft.fft(filtered) / n
    Y_filtered = Y_filtered[:n // 2]

    # Plot Time Domain
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 6))
    fig.suptitle(f'{file}: FIR {filter_name} Filter (cutoff={cutoff_freq}Hz, BW={bandwidth}Hz, taps={X})')

    # Time domain
    ax1.plot(t, data1, 'k-', label='Original')
    ax1.plot(t, filtered, 'r-', label='Filtered')
    ax1.set_xlabel('Time (s)')
    ax1.set_ylabel('Amplitude')
    ax1.set_title('Time Domain')
    ax1.legend()

    # Frequency domain
    ax2.loglog(frq, abs(Y), 'k-', label='Original FFT')
    ax2.loglog(frq, abs(Y_filtered), 'r-', label='Filtered FFT')
    ax2.set_xlabel('Frequency (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    ax2.set_title('Frequency Domain')
    ax2.legend()

    plt.tight_layout()
    plt.savefig(f'{file}_FIR.png')
    plt.show()

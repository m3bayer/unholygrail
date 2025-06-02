import csv
import matplotlib.pyplot as plt # for plotting
import numpy as np # for sine function

files = ['sigA', 'sigB', 'sigC', 'sigD']

for file in files:

    t = [] # column 0
    data1 = [] # column 1
    X = 1000 #average last X datapoints

    with open(file + '.csv') as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column time
            data1.append(float(row[1])) # second column values


    Fs = len(t) / (t[-1] - t[0]) # sample rate
    Ts = 1.0/Fs; # sampling interval
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
    filtered = []
    for i in range(len(data1)):
        if i < X:
            avg = 0  # use available points
        else:
            avg = np.mean(data1[i-X+1:i+1])
        filtered.append(avg)
    filtered = np.array(filtered)

    Y_filtered = np.fft.fft(filtered) / n
    Y_filtered = Y_filtered[:n // 2]

    # Plot Time Domain
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 6))
    fig.suptitle(f'{file}: Moving Average (X = {X})')

    ax1.plot(t, data1, 'k-', label='Original')
    ax1.plot(t, filtered, 'r-', label='Filtered')
    ax1.set_xlabel('Time [s]')
    ax1.set_ylabel('Amplitude')
    ax1.set_title('Time Domain')
    ax1.legend()

    # Plot Frequency Domain
    ax2.loglog(frq, abs(Y), 'k-', label='Original FFT')
    ax2.loglog(frq, abs(Y_filtered), 'r-', label='Filtered FFT')
    ax2.set_xlabel('Frequency [Hz]')
    ax2.set_ylabel('|Y(freq)|')
    ax2.set_title('Frequency Domain')
    ax2.legend()

    plt.tight_layout()
    plt.savefig(f'{file}_X{X}_movingAve_comparison.png')
    plt.show()
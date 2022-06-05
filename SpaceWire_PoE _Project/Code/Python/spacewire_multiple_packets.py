import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter, MaxNLocator, MultipleLocator, FormatStrFormatter, AutoMinorLocator
import numpy as np
from random import seed
import random

swpoeFrameSize = 1500          #sie of sw poe frame headers, excluding headerbyte and EOP
ethHeadersSize = 18            #size of eth headers
ethFrameSizeMin = 64         #minimum eth frame

eth_data_array = np.array([None]*1500)
sw_poe_data_array = np.array([None]*28)

x_data = np.array([None]*28)
frameBufferSize = 1500
efficiency = []
efficiency2 = []
efficiency3 = []
efficiency4 = []
efficiency5 = []
efficiency6 = []
packetsSent = []
packetsSent2 = []
packetsSent3 = []
packetsSent4 = []
packetsSent5 = []
packetsSent6 = []
loops = 0
n = 50

index = 0
byte = 1
packets = 1


seed(None)     #seed rnadom number generator, randint("range") to generate int value

#generate data for efficiency[], packets sizes 1-1498 for over 46 bytes
while loops < n:
    frameBufferSize = random.randint(47, 1500)
    #print("frame length", frameBufferSize)
    currentBufferSize = 0
    totalData = 0
    attempts = 0
    packetNum = 0

    #generate data for SpaceWire multiple Packets in fixed 1500 frame length, with random length
    while (currentBufferSize < frameBufferSize):
        dataLength = random.randint(1, 1498)
        if ((dataLength+2+currentBufferSize) > frameBufferSize):
            #print("Too large data for frame!")
            attempts = attempts + 1
            if(attempts > 100):
                totalData = totalData + ((frameBufferSize-currentBufferSize) - 2)
                currentBufferSize = currentBufferSize + (frameBufferSize-currentBufferSize)
                packetNum = packetNum + 1
                break
            else: continue
        #print("data length:", dataLength)
        packetLength = dataLength + 2        #data +2 for headerbyte and EOP
        totalData = totalData + dataLength
        currentBufferSize = currentBufferSize + packetLength
        packetNum = packetNum + 1
    
    #print("total data:", totalData)
    #print("current buffer size", currentBufferSize)
    loopEfficiency = (totalData/(frameBufferSize+ethHeadersSize)*100)       #total data divided by frame + ethheaders (18)
    efficiency.insert(loops, loopEfficiency)
    packetsSent.insert(loops, packetNum)
    #print("efficiency", efficiency[loops])
    loops = loops + 1

loops = 0

    #generate data for efficiency5[], packets sizes 47-80 for over 46 bytes (area of interest!)
while loops < n:
    frameBufferSize = random.randint(46, 70)
    #print("frame length", frameBufferSize)
    currentBufferSize = 0
    totalData = 0
    attempts = 0
    packetNum = 0

    #generate data for SpaceWire multiple Packets in fixed 1500 frame length, with random length
    while (currentBufferSize < frameBufferSize):
        dataLength = random.randint(1, 68)
        if ((dataLength+2+currentBufferSize) > frameBufferSize):
            #print("Too large data for frame!")
            attempts = attempts + 1
            if(attempts > 100):
                totalData = totalData + ((frameBufferSize-currentBufferSize) - 2)
                currentBufferSize = currentBufferSize + (frameBufferSize-currentBufferSize)
                packetNum = packetNum + 1
                break
            else: continue
        #print("data length:", dataLength)
        packetLength = dataLength + 2        #data +2 for headerbyte and EOP
        totalData = totalData + dataLength
        currentBufferSize = currentBufferSize + packetLength
        packetNum = packetNum + 1
    
    #print("total data:", totalData)
    #print("current buffer size", currentBufferSize)
    loopEfficiency = (totalData/(frameBufferSize+ethHeadersSize)*100)       #total data divided by frame + ethheaders (18)
    efficiency5.insert(loops, loopEfficiency)
    packetsSent5.insert(loops, packetNum)
    #print("efficiency", efficiency[loops])
    loops = loops + 1

loops = 0

#generate data for efficiency2[], for data under 46 bytes
while loops < n:
    frameBufferSize = random.randint(3, 46)
    #print("frame length", frameBufferSize)
    currentBufferSize = 0
    totalData = 0
    attempts = 0
    packetNum = 0

    #generate data for SpaceWire multiple Packets in fixed 1500 frame length, with random length
    while (currentBufferSize < frameBufferSize):
        dataLength = random.randint(1, 44)
        if ((dataLength+2+currentBufferSize) > frameBufferSize):
            #print("Too large data for frame!")
            attempts = attempts + 1
            if(attempts > 100):
                totalData = totalData + ((frameBufferSize-currentBufferSize) - 2)
                currentBufferSize = currentBufferSize + (frameBufferSize-currentBufferSize)
                packetNum = packetNum + 1
                break
            else: continue
        #print("data length:", dataLength)
        packetLength = dataLength + 2        #data +2 for headerbyte and EOP
        totalData = totalData + dataLength
        currentBufferSize = currentBufferSize + packetLength
        packetNum = packetNum + 1
    
    #print("total data:", totalData)
    #print("current buffer size", currentBufferSize)

    loopEfficiency = (totalData/ethFrameSizeMin)*100            #total data from frame divided by 64 eth frame min
    efficiency2.insert(loops, loopEfficiency)
    packetsSent2.insert(loops, packetNum)
    #print("efficiency", efficiency2[loops])
    loops = loops + 1

loops =  0

#generate data for efficiency3[], packets sizes 1-200 for over 46 bytes
while loops < n:
    frameBufferSize = random.randint(47, 1500)
    #print("frame length", frameBufferSize)
    currentBufferSize = 0
    totalData = 0
    attempts = 0
    packetNum = 0

    #generate data for SpaceWire multiple Packets in fixed 1500 frame length, with random length
    while (currentBufferSize < frameBufferSize):
        dataLength = random.randint(1, 250)
        if ((dataLength+2+currentBufferSize) > frameBufferSize):
            #print("Too large data for frame!")
            attempts = attempts + 1
            if(attempts > 100):
                totalData = totalData + ((frameBufferSize-currentBufferSize) - 2)
                currentBufferSize = currentBufferSize + (frameBufferSize-currentBufferSize)
                packetNum = packetNum + 1
                break
            else: continue
        #print("data length:", dataLength)
        packetLength = dataLength + 2        #data +2 for headerbyte and EOP
        totalData = totalData + dataLength
        currentBufferSize = currentBufferSize + packetLength
        packetNum = packetNum + 1
    
    #print("total data:", totalData)
    #print("current buffer size", currentBufferSize)
    loopEfficiency = (totalData/(frameBufferSize+ethHeadersSize)*100)       #total data divided by frame + ethheaders (18)
    efficiency3.insert(loops, loopEfficiency)
    packetsSent3.insert(loops, packetNum)
    #print("efficiency", efficiency[loops])
    loops = loops + 1

loops = 0

while loops < n:
    frameBufferSize = random.randint(3, 46)
    #print("frame length", frameBufferSize)
    currentBufferSize = 0
    totalData = 0
    attempts = 0
    packetNum = 0

    #generate data for SpaceWire multiple Packets in fixed 1500 frame length, with random length
    while (currentBufferSize < frameBufferSize):
        dataLength = random.randint(1, 44)
        if ((dataLength+2+currentBufferSize) > frameBufferSize):
            #print("Too large data for frame!")
            attempts = attempts + 1
            if(attempts > 100):
                totalData = totalData + ((frameBufferSize-currentBufferSize) - 2)
                currentBufferSize = currentBufferSize + (frameBufferSize-currentBufferSize)
                packetNum = packetNum + 1
                break
            else: continue
        #print("data length:", dataLength)
        packetLength = dataLength + 2        #data +2 for headerbyte and EOP
        totalData = totalData + dataLength
        currentBufferSize = currentBufferSize + packetLength
        packetNum = packetNum + 1
    
    #print("total data:", totalData)
    #print("current buffer size", currentBufferSize)

    loopEfficiency = (totalData/ethFrameSizeMin)*100            #total data from frame divided by 64 eth frame min
    efficiency4.insert(loops, loopEfficiency)
    packetsSent4.insert(loops, packetNum)
    #print("efficiency", efficiency2[loops])
    loops = loops + 1

while loops < n:
    frameBufferSize = random.randint(46, 70)
    #print("frame length", frameBufferSize)
    currentBufferSize = 0
    totalData = 0
    attempts = 0
    packetNum = 0

    #generate data for SpaceWire multiple Packets in fixed 1500 frame length, with random length
    while (currentBufferSize < frameBufferSize):
        dataLength = random.randint(1, 68)
        if ((dataLength+2+currentBufferSize) > frameBufferSize):
            #print("Too large data for frame!")
            attempts = attempts + 1
            if(attempts > 100):
                totalData = totalData + ((frameBufferSize-currentBufferSize) - 2)
                currentBufferSize = currentBufferSize + (frameBufferSize-currentBufferSize)
                packetNum = packetNum + 1
                break
            else: continue
        #print("data length:", dataLength)
        packetLength = dataLength + 2        #data +2 for headerbyte and EOP
        totalData = totalData + dataLength
        currentBufferSize = currentBufferSize + packetLength
        packetNum = packetNum + 1
    
    #print("total data:", totalData)
    #print("current buffer size", currentBufferSize)
    loopEfficiency = (totalData/(frameBufferSize+ethHeadersSize)*100)       #total data divided by frame + ethheaders (18)
    efficiency6.insert(loops, loopEfficiency)
    packetsSent6.insert(loops, packetNum)
    #print("efficiency", efficiency[loops])
    loops = loops + 1

'''
#generate data for ethernet
while byte < 1500:
    if(byte < 47):
        efficiency = (byte/ethFrameSizeMin)*100

    else:
        efficiency = (byte/(byte+ethFrameSize))*100
    
    eth_data_array[index] = efficiency
    x_data[index] = byte
    byte = byte + 1
    index = index + 1

#setup plot
plt.figure(1)
plt.title("Multiple SpaceWire Packets Frame vs Payload Efficiency")
plt.xlabel("Number of SpaceWire Packets")
plt.ylabel("Efficiency (%)")
#plt.yscale('log')
plt.grid(True, which='both', axis='both')
plt.xlim([1, packets-1])

#plot eth data
#plt.plot(x_data, eth_data_array, "-g", label="Ethernet")

plt.plot(x_data, sw_poe_data_array, "-b", label="SpaceWire PoE")

#insert legend and show
plt.legend(loc="lower left")
#plt.show()
'''
#plot figure 2
fig, (ax1, ax2) = plt.subplots(2, sharex=True)
fig.suptitle("SpaceWire Packets Sent in Frame vs Frame Length", fontweight='bold')

#subplot 1
ax1.set_title('SpaceWire Data Size Range 1 - 1498 Bytes', fontsize=10)
ax1.set(ylabel='Efficiency (%)')
ax1.set_ylim(0, 100)
ax1.yaxis.set_major_locator(MaxNLocator(integer=True))
ax1.yaxis.set_major_locator(MultipleLocator(10))
ax1.yaxis.set_major_formatter(FormatStrFormatter('%d'))
ax1.yaxis.set_minor_locator(MultipleLocator(2.5))

#subplot 2
ax2.set_title('\nSpaceWire Data Size Range 1 - 250 Bytes', fontsize=10)
ax2.set(xlabel="Number of SpaceWire Packets Sent in Frame", ylabel='Efficiency (%)')
ax2.set_xlim(0, max(packetsSent3)+1)
ax2.xaxis.set_major_locator(MaxNLocator(integer=True))
ax2.xaxis.set_major_locator(MultipleLocator(5))
ax2.xaxis.set_major_formatter(FormatStrFormatter('%d'))
ax2.xaxis.set_minor_locator(MultipleLocator(1))
ax2.yaxis.set_major_locator(MaxNLocator(integer=True))
ax2.yaxis.set_major_locator(MultipleLocator(10))
ax2.yaxis.set_major_formatter(FormatStrFormatter('%d'))
ax2.yaxis.set_minor_locator(MultipleLocator(2.5))
ax2.set_ylim(0, 100)

#ax2.ylabel("Efficiency (%)")
#plt.xscale('log')
ax1.plot(packetsSent, efficiency, 'ro', markersize=4, label='Eth Frame Payload > 44 Bytes')
ax1.plot(packetsSent2, efficiency2, 'bo', markersize=4, label='Eth Frame Payload <= 44 Bytes')
ax1.plot(packetsSent5, efficiency5, 'bo', markersize=4)

ax2.plot(packetsSent3, efficiency3, 'ro', markersize=4)
ax2.plot(packetsSent4, efficiency4, 'bo', markersize=4)
ax2.plot(packetsSent5, efficiency5, 'bo', markersize=4)
#plt.plot(packetsSent, m*packetsSent + b)
box = ax2.get_position()
#ax2.set_position([box.x0, box.y0, box.width * 0.8, box.height])
#fig.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05), ncol=2)
fig.legend(loc = 'upper center', bbox_to_anchor=(0.71, 0.19))
ax1.grid(True)
ax2.grid(True)
plt.show()

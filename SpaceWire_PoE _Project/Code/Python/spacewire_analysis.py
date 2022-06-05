import matplotlib.pyplot as plt
import numpy as np

swpoeFrameSize = 20          #sie of ethernet frame headers
swpoeFrameSizeMin = 64       #minimum eth frame size for sw poe
ethFrameSizeMin = 64         #minimum eth frame
ethFrameSize = 18            #size of eth headers
payload_max = 1498           #max no. of byted to analyse
sw_poe_data_array = np.array([None]*1500)
sw_data_array = np.array([None]*1500)
eth_data_array = np.array([None]*1500)
x_data = np.array([None]*1500)
byte = 1

#generate data for spacewire poe
while byte < payload_max:
    
    if(byte < 45):
        efficiency = (byte/swpoeFrameSizeMin)*100

    else:
        efficiency = (byte/(byte+swpoeFrameSize))*100
    
    sw_poe_data_array[byte-1] = efficiency
    #x_data[byte-1] = byte
    byte = byte + 1

byte = 1        #reset index to 1

#generate data for spacewire
while byte < payload_max:
    efficiency = (byte/(byte+1))*100
    sw_data_array[byte-1] = efficiency
    byte = byte + 1

byte = 1
#generate data for ethernet
while byte < 1500:
    if(byte < 47):
        efficiency = (byte/ethFrameSizeMin)*100

    else:
        efficiency = (byte/(byte+ethFrameSize))*100
    
    eth_data_array[byte-1] = efficiency
    x_data[byte-1] = byte
    byte = byte + 1

#setup plot
plt.title("Frame vs Payload Length Efficiency")
plt.xlabel("Payload Length (bytes)")
plt.ylabel("Efficiency (%)")
plt.xscale('log')
plt.grid(True, which='both', axis='both')
plt.xlim([1, 1600])

#plot sw data
plt.plot(x_data, sw_data_array, "-r", label="SpaceWire (No addressing)")

#plot spacewire poe data
plt.plot(x_data, sw_poe_data_array, "-b", label="SpaceWire PoE")

#plot eth data
plt.plot(x_data, eth_data_array, "-g", label="Ethernet")

#insert legend
plt.legend(loc="lower right")

plt.show()
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter, MaxNLocator, MultipleLocator, FormatStrFormatter, AutoMinorLocator, FixedLocator
import numpy as np

swpoeFrameSize = 28          #size of ethernet frame headers
swpoeFrameSizeMin = 72       #minimum eth frame size for sw poe
ethFrameSizeMin = 64         #minimum eth frame
ethFrameSize = 18            #size of eth headers

perBytePower = 0.07875

usefulPowerArray = np.array([None]*12)
wastedPowerArray = np.array([None]*12)
totalPowerArray = np.array([None]*12)
index = 0

width = 0.75

dataSizeList = [1, 10, 30, 40, 60, 100, 200, 300, 500, 750, 1000, 1498]
dataSizeArray = np.array(dataSizeList)
print(dataSizeArray)

for dataSize in dataSizeList:

    if(dataSize < 45):
        
        totalPowerArray[index] = perBytePower*swpoeFrameSizeMin
        usefulPowerArray[index] = perBytePower*dataSize
        wastedPowerArray[index] = perBytePower*(swpoeFrameSizeMin-dataSize)

    else:
        
        totalPowerArray[index] = perBytePower*(dataSize+swpoeFrameSize)
        usefulPowerArray[index] = perBytePower*dataSize
        wastedPowerArray[index] = perBytePower*swpoeFrameSize

    index = index + 1

'''#check data
print("Total Power")
for x in totalPowerArray:
    print(x)

print("Useful Power")
for x in usefulPowerArray:
    print(x)

print("Wasted Power")
for x in wastedPowerArray:
    print(x)
'''
#plot the data

fig, ax = plt.subplots()
ax.set_title("Total Power for Transmission of SpaceWire PoE Frame", fontweight='bold')

x = np.arange(0, len(dataSizeList))


rects1 = ax.bar(x, usefulPowerArray, width, label='Power for SpaceWire Data', color = 'g')
ax.bar(x, wastedPowerArray, width, bottom = usefulPowerArray, label = 'Power for Non-useful Data', color = 'r')
ax.set_xticks(x)
#ax.set_xticks(dataSizeList)
ax.set_xticklabels(dataSizeList)
ax.yaxis.set_major_locator(MaxNLocator(integer=True))
ax.yaxis.set_major_locator(MultipleLocator(10))
ax.yaxis.set_major_formatter(FormatStrFormatter('%d'))



def autolabel():
    """Attach a text label above each bar in *rects*, displaying its height."""
    index = 0
    while(index<12):
        height = totalPowerArray[index]
        ax.annotate('{:.2f}'.format(height)+'W',
                    xy=(x[index], height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom', fontsize='8')
        index = index+1

autolabel()

plt.xlabel("SpaceWire Data Size within Frame (Bytes)", fontweight='bold')
plt.ylabel("Total Power (W)", fontweight='bold')
plt.legend()

plt.tight_layout()
plt.show()

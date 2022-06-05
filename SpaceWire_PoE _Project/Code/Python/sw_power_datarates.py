import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter, MaxNLocator, MultipleLocator, FormatStrFormatter, AutoMinorLocator, FixedLocator
import numpy as np

Pc = 4
Pe = 1.102
N = 2

usefulPowerArray = np.array([None]*3)
wastedPowerArray = np.array([None]*3)
totalPowerArray = np.array([None]*3)
ind = 0

width = 0.75

Pp = np.array([0.00016, 0.0016, 0.016])
Pb = np.array ([0.004145, 0.04145, 0.4145])
dataRateList = [10, 100, 1000]
dataRateArray = np.array(dataRateList)

while ind < 3:

    usefulPowerArray[ind] = N*(Pb[ind] + Pp[ind])
    wastedPowerArray[ind] = Pc + (N*Pe)
    totalPowerArray[ind] = usefulPowerArray[ind]+wastedPowerArray[ind]

    ind = ind + 1

#check data
print("Total Power")
for x in totalPowerArray:
    print(x)

print("Useful Power")
for x in usefulPowerArray:
    print(x)

print("Wasted Power")
for x in wastedPowerArray:
    print(x)

#plot the data

fig, ax = plt.subplots()
ax.set_title("Total Power Consumption of SpaceWire PoE System", fontweight='bold')

x = np.arange(0, len(dataRateList))


rects1 = ax.bar(x, wastedPowerArray, width, label='Power Consumption of SpaceWire PoE System', color = 'g')
ax.bar(x, usefulPowerArray, width, bottom = wastedPowerArray, label = 'Power Consumption for SpaceWire Data', color = 'r')
ax.set_xticks(x)
#ax.set_xticks(dataRateList)
ax.set_xticklabels(dataRateList)
#ax.yaxis.set_major_locator(MaxNLocator(integer=True))
#ax.yaxis.set_major_locator(MultipleLocator(10))
#ax.yaxis.set_major_formatter(FormatStrFormatter('%d'))

low = min(totalPowerArray)
high = max(totalPowerArray)

plt.ylim([low-0.5, high+0.5]) 



def autolabel():
    """Attach a text label above each bar in *rects*, displaying its height."""
    index = 0
    while(index<3):
        height = totalPowerArray[index]
        ax.annotate('{:.4f}'.format(height-wastedPowerArray[index])+'W',
                    xy=(x[index], height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom', fontsize='8')
        index = index+1

autolabel()

plt.xlabel("SpaceWire PoE DataRate (Mbps)", fontweight='bold')
plt.ylabel("Total Power Consumption (W)", fontweight='bold')
plt.legend()

#plt.tight_layout()
plt.show()

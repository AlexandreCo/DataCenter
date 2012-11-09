from numpy import *
from pylab import *
import Gnuplot
import time
import csv
import matplotlib.ticker as ticker

lm=[]
y=[]
sht=[]
pression=[]
hum=[]
heure=[]

cr=csv.reader(open('20101118_071200+_picolive_read.log', 'rU'), delimiter=';')
for row in cr:
    if len(row)==6:
        lm.append(float(row[3])/2)
        y.append(float(row[0]))
        sht.append((float(row[4])/10)-1.8)
        pression.append(int(row[2]))
        hum.append(int(row[5]))
        heure.append(row[1])

fig = figure()
ax=fig.add_subplot(311)
ax.plot(y, sht, label='sht')
ax.plot(y, lm, label='lm')
xticks(y, heure, rotation=45)
ax.xaxis.set_major_locator(MaxNLocator(10))
xlabel('Heure') 
ylabel('Temperature')
subplot(312)
plot(y, pression, label='MPX')
xlabel('Heure') 
ylabel('Pression')
subplot(313)
plot(y, hum, label='sht')
xlabel('Heure') 
ylabel('Humidite')

# first we'll do it the default way, with gaps on weekends

show()

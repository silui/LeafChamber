##co2grapher.py
##A simple and quick program to parse, graph, and calculate slope of the txt files saved on SD card.
import numpy as np
import matplotlib.pyplot as plt
list1=[]
userinput = input('Enter file path\n')    ##prompt user for input file
myfile=open(userinput)
linearray=myfile.readlines()
for x in range(1, len(linearray)-1):
    tokenarray=linearray[x].split()
    list1.append(tokenarray[4])           ##depends on how the input file is written, you might need to tweak 4 on this line. This works with my current arduino code
##list1 is now y data point aka co2 concentration
list2=list(range(1,len(list1)+1))
list1=[int(i) for i in list1]              ##list 1 is all the x points, Note this is not seconds since 1 measurement is not necessary 1 second
fit = np.polyfit(list2,list1,1)
fit_fn = np.poly1d(fit)
## fit_fn is now a function which takes in x and returns an estimate for y
plt.plot(list2,list1, 'r', list2, fit_fn(list2), 'k')
plt.ylabel('co2 ppm')
plt.xlabel('not seconds')
plt.title('Apparatus non-breath seal test(30min)'+ str(fit_fn) +'\n' + userinput)   ##set title to display function
plt.show()

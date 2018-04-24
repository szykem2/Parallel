#!/usr/bin/python 
import random
import time

Xpos = []
Ypos = []
Xspeed = []
Yspeed = []
mass = []
num = 1000
v = 0.5


for i in range(0, num+1):
	val = random.uniform(-v,v)
	while (val in Xpos):
		val = random.uniform(-v,v)
	Xpos.append(val)
	val = random.uniform(-v,v)
	
	while (val in Ypos):
		val = random.uniform(-v,v)
	Ypos.append(val)
	val = random.uniform(-v/5.0,v/5.0)
	
	while (val in Xspeed):
		val = random.uniform(-v/5.0,v/5.0)
	Xspeed.append(val)
	val = random.uniform(-v/5.0,v/5.0)
	
	while (val in Yspeed):
		val = random.uniform(-v/5.0,v/5.0)
	Yspeed.append(val)
	val = random.uniform(0,1)
	
	while (val in mass):
		val = random.uniform(0,100)
	mass.append(val)
	
with open("dataFile", "w") as f:
	for i in range(0,num-1):
		f.write('{0:.6f},{1:.6f};{2:.6f},{3:.6f};{4:.6f}\n'.format(Xpos[i], Ypos[i], Xspeed[i], Yspeed[i], mass[i]))
	f.write('{0:.6f},{1:.6f};{2:.6f},{3:.6f};{4:.6f}'.format(Xpos[num-1], Ypos[num-1], Xspeed[num-1], Yspeed[num-1], mass[num-1]))

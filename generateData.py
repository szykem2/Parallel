#!/usr/bin/python 
import random
import time

Xpos = []
Ypos = []
Xspeed = []
Yspeed = []
mass = []
num = 10000

for i in range(0, num):
	val = random.uniform(-1,1)
	while (val in Xpos):
		val = random.uniform(-1,1)
	Xpos.append(val)
	val = random.uniform(-1,1)
	
	while (val in Ypos):
		val = random.uniform(-1,1)
	Ypos.append(val)
	val = random.uniform(-1,1)
	
	while (val in Xspeed):
		val = random.uniform(-1,1)
	Xspeed.append(val)
	val = random.uniform(-1,1)
	
	while (val in Yspeed):
		val = random.uniform(-1,1)
	Yspeed.append(val)
	val = random.uniform(0,1)
	
	while (val in mass):
		val = random.uniform(0,1)
	mass.append(val)
	
with open("dataFile", "w") as f:
	for i in range(0,num):
		f.write('{0:.6f},{1:.6f};{2:.6f},{3:.6f};{4:.6f}\n'.format(Xpos[i], Ypos[i], Xspeed[i], Yspeed[i], mass[i]))
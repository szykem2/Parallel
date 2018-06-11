#!/usr/bin/python
from __future__ import division
from math import sqrt
import random

masses=[250]
xpos=[0]
ypos=[0]
xspeed=[0]
yspeed=[0]
G=0.001
num=10

def getVel(xpos):
    return sqrt(G*250/xpos)

for i in range(1,num):
    masses.append(random.uniform(0,0.08))
    xpos.append(i/num * 0.9)
    ypos.append(0)
    xspeed.append(0)
    yspeed.append(-1*getVel(i/num * 0.9))

with open("planets.data", "w") as f:
    for i in range(0,num):
        f.write('{0:.6f},{1:.6f};{2:.6f},{3:.6f};{4:.6f}\n'.format(xpos[i], ypos[i], xspeed[i], yspeed[i], masses[i]))


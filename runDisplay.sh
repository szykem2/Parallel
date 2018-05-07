#!/bin/bash
Xnest -ac -listen tcp :1 &
xterm -fn 9x15 -display :1 & 


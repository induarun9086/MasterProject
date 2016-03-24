#!/usr/bin/env python

import os
import glob
from shutil import copyfile

files = glob.glob('/home/pi/testImgs/seg_*.bmp')
for file in files:
  copyfile(file, '/home/pi/App/' + os.path.basename(file))
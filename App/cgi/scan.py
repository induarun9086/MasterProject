#!/usr/bin/env python

import os
import sys
import glob
import subprocess

print 'Content-type: text/html;'
print '\n\n'
print '<html>'
print '<head>'
print '<title> Scan Results </title>'
print '</head>'
print '<body>'
print 'Scan Results: '

files = glob.glob('/home/pi/App/*bmp')
print '<br /><br />Deleted files: '
print files
print '<br />'
for file in files:
    os.remove(file)

subprocess.call(['/home/pi/App/imgAcqu/stepper.py'])

files = glob.glob('/home/pi/App/seg_*.bmp')
print '<br />Available input files: '
print files
print '<br />'

idx = 0
for file in files:
  PIPE = subprocess.PIPE
  pd = subprocess.Popen(['/home/pi/App/imgProc/bin/imgProc', '2', str(idx), file],
                         stdout=PIPE, stderr=PIPE)
  idx += 1
  while True:
      output = pd.stdout.read(1)
      if output == '' and pd.poll() != None:
        break
      if output != '':
        output = output.replace('\n', '<br />')
        sys.stdout.write(output)
        sys.stdout.flush()

files = glob.glob('/home/pi/App/output*bmp')
print '<br />Output files: '
print files
print '<br /><br />'
for file in files:
    if file.find('000.bmp') == -1:
      print '<a href="search.py?ob=' + file + '"> <img src="../' + os.path.basename(file) + '" /></a>'
    else:
      print '<img src="../' + os.path.basename(file) + '" />'
    
print '</body>'
print '<html>'


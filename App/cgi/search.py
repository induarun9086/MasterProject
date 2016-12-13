#!/usr/bin/env python

import os
import cgi
import sys
import glob
import subprocess

print 'Content-type: text/html;'
print '\n\n'
print '<html>'
print '<head>'
print '<title> Search Results </title>'
print '</head>'
print '<body>'
print 'Search Results: '

files = glob.glob('/home/pi/App/seg_*.bmp')
print '<br /><br />Deleted files: '
print files
print '<br />'
for file in files:
    os.remove(file)

subprocess.call(['/home/pi/App/imgAcqu/stepper.py'])

form = cgi.FieldStorage()

if "ob" not in form:
  print 'Provide object as image for searching....'
else:
  files = glob.glob('/home/pi/App/seg_*.bmp')
  print '<br />Available input files: '
  print files
  print '<br />'
  
  try:
    os.remove("maxValList.txt")
  except OSError:
      pass
      
  idx = 0
  for file in files:
    PIPE = subprocess.PIPE
    pd = subprocess.Popen(['/home/pi/App/imgProc/bin/imgProc', '3', str(idx), file, form["ob"].value],
                           stdout=PIPE, stderr=PIPE)
    idx += 1
    while True:
        output = pd.stdout.read(1)
        if output == '' and pd.poll() != None:
          break
          
  idx = 0
  for file in files:
    PIPE = subprocess.PIPE
    pd = subprocess.Popen(['/home/pi/App/imgProc/bin/imgProc', '5', str(idx), file, form["ob"].value],
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

print '<br /><img src="../' + os.path.basename(form["ob"].value) + '" /><br />'
          
files = glob.glob('/home/pi/App/markedImg*bmp')
print '<br /><br />Output files: '
print files
print '<br />'
for file in files:
    print '<img src="../' + os.path.basename(file) + '" />'
    
print '</body>'
print '<html>'
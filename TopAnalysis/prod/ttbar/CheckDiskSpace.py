#! /usr/bin/env python
import os
from KKousour.TopAnalysis.eostools import *

path = '/store/cmst3/user/kkousour/'

dirs = ls_EOS(path)

total_size = 0
for dd in dirs:
  print dd+", size = "+str(eosDirSize(dd))+" GB"
  total_size += eosDirSize(dd)
print "Total size = "+str(total_size)+" GB"


#!/usr/bin/env python
import sys, getopt, math, os
import argparse
#from scipy.odr import *
#import scipy
import pandas as pd
#import numpy as np
import matplotlib.pyplot as plt
#from UNLP import Herramientas as H
#from UNLP import Funciones as F

#try:
#    plt.xkcd()
#except:
#    print('Didn\'t find xkcd theme...')


def main(argv):
  parser = argparse.ArgumentParser("Ploting data")
  parser.add_argument('-d', '--DataFile',  required=True, help="File with data to plot")
  parser.add_argument('-r', '--Rows',  nargs='+', required=False, help="Row to be used as y to be used to plot")
  parser.add_argument('-x', '--EjeX',   required=False, help="x y to be used to plot")
  parser.add_argument('-f', '--Fecha',  action="store_true", help="If set, X will be interpreted as date")
  args = parser.parse_args()

  datafile = args.DataFile

  data = pd.read_csv(datafile)
  output = 'Plot.png'

  PlotArgs=[]


  if args.Rows:
      output = '_'.join(args.Rows)+'.png'

  keys=[]
  for k in data:
      keys.append(k)

  if args.Rows:
      print(args.Rows)
      for i in args.Rows:
          print(i)
          print(keys)
          data.plot(y=i)
  else:
      data.plot()
  

  plt.savefig(output)
  plt.show()

  
if __name__ == "__main__":
    main(sys.argv[1:])






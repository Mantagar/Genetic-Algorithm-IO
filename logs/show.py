import numpy
import os
import matplotlib.pyplot as pp
from pandas import read_csv
import pandas as pd
import sys

path = sys.argv[1]

data = read_csv(path)

data.plot()
pp.xlabel('Steps')
pp.ylabel('Loss')
pp.title(path)
pp.yscale('log')

pp.show()
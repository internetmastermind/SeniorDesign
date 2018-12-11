from pandas import DataFrame, read_csv

import matplotlib.pyplot as plt
from matplotlib import dates as mdates

import pandas as pd
import sys
import matplotlib
import datetime

print('Python version ' + sys.version)
print('Pandas version ' + pd.__version__)
print('Matplotlib version ' + matplotlib.__version__)

file_name = 'log.txt'

data = pd.read_csv(file_name, sep='\t', lineterminator='\n', header=None, index_col=2, parse_dates=True)
data.index = pd.to_datetime(data.index)
data.columns = ['ID', 'State']
del data['ID']
print(data)

#datadata.set_index('Timestamp', drop=False, inplace=True)
ax = data.groupby(pd.Grouper(freq='1H')).count().plot(kind='bar')
f = lambda x: datetime.datetime.strptime(x, '%Y-%m-%d %H:%M:%S').strftime('%-I%p')
ax.set_xticklabels([ f(x.get_text()) for x in ax.get_xticklabels()])

plt.show()
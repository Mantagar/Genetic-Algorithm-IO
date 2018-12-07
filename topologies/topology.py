import sys
import random
n = int(sys.argv[1])
ranks = range(n)
links = []
for r in ranks:
  selec = random.sample(ranks, int(n*random.random()/2))
  for i in selec:
    links.append((r,i))
  if r == 0:
    links.append((0,n-1))
  else:
    links.append((r,r-1))
for (x,y) in list(links):
  links.append((y,x))
links = [ (x,y) for (x,y) in links if x!=y ]
links = set(links)

for (x,y) in links:
  print(str(x)+' '+str(y))

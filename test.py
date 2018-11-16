import sys
import matplotlib.pyplot as plt
from subprocess import Popen, PIPE

dim=int(sys.argv[1])
size=int(sys.argv[2])
mutate=float(sys.argv[3])
stop=int(sys.argv[4])
problem=int(sys.argv[5])

p = Popen(["./a.out", str(dim), str(size), str(mutate), str(stop), str(problem)], stdout=PIPE)
loss=[float(i) for i in filter(None, p.communicate()[0].split('\n'))]
exit_code = p.wait()

print(loss[-1])
plt.plot(loss)
plt.xlabel("generation")
plt.ylabel("fitness")
plt.show()

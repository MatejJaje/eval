'''
def lcm(a,b):
    g=max(a,b)
    s=min(a,b)
    for i in range(g,a*b+1,g):
        if i %s == 0:
            return i
    return a*b
'''
from numpy import lcm
n=int(input())
visine = tuple(map(int,input().split()))
dijelovi = tuple(map(int,input().split()))
duzine = n+2
duzine+= dijelovi[0]
for i in range(n-1):
    umnozak = dijelovi[i]*dijelovi[i+1]
    l = visine[i]*dijelovi[i+1]
    r = visine[i+1]*dijelovi[i]
    duzine -= min(visine[i],visine[i+1])//(lcm(l,r)//umnozak) 
    duzine += dijelovi[i+1]

print(duzine)


n, m = list(map(int,input().split()))
start = []
end = []
portali = [[0 for i in range(m)]for j in range(n)]
tempPortali={}
def func(s,i,j):
    if s == "#":
        return 1
    elif s==".": return 0
    elif s=="S": start.extend([i,j]); return 0
    elif s=="E": end.extend([i,j]); return 0
    else:
        if s in tempPortali:
            portali[tempPortali[s][0]][tempPortali[s][1]]=[i,j]
            portali[i][j]=tempPortali[s]
        tempPortali[s]=[i,j]
        return 0
        
ls = [list(input())for i in range(n)]

for i in range(n):
    for j in range(m):
        ls[i][j] = func(ls[i][j],i,j)

printed = False
koraci = [[0 for i in range(m)]for j in range(n)]
koraci[start[0]][start[1]]=1
next = [start[:]]
korak = 0
while next!=[]:
    korak+=1
    nextNext = []
    for i,j in next:
        for pi, pj in [[1,0],[-1,0],[0,1],[0,-1],[1,1],[1,-1],[-1,1],[-1,-1]]:
            i2=i+pi; j2=j+pj
            while i2<n and i2>=0 and j2<m and j2>=0 and ls[i2][j2]==0:
                if not koraci[i2][j2]:
                    koraci[i2][j2]=1
                    nextNext.append([i2,j2])
                    if portali[i2][j2] and not koraci[portali[i2][j2][0]][portali[i2][j2][1]]:
                        nextNext.append(portali[i2][j2])
                        koraci[portali[i2][j2][0]][portali[i2][j2][0]]=1
                i2+=pi; j2+=pj
    next=nextNext[:]
                
    if koraci[end[0]][end[1]]: 
        print(korak)
        printed = True
        break
        

if not printed: print(-1)
    






"""
4 4
S###
.1..
###.
E.1.
"""
'''
5 4
S.21
####
2##1
###.
E..#
'''

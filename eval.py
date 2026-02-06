import sys
import subprocess,time
import platform
if len(sys.argv)!=4:
    sys.exit("usage: python eval.py <testFilePath> <testCasesPath> <timeLimit>")
    
filePath = sys.argv[1]
testPath = sys.argv[2]
timeLimit = float(sys.argv[3])

if platform.system()=="Windows":
    compiler = "powershell /msys64/ucrt64/bin/g++.exe"
    compilerOutput = "./out.exe" 
    shell = "powershell"
elif platform.system()=="Linux":
    compiler = "/bin/bash g++"
    compilerOutput = "./out"
    shell = "/bin/bash"
else:
    sys.exit("OS not supported")

sufix = filePath.split(sep=".")[-1]
if sufix == "py":
    if platform.system()=="Windows":
        coderunner = f"powershell python {filePath}"
    elif platform.system()=="Linux":
        coderunner = f"/bin/bash python {filePath}"
    else:
        sys.exit("OS not supported")

elif sufix =="c" or sufix =="cpp":
    print(subprocess.getoutput(f"{compiler} {filePath} -o {compilerOutput}"))
    if platform.system()=="Windows":
        coderunner = "powershell ./out.exe"
    elif platform.system()=="Linux":
        coderunner = "/bin/bash ./out.exe"
    else:
        sys.exit("OS not supported")

else:
    sys.exit(f"Error: file type {sufix} not supported")

class testCase:
    def __init__(self,name,type):
        self.name = name
        self.type = type
    inPath=None
    outPath=None
    result=None
    time = None
    correct = None

from os import listdir

testCases=[]
names = set()
for file in listdir(testPath):
    file = file.split(sep=".")

    if len(file)==4:
        type = "dummy"
    else:
        type = "testcase"

    if (type,file[-1]) not in names:
        testCases.append(testCase(file[-1],type))
        names.add((type, file[-1]))
task = file[0]

testCases.sort(key=lambda x: [x.type,x.name])


for case in testCases:
    if case.type == "dummy":
        caseInPath = f"{testPath}/{task}.dummy.in.{case.name}"
        caseOutPath = f"{testPath}/{task}.dummy.out.{case.name}"
    else:
        caseInPath = f"{testPath}/{task}.in.{case.name}"
        caseOutPath = f"{testPath}/{task}.out.{case.name}"
    case.inPath = caseInPath
    case.outPath = caseOutPath
    
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'





for case in testCases:

    start = time.perf_counter()

    result = subprocess.getoutput(f"{shell} cat {case.inPath}|{coderunner}")

    end = time.perf_counter()

    case.time = end - start   
    case.result = result

    
    
    if open(case.outPath,"r").read().strip()==case.result.strip():
        case.correct =True
    else:
        case.correct = False

    if case.correct and case.time<timeLimit:
        print(f"[{bcolors.OKGREEN}OK{bcolors.ENDC}]",end=" ")
    elif case.correct:
        print(f"[{bcolors.WARNING}TIME LIMIT EXCEEDED{bcolors.ENDC}]",end=" ")
    else:
        print(f"[{bcolors.FAIL}FAIL{bcolors.ENDC}]",end=" ")

    print(case.type,case.name, "\tExecution time:", round(case.time,3))


instruction = input("> ")
while instruction!="q":
    found = False
    for case in testCases:
        if case.name==instruction:
            found = True
            break
    if found: 
        print(f"....Input ....\n{open(case.inPath,"r").read()}")
        print(f"\n....User Result ....\n{case.result}\n")
        print(f"....Solution ....\n{open(case.outPath,"r").read()}")
    else:
        print("testCase not found")
    
    instruction = input("> ")
#!/usr/bin/python3
import os
import subprocess


cwd = os.getcwd()
dirPath = "./lists"

print(cwd)


subprocess.call("bash compile.sh", shell=True, cwd="{}/..".format(cwd))

for filename in os.listdir(dirPath):
    filePath = os.path.join(dirPath, filename)
    if os.path.isfile(filePath):
        # print(filename)
        # print(filePath)
        fName = filename.split(".")[0]
        try:
            os.rmdir("./{}".format(fName))
        except:
            print("[rmdir] Folder {} not exists".format(fName))
        
        
        try:
            os.mkdir("./{}".format(fName))
        except:
            pass
        
        command = f"bash download.sh {filePath} ./{fName}/banks"

        subprocess.call(command, shell=True, cwd=cwd)

        subprocess.call("python3 gbk2ig.py {} {}".format(f"./{fName}/banks/", f"./{fName}/{fName}.pdi"), shell=True, cwd=cwd)

        command = "python3 ../../tools/genesDistribution.py {} {}".format(f"./{fName}.pdi", f"./{fName}")
        subprocess.call(command, shell=True, cwd="{}/{}".format(cwd, fName))
        
        os.mkdir("./{}/nets".format(fName))
        command = "bash execute.sh -i {} -o {} -p {}".format(f"./{fName}/{fName}.pdi", f"./{fName}/nets/{fName}", f"./{fName}/")
        subprocess.call(command, shell=True, cwd=cwd)







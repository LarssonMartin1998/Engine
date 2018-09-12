import os
import subprocess

local_dir = os.path.dirname(os.path.realpath(__file__)) + "\\"
premake_dir = local_dir + "..\\utils\\premake\\"

# remove all the VS solutions in the local directory before producing a new one.
for filename in os.listdir(local_dir):
    if filename.endswith(".sln"): 
        os.remove(filename)

args = [
        premake_dir + "premake5.exe",
        "vs2017",
        "--file=" + premake_dir + "premake5.lua"
        ]

subprocess.run(args)

# IMPORTANT: If this script only removes your old solution without producing a new one, run this script through cmd to see what premake5 errors are causing it not to spit out a new solution!
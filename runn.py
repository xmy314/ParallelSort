import os
import re
import subprocess

print("start test for bad")

file_name = "parallel_mergesort_bad.h"
command = "parbad"

with open(file_name,"r") as fi:
    content = fi.read()
    

with open(f"result_{command}.txt","w") as fi:
    pass


merge_granularity_options = [1<<i for i in range(5,20)]
merge_sort_granularity_options = [1<<i for i in range(5,20)]

for merge_granularity in merge_granularity_options:
    for merge_sort_granularity in merge_sort_granularity_options:
        content = re.sub(r"inline constexpr int MERGE_GRANULARITY = .*;",f"inline constexpr int MERGE_GRANULARITY = {merge_granularity};",content)
        content = re.sub(r"inline constexpr int MERGE_SORT_GRANULARITY = .*;",f"inline constexpr int MERGE_SORT_GRANULARITY = {merge_sort_granularity};",content)
        with open(file_name,"w") as fi:
            fi.write(content)
            
        result = subprocess.run(f"make; PARLAY_NUM_THREADS=64 ./mergesort 20000000 {command}",capture_output=True,shell=True)
        line = str(result.stdout)
        timing_tuple = tuple(map(float, re.findall(r"Time: mergesort: ([\d.]+)", line)))
        
        with open(f"result_{command}.txt","a") as fi:
            fi.write(f"({merge_granularity}, {merge_sort_granularity}, {timing_tuple})\n")


print("start test for good")

file_name = "parallel_mergesort_good.h"
command = "pargood"

with open(file_name,"r") as fi:
    content = fi.read()
    

with open(f"result_{command}.txt","w") as fi:
    pass


merge_granularity_options = [1<<i for i in range(5,20)]
merge_sort_granularity_options = [1<<i for i in range(5,20)]

for merge_granularity in merge_granularity_options:
    for merge_sort_granularity in merge_sort_granularity_options:
        content = re.sub(r"inline constexpr int MERGE_GRANULARITY = .*;",f"inline constexpr int MERGE_GRANULARITY = {merge_granularity};",content)
        content = re.sub(r"inline constexpr int MERGE_SORT_GRANULARITY = .*;",f"inline constexpr int MERGE_SORT_GRANULARITY = {merge_sort_granularity};",content)
        with open(file_name,"w") as fi:
            fi.write(content)
            
        result = subprocess.run(f"make; PARLAY_NUM_THREADS=64 ./mergesort 20000000 {command}",capture_output=True,shell=True)
        line = str(result.stdout)
        timing_tuple = tuple(map(float, re.findall(r"Time: mergesort: ([\d.]+)", line)))
        
        with open(f"result_{command}.txt","a") as fi:
            fi.write(f"({merge_granularity}, {merge_sort_granularity}, {timing_tuple})\n")


print("start test for seq")

file_name = "sequential_mergesort.h"
command = "seq"

with open(file_name,"r") as fi:
    content = fi.read()
    

with open(f"result_{command}.txt","w") as fi:
    pass


merge_sort_granularity_options = [1<<i for i in range(5,20)]

for merge_sort_granularity in merge_sort_granularity_options:
    content = re.sub(r"inline constexpr int MERGE_SORT_GRANULARITY = .*;",f"inline constexpr int MERGE_SORT_GRANULARITY = {merge_sort_granularity};",content)
    with open(file_name,"w") as fi:
        fi.write(content)
        
    result = subprocess.run(f"make; PARLAY_NUM_THREADS=64 ./mergesort 20000000 {command}",capture_output=True,shell=True)
    line = str(result.stdout)
    timing_tuple = tuple(map(float, re.findall(r"Time: mergesort: ([\d.]+)", line)))
    
    with open(f"result_{command}.txt","a") as fi:
        fi.write(f"( {merge_sort_granularity}, {timing_tuple})\n")


print("start test for parseq")

file_name = "parallel_mergesort_seq.h"
command = "parseq"

with open(file_name,"r") as fi:
    content = fi.read()
    

with open(f"result_{command}.txt","w") as fi:
    pass


merge_sort_granularity_options = [1<<i for i in range(5,20)]

for merge_sort_granularity in merge_sort_granularity_options:
    content = re.sub(r"inline constexpr int MERGE_SORT_GRANULARITY = .*;",f"inline constexpr int MERGE_SORT_GRANULARITY = {merge_sort_granularity};",content)
    with open(file_name,"w") as fi:
        fi.write(content)
        
    result = subprocess.run(f"make; PARLAY_NUM_THREADS=64 ./mergesort 20000000 {command}",capture_output=True,shell=True)
    line = str(result.stdout)
    timing_tuple = tuple(map(float, re.findall(r"Time: mergesort: ([\d.]+)", line)))
    
    with open(f"result_{command}.txt","a") as fi:
        fi.write(f"({merge_sort_granularity}, {timing_tuple})\n")

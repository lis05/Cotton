#!/bin/python3

import glob
import subprocess

tests = [f for f in glob.glob("**", recursive=True) if f.endswith(".ctn")]
succeeded = 0
failed = 0

for file in tests:
    fd = open(file, "r")
    src = fd.read()
    fd.close()
    lines = src.splitlines()
    words = src.split()

    desc = lines[0].replace("//", "").strip()


    expected_output = []

    try:
        output_first = words.index("BEGIN_MATCH_WORDS")
        output_last = words.index("END_MATCH_WORDS")
        expected_output = words[output_first + 1: output_last]
    except ValueError:
        pass

    proc = subprocess.run(["/home/lis05/Projects/Cotton/build/cotton_int/cotton_int", file], capture_output=True)
    if proc.returncode != 0:
        print(f"❌ {desc} - FAILED: exit code {proc.returncode}\nErrors:")
        print(proc.stderr.decode())
        failed = failed + 1
        continue

    proc_output = proc.stdout.decode().split()

    if len(expected_output) != len(proc_output):
        print(f"❌ {desc} - FAILED: produced %s words, expected %s" % (len(proc_output), len(expected_output)))
        failed = failed + 1
        continue
    flag = 0
    for i in range(len(expected_output)):
        if expected_output[i] != proc_output[i]:
            print(f"❌ {desc} - FAILED: words mismatch at position %s: produced %s, expected %s" % (i, proc_output[i], expected_output[i]))
            failed = failed + 1
            flag = 1
            break
    if flag == 1: 
        continue
    
    print(f"✅ {desc} - SUCCESS")
    succeeded = succeeded + 1

print("SUCCEEDED %s, FAILED %s (total %s)" % (succeeded, failed, succeeded + failed))

    


    

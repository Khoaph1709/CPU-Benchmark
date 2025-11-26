import os, subprocess, sys, glob, time

def check_corressness():
    # Clean and rebuild
    print("Rebuilding...")
    subprocess.run(["make", "clean"])
    subprocess.run(["make"])
    # espect the output of the program lwords 
    # to be the same as running word program
    # with the input file
    print("Checking correctness...")
    correct_file = 0
    total_file = 0
    for file_text in glob.glob("gutenberg/*.txt"):
        res1 = subprocess.run(["./words"], stdin=open(file_text, "r"), stdout=open("output1.txt", "w"), stderr=open("output1.txt", "w"))
        res2 = subprocess.run(["./lwords"], stdin=open(file_text, "r"), stdout=open("output2.txt", "w"), stderr=open("output2.txt", "w"))
        res3 = subprocess.run(["./pwords_v1"], stdin=open(file_text, "r"), stdout=open("output3.txt", "w"), stderr=open("output3.txt", "w"))
        print("Comparing " + file_text)
        res4 = subprocess.run(["./pwords_v2"], stdin=open(file_text, "r"), stdout=open("output4.txt", "w"), stderr=open("output4.txt", "w"))
        if open("output1.txt", "r").read() == open("output2.txt", "r").read() and open("output1.txt", "r").read() == open("output3.txt", "r").read() and open("output1.txt", "r").read() == open("output4.txt", "r").read():
            correct_file += 1
        total_file += 1
    print("Grade: " + str(correct_file) + "/" + str(total_file))
    if correct_file == total_file:
        print("Good job!")
        return True
    else:
        print("Incorrect. Try again.")
        return False

def check_speed():
    print("\nChecking speed (all files at once)...")
    
    all_files = glob.glob("gutenberg/*.txt")
    if not all_files:
        print("No files found in gutenberg/ directory to test speed.")
        return False

    dev_null = open(os.devnull, 'w')

    tik = time.time()
    res1 = subprocess.run(["./words"] + all_files, stdout=dev_null, stderr=dev_null)
    tok = time.time()
    time_elapsed1 = tok - tik
    print(f"Time elapsed for words (total): {time_elapsed1:.4f}s")

    tik = time.time()
    res2 = subprocess.run(["./lwords"] + all_files, stdout=dev_null, stderr=dev_null)
    tok = time.time()
    time_elapsed2 = tok - tik
    print(f"Time elapsed for lwords (total): {time_elapsed2:.4f}s")

    tik = time.time()
    res3 = subprocess.run(["./pwords_v1"] + all_files, stdout=dev_null, stderr=dev_null)
    tok = time.time()
    time_elapsed3 = tok - tik
    print(f"Time elapsed for pwords_v1 (total): {time_elapsed3:.4f}s")

    tik = time.time()
    res4 = subprocess.run(["./pwords_v2"] + all_files, stdout=dev_null, stderr=dev_null)
    tok = time.time()
    time_elapsed4 = tok - tik
    print(f"Time elapsed for pwords_v2 (total): {time_elapsed4:.4f}s")

    dev_null.close()

    if res1.returncode != 0 or res2.returncode != 0 or res3.returncode != 0 or res4.returncode != 0:
        print("Incorrect. One or more programs exited with non-zero return code.")
        return False

    if time_elapsed3 < time_elapsed1 and time_elapsed3 < time_elapsed2 and time_elapsed3 < time_elapsed4:
        print("Good job! pwords is faster than all single-threaded versions.")
        return True
    elif time_elapsed3 < time_elapsed1 or time_elapsed3 < time_elapsed2 or time_elapsed3 < time_elapsed4:
        print("Good job! pwords is faster than at least one single-threaded version.")
        return True
    else:
        print("Incorrect. pwords is not faster than the single-threaded versions.")
        print(f"(pwords: {time_elapsed3:.4f}s vs lwords: {time_elapsed2:.4f}s)")
        return False

if __name__ == "__main__":
    if check_corressness():
        if check_speed():
            exit(0)
    exit(1)

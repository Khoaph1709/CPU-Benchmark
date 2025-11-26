import os
import subprocess
import time
import json
import statistics
import platform
import glob
import re

TEST_SETS = {
    "Set_A": "dataset/set_A/*.txt",
    "Set_B": "dataset/set_B/*.txt",
    "Set_C": "dataset/set_C/*.txt"  
}
ITERATIONS = 3 

PROGRAMS = [
    "./lwords",      
    "./pwords_v1",   
    "./pwords_v2"    
]

def get_cpu_info():
    try:
        command = "grep 'model name' /proc/cpuinfo | head -1"
        output = subprocess.check_output(command, shell=True).decode().strip()
        cpu_name_raw = output.split(': ')[1]
    except:
        cpu_name_raw = platform.processor()

    safe_cpu_name = re.sub(r'[^a-zA-Z0-9]', '_', cpu_name_raw)
    
    return cpu_name_raw, safe_cpu_name, os.cpu_count()

def run_command(command, files):
    start = time.time()
    subprocess.run(command + files, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return time.time() - start

def main():
    cpu_display, cpu_filename, threads = get_cpu_info()
    print(f"Detected CPU: {cpu_display}")
    print(f"Threads: {threads}")
    print("Starting ultimate benchmark...")

    results = {
        "cpu_name": cpu_display,
        "Threads": threads,
        "raw_data": {}
    }

    for set_name, path_pattern in TEST_SETS.items():
        files = glob.glob(path_pattern)
        if not files:
            print(f"Warning: Không tìm thấy data cho {set_name}. Bỏ qua.")
            continue
        
        files.sort()

        print(f"\n=== Running {set_name} ({len(files)} files) ===")
        
        set_result = {}
        
        baseline_time = 0
        
        for prog in PROGRAMS:
            if not os.path.exists(prog):
                print(f"  [Skip] Không tìm thấy file {prog}")
                continue

            times = []
            for i in range(ITERATIONS):
                t = run_command([prog], files)
                times.append(t)
            
            mean_time = statistics.mean(times)
            stdev = statistics.stdev(times) if len(times) > 1 else 0
            
            if "lwords" in prog:
                baseline_time = mean_time
                speedup = 1.0
            else:
                speedup = baseline_time / mean_time if baseline_time > 0 else 0

            print(f"  -> {prog:<15}: {mean_time:.4f}s (Speedup: {speedup:.2f}x)")

            prog_key = prog.replace("./", "")
            set_result[prog_key] = {
                "time": mean_time,
                "stdev": stdev,
                "speedup": speedup
            }

        results["raw_data"][set_name] = set_result

    filename = f"result_{cpu_filename}.json"
    with open(filename, 'w') as f:
        json.dump(results, f, indent=4)
    print(f"\nSuccess! Data saved to {filename}")

if __name__ == "__main__":
    main()
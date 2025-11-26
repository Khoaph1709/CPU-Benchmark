# Multithreading Performance Analysis & CPU Benchmarking

![Language](https://img.shields.io/badge/Language-C-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20WSL-orange.svg)
![Tools](https://img.shields.io/badge/Tools-Pthreads%20%7C%20Python%20%7C%20Bash-green.svg)

## 📖 Overview
This project implements a high-performance **Word Count** application to analyze the trade-offs between **Parallelism** and **Synchronization Overhead** across different CPU architectures.

The goal is not just to count words, but to stress-test how different processors (Laptop, Desktop, Low-power) handle thread creation, context switching, and memory contention (locking) under various workloads.

## 🚀 Key Features
The benchmark compares three implementations:
1.  **`lwords` (Baseline):** Single-threaded linked list implementation.
2.  **`pwords_v1` (High Contention):** Multi-threaded with **Fine-grained Locking** (Global Mutex per word insertion). Designed to measure synchronization latency.
3.  **`pwords_v2` (Optimized):** Multi-threaded with **Local Aggregation** (Thread-local storage + Single merge). Designed to measure raw multi-core throughput.

## 🧪 Experimental Design
The benchmark evaluates performance on three distinct datasets (`~62.5` each) to simulate different real-world scenarios:

| Dataset | Description | Purpose |
| :--- | :--- | :--- |
| **Set A** | 4 large files (15.6MB each) | **CPU Bound:** Tests raw compute power & turbo clock speeds. |
| **Set B** | 100 small files (625KB each) | **Overhead Bound:** Tests OS scheduler & thread creation cost. |
| **Set C** | Mixed sizes (100KB - 10MB) | **Load Imbalance:** Tests load balancing & hybrid architecture efficiency (P-core vs E-core). |

## 💻 Hardware Tested
This project ranks performance across three different CPU architectures:

| Segment | CPU Model | Cores/Threads | Architecture |
| :--- | :--- | :--- | :--- |
| **Desktop** | Intel Core Ultra 9 [Model] | [XX] / [XX] | **Hybrid** (Arrow Lake) - High Performance |
| **Laptop** | AMD Ryzen 9 7940HS | 8 / 16 | **Traditional** - High Efficiency |
| **Server/Edge** | Intel Pentium Silver J5040 | 4 / 4 | **Low Power** - Entry Level |

## 📊 How to Run
The project includes an automated script that compiles the C code, verifies correctness, and runs the full benchmark suite.

### Prerequisites
* GCC Compiler
* Python 3 
* Make

### One-Click Benchmark
Run the following command to compile, test logic, and generate performance reports:

```bash
chmod +x run_all.sh
./run_all.sh

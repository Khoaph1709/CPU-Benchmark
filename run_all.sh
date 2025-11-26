#!/bin/bash

echo "=== BẮT ĐẦU QUY TRÌNH KIỂM THỬ VÀ BENCHMARK ==="

# --- BƯỚC 1: BIÊN DỊCH ---
echo "[1/4] Đang biên dịch mã nguồn C..."
make clean > /dev/null 2>&1
make > /dev/null

if [ ! -f "./pwords_v1" ] || [ ! -f "./pwords_v2" ] || [ ! -f "./lwords" ]; then
    echo "LỖI: Compile thất bại. Thiếu file thực thi (lwords/pwords_v1/pwords_v2)."
    echo "Hãy kiểm tra lại Makefile."
    exit 1
fi
echo "   -> Compile thành công."

# --- BƯỚC 2: TIỀN KIỂM TRA (Pre-check) ---
echo "[2/4] Đang chạy kiểm tra Logic và Tốc độ cơ bản (grade.py)..."

if [ ! -d "gutenberg" ]; then
    echo "   -> Cảnh báo: Không tìm thấy thư mục 'gutenberg'. grade.py có thể bị lỗi."
fi

python3 grade.py

# Kiểm tra Exit Code ($?) của lệnh python vừa chạy
if [ $? -ne 0 ]; then
    echo ""
    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    echo "LỖI NGHIÊM TRỌNG: Chương trình không vượt qua bài kiểm tra của grade.py"
    echo "Lý do có thể: Output sai (Correctness) hoặc Tốc độ không đạt."
    echo "Quá trình Benchmark bị hủy bỏ."
    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    exit 1
else
    echo "   -> Logic OK. Tốc độ cơ bản OK. Chuyển sang Benchmark chi tiết."
fi

# --- BƯỚC 3: KIỂM TRA DATASET BENCHMARK ---
echo "[3/4] Đang kiểm tra dữ liệu Benchmark (Set A, B, C)..."
if [ ! -d "dataset/set_A" ] || [ ! -d "dataset/set_C" ]; then
    echo "   -> Cảnh báo: Thư mục 'dataset' có vẻ thiếu. Benchmark có thể không chạy đủ 3 set."
fi

# --- BƯỚC 4: CHẠY BENCHMARK CHI TIẾT ---
echo "[4/4] Đang chạy Benchmark Full (Set A, B, C)..."
echo "Quá trình này mất khoảng 1-3 phút tùy cấu hình máy..."

if [ -f "benchmark_final.py" ]; then
    python3 benchmark_final.py
elif [ -f "benchmark-full.py" ]; then
    python3 benchmark-full.py
elif [ -f "benchmark.py" ]; then
    python3 benchmark.py
else
    echo "LỖI: Không tìm thấy file script benchmark python nào (benchmark_final.py)."
    exit 1
fi

echo "=== HOÀN TẤT TOÀN BỘ QUY TRÌNH ==="
echo "Kết quả chi tiết đã được lưu trong file .json."
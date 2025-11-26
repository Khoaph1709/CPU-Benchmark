import random
import os

def create_mixed_dataset(source_file, output_dir):
    if not os.path.exists(source_file):
        print(f"Lỗi: Không tìm thấy file nguồn {source_file}")
        return

    print(f"Đang đọc dữ liệu từ {source_file}...")
    with open(source_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    total_len = len(content)
    print(f"Tổng kích thước: {total_len} ký tự.")

    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    current_pos = 0
    file_idx = 0
    
    MIN_CHUNK = 100 * 1024      
    MAX_CHUNK = 5 * 1024 * 1024 

    while current_pos < total_len:
        chunk_size = random.randint(MIN_CHUNK, MAX_CHUNK)
        
        end_pos = current_pos + chunk_size
        if end_pos > total_len:
            end_pos = total_len
            
        chunk_data = content[current_pos:end_pos]
        
        output_filename = os.path.join(output_dir, f"part_{file_idx:03d}.txt")
        with open(output_filename, 'w', encoding='utf-8') as out_f:
            out_f.write(chunk_data)
            
        print(f"Đã tạo: {output_filename} ({len(chunk_data)} bytes)")
        
        current_pos = end_pos
        file_idx += 1

    print(f"\nHoàn tất! Đã chia thành {file_idx} files trong thư mục {output_dir}")

if __name__ == "__main__":
    create_mixed_dataset("dataset/source.txt", "dataset/set_C")
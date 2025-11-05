import os

def count_nonempty_lines_in_cpp():
    total_lines = 0
    current_dir = os.path.dirname(os.path.abspath(__file__))

    for filename in os.listdir(current_dir):
        if filename.endswith(".cpp") or filename.endswith(".h"):
            filepath = os.path.join(current_dir, filename)
            with open(filepath, "r", encoding="utf-8", errors="ignore") as file:
                for line in file:
                    if line.strip():  # строка не пустая
                        total_lines += 1
    return total_lines


if __name__ == "__main__":
    total = count_nonempty_lines_in_cpp()
    print(f"Суммарное количество непустых строк во всех .cpp файлах: {total}")

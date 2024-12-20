import os
import sys
import multiprocessing
import time

def convert_command_to_python(command):
    # Удаляем пробелы в начале и конце команды
    command = command.strip()
    
    # Пропускаем пустые строки и строки с комментариями
    if not command or command.startswith('@echo off') or command.startswith('::'):
        return None
    
    # Обработка команды echo
    if command.startswith('echo'):
        return f'print("{command[5:].strip()}")'
    
    # Обработка других команд (простой вызов через os.system)
    return f'os.system("{command}")'

def convert_batch_to_python(batch_file_path, python_file_path):
    with open(batch_file_path, 'r', encoding='utf-8') as batch_file:
        batch_commands = batch_file.readlines()
    
    python_commands = ['import os\n\n']
    
    for command in batch_commands:
        python_command = convert_command_to_python(command)
        if python_command:
            python_commands.append(python_command + '\n')
    
    with open(python_file_path, 'w', encoding='utf-8') as python_file:
        python_file.writelines(python_commands)

if __name__ == "__main__":
    # Получаем количество доступных ядер процессора
    cpu_count = multiprocessing.cpu_count()
    
    if len(sys.argv) != 2:
        print("Usage / Использование: mconvert <filename / имя файла>")
        sys.exit(1)

    name = sys.argv[1]
    batch_file_path = name
    python_file_path = f'{name}-converted.py'

    convert_batch_to_python(batch_file_path, python_file_path)
    print(f"Conversion completed. Result saved in {python_file_path} / Результат сохранен в {python_file_path}")

    print("Creating .exe (compiling) with MBuilder / Создание .exe (компиляция) с MBuilder")
    
    os.system("pip install nuitka >NUL")
    os.system(f"nuitka --jobs={cpu_count} --lto=yes --mingw64 --follow-imports --standalone --onefile " + python_file_path)

    time.sleep(2)

    os.system("cls")
    print("Done! / Готово!")
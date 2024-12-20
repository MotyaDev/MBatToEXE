import os

file = input("Enter the file name: ")
export = input("Enter the export name (without .exe): ")

export = export + ".exe"

os.system(f"start gcc/bin/gcc {file} -o {export}")

print(f"Compilation successful. Executable saved in {export}")
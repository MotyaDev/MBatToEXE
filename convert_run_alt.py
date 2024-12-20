import os

files = input("Enter the .bat or .cmd name (with extension): ")

os.system("python convert_alt.py " + files)
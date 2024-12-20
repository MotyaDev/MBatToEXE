import os

files = input("Enter the .bat or .cmd name (with extension): ")

os.system("convert.exe " + files)
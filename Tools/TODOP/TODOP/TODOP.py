import os
import sys
import fileinput
from github import Github

#important stuff here
GITTOKEN = "a8193562d0023da3548295f4b3f2a7b5dde76d94"
REPO = "SushiSalad/P3DPGE"

def find_files(dir_name, exts):
	filepaths = []
	for root, dirs, files in os.walk(dir_name):
		for file in files:
			for ext in exts:
				if file.endswith(ext):
					filepaths.append(os.path.join(root, file))
	return filepaths

def find_files_git():
	contents = repo.get_contents("")
	files = []
	while contents:
		file = contents.pop(0)
		if file.type == "dir":
			contents.extend(repo.get_contents(file.path))
		else:
			files.append(file)
	return files;
			

def main(src_dir, local):

	g = Github(GITTOKEN)


	filePaths = find_files(src_dir, ['.cpp', '.h'])
	filePathsGit = find_files_git()
	
	TODOs = []
	TODOList = open('TODOs.txt', "w")

	#get TODOs and their data
	for filePath in filePaths:
		file = open(filePath, 'r+')
		line_num = 0
		for line in file:
			if "TODO(" in line:
				TODO = line[line.find("TODO("):]
				arguments = TODO[TODO.find("(") + 1:TODO.find(")")].split(",")
				body = TODO[TODO.find(":") + 1:]
				TODOs.append((line_num, arguments, body))

			line_num += 1

	Tags = []

	for line_num, arguments, body in TODOs:
		
		arg = arguments[0]

		if '+' in arg and "-" in arg:
			Tags.append("Invalid")
		elif "+" in arg:
			Tags.append("GitIssue")
		if "s" in arg and "u" in arg:
			Tags.append("Invalid")
		else:
			if "s" in arg:
				Tags.append("Severe")
		if "p" in arg:
			Tags.append("Physics")
		if "r" in arg:
			Tags.append("Render")
		if "e" in arg:
			Tags.append("Entity")
		if "i" in arg:
			Tags.append("Input")
		if "m" in arg:
			Tags.append("Math")

	

	repo = g.get_repo(REPO)

	
	

	


	
	

if __name__ == "__main__":
	useage = "\nUseage:\n    python TODOP.py src_folder_path \n    Eg: python TODOP.py "
	if len(sys.argv) < 2:
		print(useage)
		exit()
	if len(sys.argv) > 2:
		src_dir = sys.argv[1]
	main(src_dir)

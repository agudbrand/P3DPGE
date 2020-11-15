import os
import sys
import fileinput
from github import Github
import github
from flask import Flask, request, Response
import json

#events are received 5 minutes after they occur

#important stuff here
GITTOKEN = "320f32fa7ea8b537c83578d76d77d03bcb66671f"
REPO = "SushiSalad/P3DPGE"
APP_ID = 88527

app = Flask(__name__)



@app.route('/webhook', methods=['POST'])
def respond():
	payload = request.get_json()
	print(payload)
	return Response(status=200)

@app.route('/', methods=['GET'])
def hello_world():
	return "TODOP time"

def find_files(dir_name, exts):
	filepaths = []
	
	for root, dirs, files in os.walk(dir_name):
		for file in files:
			for ext in exts:
				if file.endswith(ext):
					filepaths.append(os.path.join(root, file))
	return filepaths

def find_files_git(repo):
	contents = repo.get_contents("")
	files = []
	while contents:
		file = contents.pop(0)
		if file.type == "dir":
			contents.extend(repo.get_contents(file.path))
		else:
			files.append(file)
	return files;

def getTODOs(file):
	TODOs = []
	line_num = 0
	for line in file:
		if "TODO(" in line:
			TODO = line[line.find("TODO("):]
			arguments = TODO[TODO.find("(") + 1:TODO.find(")")].split(",")
			body = TODO[TODO.find(")") + 1:]
			TODOs.append((file.name, line_num, arguments, body))
		line_num += 1
	return TODOs
	
			

def main(src_dir):

	#do this shit later I guess
	#i = 0
	#while os.path.exists("logs/payload%s.json" % i):
	#	i += 1	
	#with open("payload%s.json" % i, 'w') as logfile:
	#	json.dump(payload, logfile)

	#g = Github(GITTOKEN)
	#repo = g.get_repo(REPO)

	#Events = repo.get_events()
	#PushEvents = []

	#for event in Events:
	#	if event.type == "PushEvent":
	#		PushEvents.append(event)

	#print(PushEvents[0].payload)

	TODOs = []
	with open("TODOs.txt", "w") as TODOList:
		#get TODOs and their data
		#if git_check:
		#	filePathsGit = find_files_git(repo)
		#	for filePath in filePathsGit:
		#		file = open(filePath, "r+")
		#		TODOs.extend(getTODOs(file))
		#else:
		filePaths = find_files(src_dir, ['.cpp', '.h'])
		for filePath in filePaths:
			file = open(filePath, 'r+')
			TODOs.extend(getTODOs(file))
		print(filePaths)

		#make room for listing number of TODOs later
		TODOList.write("TODO List successfully generated with " + str(len(TODOs)) + " TODOs found.\n\n")

		TODO_num = 0
		for file_name, line_num, arguments, body in TODOs:
		
			Tags = []

			#this looks so FUCKING bad

			#get tags
			argument = arguments[0]
			for arg in argument:
				if "+" in arg:
					Tags.append("GitIssue")
				if "s" in arg and "u" in arg:
					Tags.append("CHECK_TAGS")
				else:
					if "s" in arg:
						Tags.append("Severe")
					if "u" in arg:
						Tags.append("Unimportant")
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
				if "o" in arg:
					Tags.append("Optimization")
				if "g" in arg:
					Tags.append("General")
				if "c" in arg:
					Tags.append("Clean-Code")
			if len(Tags) == 0:
				Tags.append("No Tags")

			#start writing everything
			creator = arguments[1]
			TODOList.write("~~~~~~~ TODO " + str(TODO_num) + " ~~~~~~~")
			
			TODOList.write("\n")
			if len(arguments) == 4:
				title = arguments[3]
				TODOList.write("Title: " + title)
				TODOList.write("\n")

			TODOList.write("in file " + file_name[file_name.rfind("\\") + 1:] + " at line " + str(line_num))
			TODOList.write("\n")
			TODOList.write("created by: " + creator)
			if len(arguments) == 5:
				TODOList.write(" and assigned to " + arguments[4])
			TODOList.write("\n")
			TODOList.write("----------------------")
			TODOList.write("\n")

			if len(arguments) == 3:
				date = arguments[2]
				TODOList.write("Date: " + date)
				TODOList.write("\n")

			TODOList.write("Tags: ")
			for i, tag in enumerate(Tags):
				TODOList.write(tag)
				if i != len(Tags) - 1:
					TODOList.write(", ")

			TODOList.write("\n")
			TODOList.write("----------------------\n")

			if body[0] == " ":
				TODOList.write(body[1:])
			else:
				TODOList.write(body)

			TODOList.write("\n\n")
			TODOList.write("~~~~~~~ TODO " + str(TODO_num) + " ~~~~~~~")
			TODOList.write("\n\n\n")
			TODO_num += 1

			





	

if __name__ == "__main__":
	useage = "\nUseage:\n    python TODOP.py src_folder_path \n    Eg: python TODOP.py "
	if len(sys.argv) < 1:
		print(useage)
		exit()
	if len(sys.argv) > 1:
		src_dir = sys.argv[1]
	#	git_check = sys.argv[2]

	main(src_dir)
	#app.run(host="0.0.0.0", port = 80, threaded = True, debug = False)
	
	

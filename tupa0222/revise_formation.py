# -*- coding: utf-8 -*-
# @Date    : 2019-01-26 19:28:16
# @Author  : Weimin
# @Python  : 3.6
# Revise formation of file name (Remove *_0.xml --> *.xml) and PassageID ("*_0" --> "*")

import os,re
from tempfile import mkstemp
from shutil import move
from os import fdopen, remove

def replace(file_path, pattern, subst):
    #Create temp file
    fh, abs_path = mkstemp()
    with fdopen(fh,'w') as new_file:
        with open(file_path) as old_file:
        	# print(type(old_file))
            for line in old_file:
                # print(re.sub(pattern, subst + r'\1', line))
                # p = re.compile(r'%s'%pattern)
                # print(p.sub(subst+p.group(1)+'\"', line))
                # new_file.write(line.replace(pattern, subst))
                new_file.write(re.sub(pattern, subst + r'\1"', line))

    #Remove original file
    remove(file_path)
    #Move new file
    move(abs_path, file_path)


input_path = '/Users/insisterlouis/Workspaces/Courses_HW/2018.09NLP/data/output/out_TUPA_M+B_0225/'


if os.path.exists(input_path):
	for file in os.listdir(input_path):
		print(file)
		# closed/open
		if os.path.isdir(os.path.join(input_path,file)):
			second_level = os.path.join(input_path,file)
			# UCCA blabla
			for second_file in os.listdir(second_level):
				print(second_file)
				if os.path.isdir(os.path.join(second_level,second_file)):
					third_level = os.path.join(second_level,second_file)
					for third_file in os.listdir(third_level):
						final_path = os.path.join(third_level,third_file)
						# Rename the file
						rename_path = re.sub('_0.xml','.xml',final_path)
						os.rename(final_path,rename_path)
						# change the passageID
						replace(rename_path, "passageID=\"(.*)_0\"", "passageID=\"")




# def renameFile(input_path):
#     fileList = os.listdir(input_path)
#     print(fileList)

#     # get current work path
#     currentpath = os.getcwd()
#     print("Current is "+currentpath)
#     # change current work path
#     os.chdir(r"C:\Users\dell\Desktop\Udacity\prank\prank")
#     for fileName in fileList:
#         print("Original is " + fileName)
#         # delete 0123456789 in file name
#         os.rename(fileName, fileName.translate(None, "0123456789"))
#         print("Changed is " + fileName.translate(None, "0123456789"))
#     os.chdir(currentpath)
# renameFile()



# path = "C://Python34//"
# for file in os.listdir(path):
#     if os.path.isfile(os.path.join(path,file))==True:
#         if file.find('.')<0:
#             newname=file+'.jpg'

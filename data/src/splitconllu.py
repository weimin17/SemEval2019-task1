# -*- coding: utf-8 -*-
# @Date    : 2019-01-30 17:57:07
# @Author  : Weimin
# @Python  : 2.7/3.6
# 

import os, re



input_path = '../ori_data/english_wiki_en2_test_filename.txt'
file_index = []
with open(input_path,'rb') as f:
	for file in f.readlines():
		file = re.split('.txt',file.decode('utf8'))[0]
		file_index.append(file)
print(len(file_index))
f.close()


input_path = '../temp/train.conllx'
with open(input_path, 'rb') as f:
	content = (f.read().decode('utf8'))
	splitcon = re.split('\n\n', content)
	print(len(splitcon))
	# print(splitcon)
		# print(file.decode('utf8'))

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

# output_file = open('../train_dev_data/english_wiki_en2_test.clean','w')

# nlp = spacy.load('en')
# nlp.add_pipe(BeneparComponent("benepar_en2"))

# store_name = open('../train_dev_data/english_wiki_en2_test_filename.txt','w')
# for file in os.listdir(input_path):
# 	print(file)
# 	rename_path = re.sub('.txt','',file)
# 	print(rename_path)
	# store_name.write(file)
	# store_name.write('\n')
	# with open(os.path.join(input_path,file),'rb') as f:
	# 	sentences = f.read().decode('utf8')[:-1]

	# 	f.close()
	# 	try:
	# 		doc = nlp(sentences)

	# 		for sent in list(doc.sents):
	# 			trees = sent._.parse_string
	# 			output_file.write(trees)
	# 	except Exception as e:
	# 		output_file.write('\n')

	# 	output_file.write('\n')

# output_file.close()



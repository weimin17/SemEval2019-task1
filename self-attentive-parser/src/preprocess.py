# -*- coding: utf-8 -*-
# @Date    : 2018-12-26 15:53:53
# @Author  : Weimin
# @Python  : 3.6
# 

# import os
# import benepar
# from itertools import chain


# input_path = '../data/train-txt/UCCA_English-Wiki'

# output_file = open('../data/train.clean','w')

# for keys, file in enumerate(os.listdir(input_path)[0:3]):
# 	if keys == 0:
# 		with open(os.path.join(input_path,file),'rb') as f:
# 			sentences = f.read().decode('utf8')
# 			f.close()
# 			parser = benepar.Parser("benepar_en")
# 			trees = parser.parse_sents(sentences)
# 			# trees = parser.parse(sentences)
# 	else:
# 		with open(os.path.join(input_path,file),'rb') as f:
# 			sentences = f.read().decode('utf8')
# 			f.close()
# 			parser = benepar.Parser("benepar_en")
# 			tree = parser.parse_sents(sentences)
# 			# tree = parser.parse(sentences)
# 			trees = chain(trees, tree)
# print(trees)

# # for generator_one in trees:
# # 	for x in generator_one:
# # 	    output_file.write(str(x))
# # 	output_file.write('\n')
# # output_file.close()
# for x in trees:
# 	output_file.write(str(x))
# output_file.close()





import os
import benepar
import spacy
from benepar.spacy_plugin import BeneparComponent

def ben2constituency(input_path, output_file,store_name,nlp):
	for file in os.listdir(input_path):
		# print(file)
		store_name.write(file)
		store_name.write('\n')
		with open(os.path.join(input_path,file),'rb') as f:
			sentences = f.read().decode('utf8')[:-1]
	
			f.close()
			try:
				doc = nlp(sentences)
				# number of trees in one files
				tree_count = len(list(doc.sents))
				if tree_count == 1:
					for sent in list(doc.sents):
						trees = sent._.parse_string
						output_file.write(trees)

				else:
					output_file.write('(')
					for sent in list(doc.sents):
						trees = sent._.parse_string
						output_file.write(trees)
					output_file.write(')')	
				# elif tree_count == 2:
				# 	print('2---' + file)
				# 	output_file.write('(')
				# 	for sent in list(doc.sents):
				# 		trees = sent._.parse_string
				# 		output_file.write(trees)
				# 	output_file.write(')')
	
				# elif tree_count == 3:
				# 	print('3---' + file)
				# 	output_file.write('((')
				# 	for sent in list(doc.sents):
				# 		trees = sent._.parse_string
				# 		output_file.write(trees)
				# 	output_file.write('))')
				# elif tree_count == 4:
				# 	print('4---' + file)
				# 	output_file.write('(((')
				# 	for sent in list(doc.sents):
				# 		trees = sent._.parse_string
				# 		output_file.write(trees)
				# 	output_file.write(')))')
				# elif tree_count == 5:
				# 	print('5---' + file)
				# 	output_file.write('((((')
				# 	for sent in list(doc.sents):
				# 		trees = sent._.parse_string
				# 		output_file.write(trees)
				# 	output_file.write('))))')
	
				# elif tree_count == 6:
				# 	print('6---' + file)
				# 	output_file.write('(((((')
				# 	for sent in list(doc.sents):
				# 		trees = sent._.parse_string
				# 		output_file.write(trees)
				# 	output_file.write(')))))')
	
				# elif tree_count > 6:
				# 	print('777777777777777777' + file)
				# 	output_file.write('((((((((((((')
				# 	for sent in list(doc.sents):
				# 		trees = sent._.parse_string
				# 		output_file.write(trees)
				# 	output_file.write('))))))))))))')
	
			except Exception as e:
				# raise e
				print('oooooooooooooooooooooooo' + file)
				output_file.write('\n')
	
			output_file.write('\n')
		
	output_file.close()
	store_name.close()
#Test Dataset
# ######################################################################
# input_path = '../ori_data/test_data/test-txt/UCCA_English-Wiki'
# output_file = open('../ori_data/english_wiki_en2_test.clean','w')

# nlp = spacy.load('en')
# nlp.add_pipe(BeneparComponent("benepar_en2"))

# store_name = open('../ori_data/english_wiki_en2_test_filename.txt','w')

# ben2constituency(input_path, output_file,store_name,nlp)
# ######################################################################
# input_path = '../ori_data/test_data/test-txt/UCCA_English-20K'
# output_file = open('../ori_data/english_20k_en2_test.clean','w')

# nlp = spacy.load('en')
# nlp.add_pipe(BeneparComponent("benepar_en2"))

# store_name = open('../ori_data/english_20k_en2_test_filename.txt','w')

# ben2constituency(input_path, output_file,store_name,nlp)


# ######################################################################

# input_path = '../test_data/test-txt/UCCA_French-20K'
# output_file = open('../train_dev_data/french_20k_test.clean','w')

# nlp = spacy.load('fr')
# nlp.add_pipe(BeneparComponent("benepar_fr"))
# store_name = open('../train_dev_data/french_20k_test_filename.txt','w')

# ben2constituency(input_path, output_file,store_name,nlp)

# ######################################################################

# input_path = '../test_data/test-txt/UCCA_German-20K'
# output_file = open('../train_dev_data/german_20k_test.clean','w')

# nlp = spacy.load('de')
# nlp.add_pipe(BeneparComponent("benepar_de"))

# store_name = open('../train_dev_data/german_20k_test_filename.txt','w')

# ben2constituency(input_path, output_file,store_name,nlp)


######################################################################
######################################################################
######################################################################
######################################################################
######################################################################
#Dev Dataset
# ######################################################################
# input_path = '../ori_data/train_dev_data/dev-txt/UCCA_English-Wiki'
# output_file = open('../ori_data/english_wiki_en2_dev.clean','w')

# nlp = spacy.load('en')
# nlp.add_pipe(BeneparComponent("benepar_en2"))

# store_name = open('../ori_data/english_wiki_en2_dev_filename.txt','w')

# ben2constituency(input_path, output_file,store_name,nlp)




# input_path = '../data/dev-txt/UCCA_English-Wiki'
# output_file = open('../data/dev.clean','w')

# for file in os.listdir(input_path):
# 	print(file)
# 	with open(os.path.join(input_path,file),'rb') as f:
# 		sentences = f.read().decode('utf8')
# 		f.close()
# 		nlp = spacy.load('en')
# 		nlp.add_pipe(BeneparComponent("benepar_en"))
# 		doc = nlp(sentences)
# 		for sent in list(doc.sents):
# 			trees = sent._.parse_string
# 			output_file.write(trees)
# 			output_file.write('\n')


# output_file.close()

# single
######################################################################
input_path = '../ori_data/test_data/test-txt/aaa'
output_file = open('../ori_data/aaa.clean','w')

nlp = spacy.load('en')
nlp.add_pipe(BeneparComponent("benepar_en2"))

store_name = open('../ori_data/aaa.txt','w')

ben2constituency(input_path, output_file,store_name,nlp)




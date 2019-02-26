# -*- coding: utf-8 -*-
# @Date    : 2018-12-26 15:53:53
# @Author  : Weimin
# @Python  : 3.6
# 


import os
import benepar
import spacy
from benepar.spacy_plugin import BeneparComponent

input_path = '../data/test-txt/UCCA_French-20K'
output_file = open('../data/french_20K_test.clean','w')

for file in os.listdir(input_path):
	print(file)
	with open(os.path.join(input_path,file),'rb') as f:
		sentences = f.read().decode('utf8')[:-1]
		print("Context::::::::::::::::::::::::")
		print(sentences)
		f.close()
		nlp = spacy.load('fr')
		nlp.add_pipe(BeneparComponent("benepar_fr"))
		doc = nlp(sentences)
		print('Sentences:::::::::::::::::::::::')
		for sent in list(doc.sents):
			trees = sent._.parse_string
			output_file.write(trees)
			print(trees)
			output_file.write('\n')

output_file.close()








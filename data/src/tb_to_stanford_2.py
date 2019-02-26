# -*- coding:utf-8 -*-
# Filename: ptb-to-stanford.py
# Author：hankcs
# Date: 2017-11-22 12:26

import argparse
import tempfile
import re, os
from os import system, remove, path
from os.path import join

from utility import make_sure_path_exists


def convert(src, dst, lang):
    if path.isfile(dst):
        remove(dst)
    java = 'edu.stanford.nlp.trees.EnglishGrammaticalStructure' if lang == 'en' \
        else 'edu.stanford.nlp.trees.international.pennchinese.ChineseGrammaticalStructure'
    print('Generating {}...'.format(dst))
    with open(src) as src:
        lines = src.readlines()
        for n, line in enumerate(lines):
            if n % 10 == 0 or n == len(lines) - 1:
                print("%c%.2f%%" % (13, (n + 1) / float(len(lines)) * 100), end='')
            if len(line.strip()) == 0:
                continue
            file = tempfile.NamedTemporaryFile()
            tmp = file.name
            with open(tmp, 'w') as out:
                out.write(line)
            system(
                'java -cp "*" -mx1g {} -basic -keepPunct -conllx '
                '-treeFile "{}" >> "{}"'.format(java,
                                                tmp, dst))
            # print(line)
    print()
def readname(input_path):
    file_index = []
    with open(input_path,'rb') as f:
        for file in f.readlines():
            file = re.split('.txt',file.decode('utf8'))[0]
            file_index.append(file)
    print(len(file_index))
    f.close()
    return file_index

def splitsingleline(consti_input_path, consti_output_path, file_name):
    #Split constituency file into single line files.
    with open(consti_input_path) as old_file:
        lines = old_file.readlines()

        for n, line in enumerate(lines):
            with open(os.path.join(consti_output_path, file_name[n]), 'w') as out:
                out.write(line)
            out.close()
        # for line in old_file:
        #     new_file.write(re.sub(pattern, subst + r'\1"', line))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Convert combined Penn Treebank files (.txt) to Stanford Dependency format (.conllx)')
    parser.add_argument("--input", required=True,
                        help='The folder containing train.txt/dev.txt/test.txt in bracketed format')
    parser.add_argument("--lang", required=True, help='Which language? Use en for English, cn for Chinese')
    parser.add_argument("--output", required=True, dest="output",
                        help='The folder where to store the output train.conllx/dev.conllx/test.conllx in Stanford '
                             'Dependency format')
    parser.add_argument("--filename", required=True, help='which file stores file name')
    parser.add_argument("--constifile", required=True, help='constituency file path')

    args = parser.parse_args()
    make_sure_path_exists(args.output)
    
    input_path = args.filename
    # '../ori_data/english_wiki_en2_test_filename.txt'
    file_name = readname(input_path)
    # constit_path = '../ori_data/english_wiki_en2_test.clean'

    splitsingleline(args.constifile, args.input, file_name)

    # for f in ['train', 'dev', 'test']:
    for f in file_name:
        convert(join(args.input, f), join(args.output, f + '.conllu'), args.lang)
    # python tb_to_stanford_2.py --input ../../temp/UCCA_English-Wiki_consti/ --lang en --output ../../temp/UCCA_English-Wiki --filename '../ori_data/english_wiki_en2_test_filename.txt' --constifile '../ori_data/english_wiki_en2_test.clean'
##########################################################################
python tb_to_stanford_2.py --input ../../temp/UCCA_English-Wiki_consti/ --lang en --output ../../temp/UCCA_English-Wiki --filename '../../ori_data/english_wiki_en2_test_filename.txt' --constifile '../../ori_data/english_wiki_en2_test.clean'


for filename in ../../temp/UCCA_English-Wiki/* ; do
	echo $filename
	python -m semstr.convert $filename -o ../../temp/output_Benepar_0217/UCCA_English-Wiki 

done

##########################################################################
python tb_to_stanford_2.py --input ../../temp/UCCA_English-20K_consti/ --lang en --output ../../temp/UCCA_English-20K --filename '../../ori_data/english_20k_en2_test_filename.txt' --constifile '../../ori_data/english_20k_en2_test.clean'

for filename in ../../temp/UCCA_English-20K/* ; do
	echo $filename 
	python -m semstr.convert $filename -o ../../temp/output_Benepar_0217/UCCA_English-20K 
done





# python tb_to_stanford_2.py --input ../../temp/UCCA_German-20K_consti/ --lang en --output ../../temp/UCCA_German-20K --filename '../../ori_data/english_wiki_en2_test_filename.txt' --constifile '../../ori_data/english_wiki_en2_test.clean'

# for filename in ../../temp/UCCA_German-20K/* ; do
# 	echo $filename
# 	python -m semstr.convert $filename -o ../../temp/output/UCCA_German-20K 
# done




# python tb_to_stanford_2.py --input ../../temp/UCCA_French-20K_consti/ --lang en --output ../../temp/UCCA_French-20K --filename '../../ori_data/english_wiki_en2_test_filename.txt' --constifile '../../ori_data/english_wiki_en2_test.clean'

# for filename in ../../temp/UCCA_French-20K/* ; do
# 	echo $filename
# 	python -m semstr.convert $filename -o ../../temp/output/UCCA_French-20K 
# done

#################################
# Dev

##########################################################################
python tb_to_stanford_2.py --input ../../temp_dev/UCCA_English-Wiki_consti/ --lang en --output ../../temp_dev/UCCA_English-Wiki --filename '../../ori_data/english_wiki_en2_dev_filename.txt' --constifile '../../ori_data/english_wiki_en2_dev.clean'


for filename in ../../temp_dev/UCCA_English-Wiki/* ; do
	echo $filename
	python -m semstr.convert $filename -o ../../temp_dev/output_Benepar_0217/UCCA_English-Wiki 

done




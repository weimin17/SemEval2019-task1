# SemEval2019-Taks1: Cross-lingual Semantic Parsing with UCCA


This Project describes the systems of the CUNY-PKU team in “[SemEval 2019 Task 1: Cross-lingual Semantic Parsing with UCCA](https://www.aclweb.org/anthology/S19-2012v2.pdf)”. The projects are finished by Weimin Lyu, Sheng Huang, Abdul Rafae Khan, Shengqiang Zhang, Weiwei Sun, and Jia Xu. We introduce a novel model by applying a cascaded MLP and BiLSTM model. Then, we ensemble multiple system-outputs by reparsing. In particular, we introduce a new decoding al- gorithm for building the UCCA represen- tation. Our system won the first place in one track (French-20K-Open), second places in four tracks (English-Wiki-Open, English-20K-Open, German-20K-Open, and German-20K-Closed), and third place in one track (English-20K-Closed), among all seven tracks.


In the post-evaluation stage, our improved systems are ranked first in three tracks ( French-20K-Open, English-20K-Open and English-Wiki-Open) and second in the other four tracks.

If you make use of this software, please cite the following paper:

Weimin Lyu, Sheng Huang, Abdul Rafae Khan, Shengqiang Zhang, Weiwei Sun, and Jia Xu. 2019.CUNY-PKU parser at SemEval-2019 Task 1: Cross-lingual semantic parsing with UCCA. In Proc. of SemEval-2019.

More details including codes and results would be updated before June 7, 2019. 


# TUPA
This work is based on https://github.com/danielhers/tupa, you can set up the environment following the instructions through above link.

## Getting Data

We only use the data downloaded from the competition website (https://competitions.codalab.org/competitions/19160#participate) as training and developing. Please download the data and extract into folder `data/ori_data`.


## Closed Track

We finished three closed tracks: En-Wiki, En-20K, Ge-20K. For each track, we run TUPA-BiLSTM, TUPA-MLP, TUPA-MLP-BiLSTM models seperately, get three test outputs according to the three trained model. Finally, we use voting system to ensemble the three test outputs into one test outputs. We only use official training datasets to train. For example, in En-Wiki closed track:

1.Train TUPA-BiLSTM model

	CUDA_VISIBLE_DEVICES=0 python -m tupa -t  ../../data/ori_data/train_dev_data/train-xml/UCCA_English-Wiki -d  ../../data/ori_data/train_dev_data/dev-xml/UCCA_English-Wiki -c bilstm -m enwiki/enwiki-bilstm

Test Output1

	CUDA_VISIBLE_DEVICES=0 python -m tupa ../../data/ori_data/test_data/test-xml/UCCA_English-Wiki -m enwiki/enwiki-bilstm -o ../out_BiLSTM_enwiki/closed/UCCA_English-Wiki

2.Train TUPA-MLP model

	CUDA_VISIBLE_DEVICES=0 python -m tupa -t  ../../data/ori_data/train_dev_data/train-xml/UCCA_English-Wiki -d  ../../data/ori_data/train_dev_data/dev-xml/UCCA_English-Wiki -c mlp -m enwiki/enwiki-mlp

Test Output2

	CUDA_VISIBLE_DEVICES=0 python -m tupa ../../data/ori_data/test_data/test-xml/UCCA_English-Wiki -m enwiki/enwiki-mlp -o ../out_MLP_enwiki/closed/UCCA_English-Wiki

3.Train TUPA-MLP-BiLSTM model: First run BiLSTM model for 100 epohes according to the second step, then run MLP model for another 50 epoches. More specifically, use the trained TUPA-BiLSTM model in Step 1 as pre-trained model, change ("epoch": #) in file enwiki-bilstm.json to ("epoch": 0). Then run the following codes:

	CUDA_VISIBLE_DEVICES=0 python -m tupa -t  ../../data/ori_data/train_dev_data/train-xml/UCCA_English-Wiki -d  ../../data/ori_data/train_dev_data/dev-xml/UCCA_English-Wiki -c mlp -m enwiki/enwiki-bilstm

Test Output3

	CUDA_VISIBLE_DEVICES=0 python -m tupa ../../data/ori_data/test_data/test-xml/UCCA_English-Wiki -m enwiki/enwiki-bilstm -o ../out_MLP+BiLSTM_enwiki/closed/UCCA_English-Wiki


4. Use Voting System to ensemble the above three testing outputs into one testing outputs. Following instructions in `/voter1/usage.txt` to generate voting outputs.

For En-20K and Ge-20K, follow instructions in `tupa0222/models/en20K_closed.sh`, `tupa0222/models/ge_closed.sh`.


## Open Track

We also rerun the four open tracks: En-Wiki, En-20K, Ge-20K, Fr-20K. For each track, we only run TUPA-BiLSTM models and test on this model. Also, for open track, we only use training data in closed track without additional data. For example, in En-Wiki open track:

Train TUPA-BiLSTM model

	CUDA_VISIBLE_DEVICES=0 python -m tupa -t  ../../data/ori_data/train_dev_data/train-xml/UCCA_English-Wiki -d  ../../data/ori_data/train_dev_data/dev-xml/UCCA_English-Wiki -c bilstm -m enwiki/enwiki-bilstm

Test Outputs

	CUDA_VISIBLE_DEVICES=0 python -m tupa ../../data/ori_data/test_data/test-xml/UCCA_English-Wiki -m enwiki/enwiki-bilstm -o ../out_BiLSTM_enwiki/open/UCCA_English-wiki

For En-20K, Ge-20K and Fr-20K, following the instructions in `tupa0222/models/open.sh`




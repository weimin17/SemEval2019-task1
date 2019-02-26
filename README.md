# SemEval2019-Taks1
semEval taks1
This Project describes the systems of the CUNY-PKU team in “[SemEval 2019 Task 1: Cross-lingual Semantic Parsing with UCCA](https://competitions.codalab.org/competitions/19160)”. We introduce a novel model by applying a cascaded MLP and BiLSTM model. Then, we ensemble multiple system-outputs by reparsing. In particular, we introduce a new decoding al- gorithm for building the UCCA represen- tation. Our system won the first place in one track (French-20K-Open), second places in four tracks (English-Wiki-Open, English-20K-Open, German-20K-Open, and German-20K-Closed), and third place in one track (English-20K-Closed), among all seven tracks.


In the post-evaluation stage, our improved systems are ranked first in three tracks ( French-20K-Open, English-20K-Open and English-Wiki-Open) and second in the other four tracks.


\begin{table*}[t]
\centering
\begin{tabular}{|c|c|ccccc|}
\hline
\multicolumn{2}{|c|}{\textbf{Tracks}}& \textbf{MLP} & \textbf{BiLSTM} & \textbf{Cascaded BiLSTM} & \textbf{Ensembled} & \textbf{baseline} \\ \hline
\multirow{3}{*}{\textbf{closed}} & En-Wiki & 0.650 & \it 0.718 & 0.721 & \bf 0.728 & 0.728   \\
                                 & En-20K  & 0.617 & \it 0.669 & 0.673 & \bf 0.681 & 0.672   \\
                                 & Ge-20K  & 0.699 & \it 0.797 & \bf 0.797 & 0.797  & 0.731\\ \hline
\hline

\multirow{4}{*}{\textbf{open}}   & En-Wiki & 0.784 & \it 0.800 &  0.843 & \bf 0.846  & 0.735  \\
                                 & En-20K  & 0.715 & \it 0.739 &  0.764 & \bf 0.770 & 0.684   \\
                                 & Ge-20K  & 0.598 & \it 0.841 & \bf 0.841 & 0.840  & 0.791 \\
                                 & Fr-20K  & 0.535 & \it 0.796 & 0.795 & \bf 0.796  & 0.487 \\
\hline
\end{tabular}
\caption{F1 scores for both closed and open tracks in SemEval Task 1 2019 competition. The italic text represents our official submission in competition and the bold text represents our best F1 scores.}
\label{tab:mlp&bilstm}
\end{table*}

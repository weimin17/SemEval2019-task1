#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"
//#include "rapidxml/rapidxml_iterators.hpp"

#include "UCCA_Graph.hpp"
#include "Bi_UCCA_Graph.hpp"
#include "Chart_Score.hpp"
#include "Dynamic_Program.hpp"
#include "transform.hpp"

using namespace std;

string gold_data_dir = "../../../data/UCCA2019/train_dev/train-xml/UCCA_English-Wiki/";
string amr_data_dir = "../../../data/UCCA2019/ucca-amr/";
string sdp_data_dir = "../../../data/UCCA2019/ucca-sdp/";
string con_data_dir = "../../../data/UCCA2019/ucca-conllu/";

void test()
{
	//string data_dir = "../../../data/UCCA2019/train_dev/train-xml/UCCA_English-Wiki/";
	string data_dir = "data/";
	FILE *dir_ls = popen( ("ls "+data_dir).c_str(), "r");
	char file_name[110]={0};
	int num_graphs = 0, num_ip = 0, num_rm = 0, sum_ip = 0, sum_rm = 0, num_cs = 0, sum_cs = 0;
	int max_ip = 0, max_rm = 0, max_ep = 0, max_cs = 0;
	while( fgets( file_name, 100, dir_ls ) )
	{
		file_name[ strlen(file_name)-1 ] = 0;
		//if( strcmp( file_name, "1370001.xml")<0 ) continue;
		UCCA_Graph graph( (data_dir+file_name).c_str(), 1.0 );
		graph.Reorder();
		//graph.Print( stdout );
		//graph.OriginalOrder();
		Bi_UCCA_Graph bi_graph;
		TransformU2B( graph, bi_graph );
		//bi_graph.Print(stdout);
		Chart_Score score( bi_graph.root->right - 1 );
		Add2Score( bi_graph, 1.0, score );
		//score.Print( stdout );
		Dynamic_Program dp(score);
		//dp.Print( stdout );
		Bi_UCCA_Graph n_bi_graph;
		DP2BiGraph( dp, n_bi_graph );
		//n_bi_graph.Print( stdout );
		UCCA_Graph n_u_graph;
		TransformB2U( n_bi_graph, n_u_graph );
		n_u_graph.Print( stdout );
		n_u_graph.passage_id = graph.passage_id;
		n_u_graph.terminal_info = graph.terminal_info;
		n_u_graph.ExportXML( "output.xml" );
		/*
		FILE *f1 = fopen("graph1.txt","w"), *f2 = fopen("graph2.txt","w");
		graph.Print(f1);
		fclose(f1);
		n_u_graph.Print(f2);
		fclose(f2);
		if( !FileCmp() )
		{
			printf("ERROR\n");
			exit(0);
		}
		*/
		num_graphs += 1;
		if( graph.implicit_cnt )
		{
			num_ip += 1;
			sum_ip += graph.implicit_cnt;
		}
		if( graph.remote_cnt )
		{
			num_rm += 1;
			sum_rm += graph.remote_cnt;
		}
		if( graph.cross_cnt )
		{
			num_cs += 1;
			sum_cs += graph.cross_cnt;
		}
		max_ip = max( max_ip, graph.implicit_cnt );
		max_rm = max( max_rm, graph.remote_cnt );
		max_ep = max( max_ep, graph.implicit_cnt + graph.remote_cnt );
		max_cs = max( max_cs, graph.cross_cnt );
		break;
	}
	fclose(dir_ls);

	double n = num_graphs;
	printf("num graphs: %d\n", num_graphs);
	printf("implicit graphs: %d(%.2lf%%),  sum : %d(%.2lf), max : %d\n", num_ip, num_ip/n*100, sum_ip, sum_ip/n, max_ip);
	printf("remote graphs: %d(%.2lf%%),  sum : %d(%.2lf), max : %d(empty %d)\n", num_rm, num_rm/n*100, sum_rm, sum_rm/n, max_rm, max_ep);
	printf("cross graphs: %d(%.2lf%%),  sum : %d(%.2lf), max : %d\n", num_cs, num_cs/n*100, sum_cs, sum_cs/n, max_cs);
	return;
}

void TestOne( string path )
{
	UCCA_Graph graph( path.c_str(), 1.0 );
	graph.Reorder();
	//graph.Print( stdout );
	//graph.OriginalOrder();
	Bi_UCCA_Graph bi_graph;
	TransformU2B( graph, bi_graph );
	//bi_graph.Print(stdout);
	Chart_Score score( bi_graph.root->right - 1 );
	Add2Score( bi_graph, 1.0, score );
	//score.Print( stdout );
	Dynamic_Program dp(score);
	//dp.Print( stdout );
	Bi_UCCA_Graph n_bi_graph;
	DP2BiGraph( dp, n_bi_graph );
	//n_bi_graph.Print( stdout );
	UCCA_Graph n_u_graph;
	TransformB2U( n_bi_graph, n_u_graph );
	n_u_graph.Print( stdout );
	n_u_graph.passage_id = graph.passage_id;
	n_u_graph.terminal_info = graph.terminal_info;
	n_u_graph.ExportXML( "output.xml" );
	return;
}

void Merge( vector<string> file_list, vector<double> weight_list, string output_dir )
{
	UCCA_Graph pre( file_list.front().c_str(), 0 );
	pre.Reorder();
	Chart_Score score( pre.terminal_info.size()-1 );
	for( int i=0; i<file_list.size(); i++ )
	{
		printf("%s\n", file_list[i].c_str());
		span_error_flag = false;
		UCCA_Graph ug( file_list[i].c_str(), weight_list[i] );
		ug.Reorder();
		string ps_id = ug.passage_id;
		//printf("%s ", ps_id.c_str());
		//ug.Print(stdout);
		Bi_UCCA_Graph bg;
		TransformU2B( ug, bg );
		//bg.Print(stdout);
		Add2Score( bg, weight_list[i], score );
	}
	//printf("\n");
	//score.Print(stdout);
	Dynamic_Program dp(score);
	Bi_UCCA_Graph bg;
	DP2BiGraph( dp, bg );
	UCCA_Graph ug;
	TransformB2U( bg, ug );
	ug.passage_id = pre.passage_id;
	ug.terminal_info = pre.terminal_info;
	ug.ExportXML( (output_dir + ug.passage_id + ".xml").c_str() );
	return;
}

void MultipleMerge( vector<string> dir_list, vector<double> weight_list, string output_dir )
{
	vector< FILE* > ls_list;
	//FILE *dir_ls = popen( ("ls "+data_dir).c_str(), "r");
	for( auto it = dir_list.begin(); it != dir_list.end(); it++ )
		ls_list.push_back( popen( ("ls " + *it).c_str(), "r") );
	char file_name[110]={0};
	vector< string > file_list;
	while(1)
	{
		for( int i=0; i<dir_list.size(); i++ )
		{
			if( !fgets( file_name, 100, ls_list[i] ) )
				goto EndOfFile;
			file_name[ strlen(file_name)-1 ] = 0;
			file_list.push_back( dir_list[i] + file_name );
		}
		Merge( file_list, weight_list, output_dir );
		file_list.clear();
	}
	EndOfFile: ;
	for( auto it = ls_list.begin(); it != ls_list.end(); it++ )
		fclose( *it );
	return;
}



int main(int argc, char *argv[])
{
	vector<string> dir_list;
	vector<double> weight_list;
	for( int i=0; i+1<argc; i++ )
	{
		dir_list.push_back( string(argv[i+1]) );
		if( dir_list[i][ dir_list[i].size()-1 ] != '/' )
			dir_list[i] += '/';
		weight_list.push_back( 1.0 );
	}
	MultipleMerge( dir_list, weight_list, "output/");
	//MultipleMerge( {"input/sdp_dir/", "input/con_dir/", "input/amr_dir/"}, {1.0, 1.0, 1.0}, "output/" );
	return 0;
	//test();
	//return 0;
	//Merge( {"input/sdp_557006.xml", "input/con_557006.xml", "input/amr_557006_0.xml" }, {1.0, 1.0, 1.0}, "output/" );
	//Merge( {"input/sdp_557006.xml"}, {1.0, 1.0, 1.0} );
	//MultipleMerge( {"input/sdp_dir/", "input/con_dir/", "input/amr_dir/"}, {1.0, 1.0, 1.0}, "output/" );
	//MultipleMerge( {sdp_data_dir, con_data_dir, amr_data_dir}, {1.0, 1.0, 1.0}, "output_all/" );
	MultipleMerge( {sdp_data_dir, con_data_dir}, {1.0, 1.0, 1.0}, "output_all/" );
	//TestOne( "temp/131001_0.xml" );
	return 0;

	string cmd = "python3 -m semstr.evaluate";
	system( (cmd+" "+gold_data_dir+" "+con_data_dir).c_str() );
	//system("python3 -m semstr.evaluate evl/104000.xml evl/test.xml");
	return 0;
}




#pragma once
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "UCCA_Graph.hpp"
#include "Bi_UCCA_Graph.hpp"
#include "Chart_Score.hpp"
#include "Dynamic_Program.hpp"
using namespace std;

bool FileCmp()
{
	FILE *f1 = fopen("graph1.txt","r"), *f2 = fopen("graph2.txt","r");
	char x[110000], y[110000];
	while( !feof(f1) )
	{
		fgets( x, 100000, f1 );
		fgets( y, 100000, f2 );
		if( strcmp(x,y) || feof(f1)!=feof(f2) )
		{
			fclose(f1);
			fclose(f2);
			return false;
		}
	}
	fclose(f1);
	fclose(f2);
	return true;
}


string NodeU2B( UCCA_Graph::Node *u_node, Bi_UCCA_Graph::Node *b_node, int start,
	Bi_UCCA_Graph &bi_graph, unordered_map< UCCA_Graph::Node*, Bi_UCCA_Graph::InnerNodePoint > &node_map, int &dfs_num )
{
	if( start>0 && start==u_node->children.size()-1 )
	{
		u_node = u_node->children[start];
		start = 0;
		dfs_num = 0;
	}
	if( start==0 )
	{
		dfs_num += 1;
		node_map.insert( make_pair( u_node, Bi_UCCA_Graph::InnerNodePoint( b_node, dfs_num ) ) );
		//node_map[ u_node ] = Bi_UCCA_Graph::InnerNodePoint( b_node, dfs_num );
	}
	if( u_node->children.size()==0 )
	{
		if( u_node->is_implicit )
			return "i" + u_node->edge_tag;
		else if( u_node->is_mirror )
			return "m" + u_node->edge_tag;
		else if( u_node->left==0 )
			return "e" + u_node->edge_tag;
		else
			return u_node->edge_tag;
	}

	bool is_merge = ( start!=0 );
	string code = "";
	for( ; start<u_node->children.size() && ( u_node->children[start]->left==0 || u_node->children[start]->right == u_node->right ); start++ )
	{
		if( code!="" )
			code += "&";
		code += NodeU2B( u_node->children[start], b_node, 0, bi_graph, node_map, dfs_num );
	}
	if( start < u_node->children.size() )
	{
		UCCA_Graph::Node *child = u_node->children[start];
		Bi_UCCA_Graph::Node *x = new Bi_UCCA_Graph::Node( child->left, child->right, bi_graph.nodes.size()+1 );
		bi_graph.nodes.push_back( x );
		dfs_num = 0;
		b_node->children[0] = x;
		x->father = b_node;
		x->tag = NodeU2B( child, x, 0, bi_graph, node_map, dfs_num );

		Bi_UCCA_Graph::Node *y = new Bi_UCCA_Graph::Node( child->right, u_node->right, bi_graph.nodes.size()+1 );
		bi_graph.nodes.push_back( y );
		dfs_num = 0;
		b_node->children[1] = y;
		y->father = b_node;
		y->tag = NodeU2B( u_node, y, start+1, bi_graph, node_map, dfs_num );
	}

	if( is_merge )
		return "Merge";
	if( code!="" )
		code = "{" + code + "}";
	code = u_node->edge_tag + code;
	if( u_node->left==0 )
		code = "e" + code;
	return code;
}
void TransformU2B( const UCCA_Graph &u_graph, Bi_UCCA_Graph &bi_graph )
{
	/*
	unordered_set<string> tag_set;
	for( auto x = u_graph.nodes.begin(); x != u_graph.nodes.end(); x++ )
		tag_set.insert( (*x)->edge_tag );
	for( auto s = tag_set.begin(); s != tag_set.end(); s++ )
		cout << *s << " ";
	cout << endl;
	*/
	for(auto x = bi_graph.nodes.begin(); x != bi_graph.nodes.end(); x++ )
		delete *x;
	bi_graph.nodes.clear();
	unordered_map< UCCA_Graph::Node*, Bi_UCCA_Graph::InnerNodePoint > node_map;
	bi_graph.root = new Bi_UCCA_Graph::Node( u_graph.root->left, u_graph.root->right, 1 );
	bi_graph.nodes.push_back( bi_graph.root );
	int dfs_num = 0;
	bi_graph.root->tag = NodeU2B( u_graph.root, bi_graph.root, 0, bi_graph, node_map, dfs_num );

	bi_graph.remote_edges.clear();
	for( auto it = u_graph.remote_edges.begin(); it != u_graph.remote_edges.end(); it++ )
	{
		auto ix = node_map.find( it->first ), iy = node_map.find( it->second );
		bi_graph.remote_edges.push_back( make_pair( ix->second, iy->second ) );
	}
	bi_graph.mirror_edges.clear();
	for( auto it = u_graph.mirror_edges.begin(); it != u_graph.mirror_edges.end(); it++ )
	{
		auto ix = node_map.find( it->first ), iy = node_map.find( it->second );
		bi_graph.mirror_edges.push_back( make_pair( ix->second, iy->second ) );
	}
	return;
}

UCCA_Graph::Node* NodeB2U( Bi_UCCA_Graph::Node *b_node, int &pos, int &dfs_num, UCCA_Graph::Node *fth, UCCA_Graph &u_graph, 
	unordered_map< Bi_UCCA_Graph::InnerNodePoint, UCCA_Graph::Node* > &node_map )
{
	//printf("%s-%d [%d,%d)    pos:%d\n", b_node->tag.c_str(), b_node->id, b_node->left, b_node->right, pos);
	if( b_node->tag=="Merge" )
	{
		int p, d;
		NodeB2U( b_node->children[0], p=0, d=0, fth, u_graph, node_map );
		NodeB2U( b_node->children[1], p=0, d=0, fth, u_graph, node_map );
		return NULL;
	}

	UCCA_Graph::Node *last = NULL;
	while( pos < b_node->tag.size() )
	{
		UCCA_Graph::Node *cur = new UCCA_Graph::Node( to_string(u_graph.nodes.size()+1), "type" );
		u_graph.nodes.push_back( cur );
		dfs_num += 1;
		node_map[ Bi_UCCA_Graph::InnerNodePoint( b_node, dfs_num ) ] = cur;
		fth->children.push_back( cur );
		cur->father = fth;
		if( b_node->tag[pos]=='i' || b_node->tag[pos]=='m' || b_node->tag[pos]=='e' )
		{
			cur->left = cur->right = 0;
			if( b_node->tag[pos]=='i' )
				cur->is_implicit = true;
			else if( b_node->tag[pos]=='m' )
				cur->is_mirror = true;
			pos += 1;
		}
		else
		{
			last = cur;
			cur->left = b_node->left;
			cur->right = b_node->right;
		}

		for( ; pos < b_node->tag.size(); pos++ )
			if( isalpha( b_node->tag[pos] ) )
				cur->edge_tag += b_node->tag[pos];
			else 
			{
				if( b_node->tag[pos]=='{' )
				{
					pos += 1;
					UCCA_Graph::Node *get = NodeB2U( b_node, pos, dfs_num, cur, u_graph, node_map );
					if( get!=NULL )
						last = get;
				}
				if( pos >= b_node->tag.size() )
					break;
				else if( b_node->tag[pos]=='}' )
				{
					pos += 1;
					return last;
				}
				else if( b_node->tag[pos]=='&' )
				{
					pos += 1;
					break;
				}
				else
				{
					cout << "ERROR: unknow node tag character " << b_node->tag[pos] << endl;
					exit(0);
				}
			}
	}
	if( b_node->children[0] )
	{
		int p, d;
		NodeB2U( b_node->children[0], p=0, d=0, last, u_graph, node_map );
		NodeB2U( b_node->children[1], p=0, d=0, last, u_graph, node_map );
	}
	return NULL;
}
void TransformB2U( const Bi_UCCA_Graph &bi_graph, UCCA_Graph &u_graph )
{
	for(auto x = u_graph.nodes.begin(); x != u_graph.nodes.end(); x++ )
		delete *x;
	u_graph.nodes.clear();
	unordered_map< Bi_UCCA_Graph::InnerNodePoint, UCCA_Graph::Node* > node_map;
	UCCA_Graph::Node vir_root( "0", "virtual_root" );
	int pos=0, dfs_num=0;
	NodeB2U( bi_graph.root, pos, dfs_num, &vir_root, u_graph, node_map );
	u_graph.root = vir_root.children[0];
	u_graph.root->father = NULL;
	
	u_graph.remote_edges.clear();
	for( auto it = bi_graph.remote_edges.begin(); it != bi_graph.remote_edges.end(); it++ )
		u_graph.remote_edges.push_back( make_pair( node_map[ it->first ], node_map[ it->second ] ) );
	u_graph.mirror_edges.clear();
	for( auto it = bi_graph.mirror_edges.begin(); it != bi_graph.mirror_edges.end(); it++ )
		u_graph.mirror_edges.push_back( make_pair( node_map[ it->first ], node_map[ it->second ] ) );
	return;
}

void Dfs2Score( Bi_UCCA_Graph::Node *b_node, double weight, Chart_Score &score )
{
	if( b_node->children[0]==NULL )
	{
		if( b_node->left+1 != b_node->right || b_node->left < 1 || b_node->right > score.Length()+1 )
		{
			cout << "ERROR: illegal terminal span " << b_node->left << " " << b_node->right << endl;
			exit(0);
		}
		auto &score_map = score.terminal_score[b_node->left];
		if( score_map.find( b_node->tag ) == score_map.end() )
			score_map[ b_node->tag ] = 0;
		score_map[ b_node->tag ] += weight;
		return;
	}
	auto &score_map = score.cmb_score[ b_node->left ][ b_node->right ];
	//printf("add %d %d %d\n", b_node->left, b_node->children[0]->right, b_node->right);
	Chart_Combination cmb( b_node->children[0]->right, b_node->children[0]->tag, b_node->children[1]->tag, b_node->tag );
	if( score_map.find( cmb ) == score_map.end() )
		score_map[ cmb ] = 0;
	score_map[ cmb ] += weight;
	Dfs2Score( b_node->children[0], weight, score );
	Dfs2Score( b_node->children[1], weight, score );
	return;
}
void Add2Score( const Bi_UCCA_Graph &bi_graph, double weight, Chart_Score &score )
{
	Dfs2Score( bi_graph.root, weight, score );
	for( auto it = bi_graph.remote_edges.begin(); it != bi_graph.remote_edges.end(); it++ )
	{
		auto pr = make_pair( 
			Chart_InnerNodePoint( Chart_Node( it->first.node->left, it->first.node->right, it->first.node->tag ), it->first.dfs_num ),
			Chart_InnerNodePoint( Chart_Node( it->second.node->left, it->second.node->right, it->second.node->tag ), it->second.dfs_num ));
		if( score.remote_score.find(pr) == score.remote_score.end() )
			score.remote_score[ pr ] = 0;
		score.remote_score[ pr ] += weight;
	}
	for( auto it = bi_graph.mirror_edges.begin(); it != bi_graph.mirror_edges.end(); it++ )
	{
		auto pr = make_pair( 
			Chart_InnerNodePoint( Chart_Node( it->first.node->left, it->first.node->right, it->first.node->tag ), it->first.dfs_num ),
			Chart_InnerNodePoint( Chart_Node( it->second.node->left, it->second.node->right, it->second.node->tag ), it->second.dfs_num ));
		if( score.mirror_score.find(pr) == score.mirror_score.end() )
			score.mirror_score[ pr ] = 0;
		score.mirror_score[ pr ] += weight;
	}
	return;
}

Bi_UCCA_Graph::Node* DfsDp2B( const Dynamic_Program::Item *item, Bi_UCCA_Graph &bi_graph, 
	unordered_map< Chart_Node, Bi_UCCA_Graph::Node* > &node_map )
{
	if( item==NULL )
		return NULL;
	auto x = new Bi_UCCA_Graph::Node( item->node.left, item->node.right, bi_graph.nodes.size()+1 );
	bi_graph.nodes.push_back( x );
	node_map[ item->node ] = x;
	x->tag = item->node.tag;
	x->children[0] = DfsDp2B( item->from[0], bi_graph, node_map );
	x->children[1] = DfsDp2B( item->from[1], bi_graph, node_map );
	return x;
}
void DP2BiGraph( const Dynamic_Program &dp, Bi_UCCA_Graph &bi_graph )
{
	for(auto x = bi_graph.nodes.begin(); x != bi_graph.nodes.end(); x++ )
		delete *x;
	bi_graph.nodes.clear();
	unordered_map< Chart_Node, Bi_UCCA_Graph::Node* > node_map;
	bi_graph.root = DfsDp2B( dp.final, bi_graph, node_map );

	bi_graph.remote_edges.clear();
	for( auto it = dp.remote_edges.begin(); it != dp.remote_edges.end(); it++ )
		bi_graph.remote_edges.push_back( make_pair( 
			Bi_UCCA_Graph::InnerNodePoint( node_map[ it->first.node ], it->first.dfs_num ),
			Bi_UCCA_Graph::InnerNodePoint( node_map[ it->second.node ], it->second.dfs_num ) ));
	bi_graph.mirror_edges.clear();
	for( auto it = dp.mirror_edges.begin(); it != dp.mirror_edges.end(); it++ )
		bi_graph.mirror_edges.push_back( make_pair( 
			Bi_UCCA_Graph::InnerNodePoint( node_map[ it->first.node ], it->first.dfs_num ),
			Bi_UCCA_Graph::InnerNodePoint( node_map[ it->second.node ], it->second.dfs_num ) ));
	return;
}

#endif





















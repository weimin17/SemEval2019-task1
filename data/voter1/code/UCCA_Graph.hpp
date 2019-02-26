#pragma once
#ifndef UCCA_GRAPH_HPP
#define UCCA_GRAPH_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
using namespace std;

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

bool span_error_flag;

bool operator ==(const pair<int,int> &x, const pair<int,int> &y)
{
	return x.first==y.first && x.second==y.second;
}
template <> struct hash< pair<int,int> >
{
	size_t operator()(const pair<int,int> &x) const
	{
		return hash<int>()( x.first * 100007 + x.second );
	}
};

class TerminalInfo
{
public:
	string text, type;
};

class UCCA_Graph
{
public:
	class Node
	{
	public:
		string id, type, edge_tag;
		vector<Node*> children;
		Node *father;
		bool is_implicit, is_mirror;
		int position, left, right;
		string text;
		Node(string v_id, string v_type): id(v_id), type(v_type), edge_tag(""), father(NULL), is_implicit(false), is_mirror(false), position(0) {}
	};
	// 主要信息
	Node *root;
	vector<Node*> nodes;
	vector< pair<Node*,Node*> > remote_edges;
	vector< pair<Node*,Node*> > mirror_edges;
	string passage_id;
	vector< TerminalInfo > terminal_info;
	// 辅助信息
	unordered_map<string,Node*> id2node;
	unordered_map< pair<int,int>, Node* > span2node;
	unordered_set< pair<int,int> > gaps;
	int remote_cnt, implicit_cnt, cross_cnt;

	~UCCA_Graph()
	{
		for( auto n = nodes.begin(); n!=nodes.end(); n++ )
			delete *n;
		return;
	}
	UCCA_Graph() {}
	UCCA_Graph(const char *file_path, double weight): root(NULL), remote_cnt(0), implicit_cnt(0), cross_cnt(0)
	{
		terminal_info.resize(1);
		rapidxml::file<> xml_file(file_path);
	    rapidxml::xml_document<> xml_doc;
	    xml_doc.parse<0>(xml_file.data());
	    rapidxml::xml_node<>* xml_root = xml_doc.first_node();
	    passage_id = xml_root->first_attribute()->next_attribute()->value();
	    //cout << passage_id << endl;
	    // 读取 XML
	    vector<Edge> edges;
	    vector<Edge> remotes;
	    for( auto node = xml_root->first_node(); node != NULL; node = node->next_sibling() )
	    	if( !strcmp( node->name(), "layer" ) )
	    	{
	    		string layer_id = node->first_attribute()->value();
	    		if( layer_id=="0" )
	    			for( auto sub_node = node->first_node(); sub_node != NULL; sub_node = sub_node->next_sibling() )
	    				XML_L0_Node( sub_node, edges, weight );
	    		else if( layer_id=="1" )
	    			for( auto sub_node = node->first_node(); sub_node != NULL; sub_node = sub_node->next_sibling() )
	    				XML_L1_Node( sub_node, edges, remotes );
	    		else
	    		{
	    			cout << "ERROR: " << "layer id" << endl;
	    			exit(0);
	    		}
	    	}
	    	else if( strcmp( node->name(), "attributes") && strcmp( node->name(), "extra") )
	    	{
	    		cout << "ERROR: " << "extra layer " << node->name() << endl;
	    		exit(0);
	    	}
	    // 建立 edge
	    for( auto edge = edges.begin(); edge != edges.end(); edge++ )
	    {
		    if( id2node.find( edge->id1 )==id2node.end() || id2node.find( edge->id2 )==id2node.end() )
			{
				cout << "ERROR: " << "not found node id " << endl;
				exit(0);
			}
			if( id2node[ edge->id2 ]->edge_tag != "" )
			{
				/*
				cout << "ERROR: " << "reentrance " << edge->id2 << endl;
				exit(0);
				*/
				remotes.push_back( Edge( edge->id1, edge->id2, edge->tag ) );
				continue;
			}
			id2node[ edge->id1 ]->children.push_back( id2node[ edge->id2 ] );
			id2node[ edge->id2 ]->father = id2node[ edge->id1 ];
			id2node[ edge->id2 ]->edge_tag = edge->tag;
		}
		for( auto node = nodes.begin(); node != nodes.end(); node++ )
			if( (*node)->id == "1.1" )
			{
				if( (*node)->father )
				{
					cout << "ERROR: " << "node 1.1 is not root" << endl;
					exit(0);
				}
				root = *node;
				break;
			}
		for( auto node = nodes.begin(); node != nodes.end(); node++ )
			if( !(*node)->father && *node != root )
			{
				if( (*node)->id[0]=='0' )
				{
					(*node)->edge_tag = "Terminal";
					(*node)->father = root;
					root->children.push_back( *node );
				}
				else
				{
					cout << "ERROR: " << "multiple root " << root->id << " " << (*node)->id << endl;
					exit(0);
				}
			}
		root->edge_tag = "root";
		// 建立 remote 结点
		remote_cnt = remotes.size();
	    for( auto edge = remotes.begin(); edge != remotes.end(); edge++ )
	    {
	    	Node *child = new Node( "r"+to_string( edge-remotes.begin()+1 ), "FN" );
	    	nodes.push_back( child );
	    	id2node[ edge->id1 ]->children.push_back( child );
	    	child->father = id2node[ edge->id1 ];
	    	child->edge_tag = edge->tag;
	    	child->left = child->right = 0;
	    	child->is_implicit = true;
	    	remote_edges.push_back( make_pair( child, id2node[ edge->id2 ] ) );
	    }
	    return;
	}
	void Reorder()
	{
		/*
		FindGap( root, root->children.begin() );
		FILE *ft = fopen("graph1.txt","w");
		Print(ft);
		fclose(ft);
		*/
		FILE *fout;// = fopen("debug.txt","w");
		fout = stdout;
		Node inf_node( "", "" );
		inf_node.right = 99999999;
		vector<Node*> inf_vec = { &inf_node };
		cross_cnt = 0;
		while(1)
		{
			auto gap_it = FindGap( root, inf_vec.begin() );
			if( span_error_flag )
				return;
			//Print(fout);
			//fprintf(fout,"\n\n");
			if( gap_it==inf_vec.begin() )
				break;
			auto gap = make_pair( (*gap_it)->right, (*(gap_it+1))->left );
			gaps.insert( gap );			
			//fprintf(fout, "find gap %d %d\n", gap.first, gap.second);
			Node *fth = (*gap_it)->father;
			for( int start = gap.first; start < gap.second; )
				for( int end = gap.second; ; end-- )
					if( span2node.find( make_pair(start,end) ) != span2node.end() )
					{
						//fprintf( fout, "\tdiv %d %d\n", start,end );
						cross_cnt += 1;
						Node *gap_node = span2node[ make_pair(start,end) ];
						for( int i=0; ; i++ )
							if( gap_node->father->children[i] == gap_node )
							{
								Node *x = new Node( "m"+to_string( cross_cnt ), "mirror" );
								nodes.push_back( x );
								x->father = gap_node->father;
								x->edge_tag = gap_node->edge_tag;
								x->is_mirror = true;
								gap_node->edge_tag = "fake";
								gap_node->father->children[i] = x;
								gap_node->father = fth;
								fth->children.insert( fth->children.end(), gap_node );
								mirror_edges.push_back( make_pair(x, gap_node) );
								break;
							}
						start = end;
						break;
					}
			//Print(fout);
			//fprintf(fout,"\n\n\n\n");
		}
		//fclose(fout);
		return;
	}
	void OriginalOrder()
	{
		for( auto edge = mirror_edges.begin(); edge != mirror_edges.end(); edge++ )
		{
			Node *x = edge->first, *y = edge->second;
			for( auto it = y->father->children.begin(); ; it++ )
				if( *it == y )
				{
					y->father->children.erase( it );
					break;
				}
			for( int i=0; ; i++ )
				if( x->father->children[i]==x )
				{
					x->father->children[i] = y;
					break;
				}
			y->father = x->father;
			y->edge_tag = x->edge_tag;
		}
		return;
	}
	void ExportXML(string filename)
	{
		using namespace rapidxml;
		xml_document<> doc;
		auto AddNode = [&doc](xml_node<> *father, const char *name) -> xml_node<> *
		{	xml_node<> *node = doc.allocate_node(node_element, name, NULL );
			father->append_node( node );
			return node; };
		auto AddAttr = [&doc](xml_node<> *node, const char *name, const string &value) -> void
		{  node->append_attribute( doc.allocate_attribute(name, doc.allocate_string(value.c_str()) ) ); };

		xml_node<> *xml_root = doc.allocate_node(node_element, "root", NULL);
		doc.append_node(xml_root);
		AddAttr( xml_root, "annotationID", "0" );
		AddAttr( xml_root, "passageID", passage_id );
		AddNode( xml_root, "attributes" );

		auto *layer_0 = AddNode( xml_root, "layer" );
		AddAttr( layer_0, "layerID", "0" );
		AddNode( layer_0, "attributes" );
		for( int i=1; i<terminal_info.size(); i++ )
		{
			auto x = AddNode( layer_0, "node" );
			AddAttr( x, "ID", "0."+to_string(i) );
			AddAttr( x, "type", terminal_info[i].type );
			auto a = AddNode( x, "attributes" );
			AddAttr( a, "paragraph", "1" );
			AddAttr( a, "paragraph_position", to_string(i) );
			AddAttr( a, "text", terminal_info[i].text );
		}

		unordered_map< Node*, Node* > remote_map, mirror_map;
		unordered_set< Node* > mirror_fake;
		for( auto it = remote_edges.begin(); it != remote_edges.end(); it++ )
			remote_map[ it->first ] = it->second;
		for( auto it = mirror_edges.begin(); it != mirror_edges.end(); it++ )
		{
			mirror_map[ it->first ] = it->second;
			mirror_fake.insert( it->second );
		}

		queue< Node* > que;
		que.push( root );
		auto AddEdge = [&doc, &que, AddNode, AddAttr]( xml_node<> *xml_node, string tag, Node *x, bool is_remote) ->void
		{
			auto edge = AddNode( xml_node, "edge" );
			if( tag == "Terminal" )
				AddAttr( edge, "toID", "0."+to_string(x->left) );
			else
				AddAttr( edge, "toID", "1."+x->id );
			AddAttr( edge, "type", tag );
			auto attr = AddNode( edge, "attributes" );
			if( is_remote )
				AddAttr( attr, "remote", "True" );
			if( tag!="Terminal" && !is_remote )
				que.push(x);
			return;
		};

		auto *layer_1 = AddNode( xml_root, "layer" );
		AddAttr( layer_1, "layerID", "1" );
		AddNode( layer_1, "attributes" );
		while( !que.empty() )
		{
			Node *x = que.front();
			que.pop();
			auto node = AddNode( layer_1, "node" );
			AddAttr( node, "ID", "1." + x->id);
			AddAttr( node, "type", "FN" );
			auto attr = AddNode( node, "attributes" );
			if( x->is_implicit )
			{
				AddAttr( attr, "implicit", "True");
				continue;
			}
			for( auto it = x->children.begin(); it != x->children.end(); it++ )
				if( (*it)->is_implicit && remote_map.find(*it) != remote_map.end() ) // remote
					AddEdge( node, (*it)->edge_tag, remote_map[*it], true );
				else if( (*it)->is_mirror )
				{
					if( mirror_map.find( *it ) == mirror_map.end() ) // mirror 未匹配，直接遗弃
						continue;
					else // mirror 匹配
						AddEdge( node, (*it)->edge_tag, mirror_map[*it], false );
				}
				else if( (*it)->edge_tag == "fake" )
				{
					if( mirror_fake.find( *it ) != mirror_fake.end() ) // fake 已匹配，跳过
						continue;
					else // fake 未匹配，转为 U 标签
						AddEdge( node, "U", *it, false );
				}
				else // 普通结点 或 非remote的implicit
					AddEdge( node, (*it)->edge_tag, *it, false );
		}

		//cout << doc;
		ofstream fout(filename);
		fout << doc;
		fout.close();
		return;
	}

	void Print(FILE *fout) 
	{	
		auto EdgeCmp = [](const pair<Node*,Node*> &x, const pair<Node*,Node*> &y) -> bool
			{ 
				auto NodeCmp = []( const Node &x, const Node &y ) -> bool
				{ return x.left < y.left || ( x.left==y.left && x.right < y.right )
					|| ( x.left==y.left && x.right==y.right && x.edge_tag < y.edge_tag); };
				return NodeCmp( *x.first->father, *y.first->father )
					|| ( x.first->father->left==y.first->father->left && x.first->father->right==y.first->father->right 
						&& x.first->father->edge_tag==y.first->father->edge_tag && NodeCmp( *x.second, *y.second ) );
			};
		sort( remote_edges.begin(), remote_edges.end(), EdgeCmp );
		sort( mirror_edges.begin(), mirror_edges.end(), EdgeCmp );

		DfsPrint( root, true, 0, fout );
		for( auto edge = remote_edges.begin(); edge != remote_edges.end(); edge++ )
			fprintf(fout, "remote: %s: %s-%s [%d,%d)  %s-%s [%d,%d)\n", edge->first->edge_tag.c_str(),
				edge->first->edge_tag.c_str(), edge->first->id.c_str(), edge->first->father->left, edge->first->father->right,
				edge->second->edge_tag.c_str(), edge->second->id.c_str(), edge->second->left, edge->second->right);
		for( auto edge = mirror_edges.begin(); edge != mirror_edges.end(); edge++ )
			fprintf(fout, "gap: %s: %s-%s [%d,%d)  %s-%s [%d,%d)\n", edge->first->edge_tag.c_str(),
				edge->first->father->edge_tag.c_str(), edge->first->father->id.c_str(), edge->first->father->left, edge->first->father->right,
				edge->second->edge_tag.c_str(), edge->second->id.c_str(), edge->second->left, edge->second->right);
		return;
	}
private:
	class Edge
	{
	public:
		string id1, id2, tag;
		Edge( string v_id1, string v_id2, string v_tag ): id1(v_id1), id2(v_id2), tag(v_tag) {}
	};
	void XML_L0_Node(rapidxml::xml_node<> *x_node, vector<Edge> &edges, double weight)
	{
		if( !strcmp( x_node->name(), "attributes" ) || !strcmp( x_node->name(), "extra" ) )
			return;
		if( strcmp( x_node->name(), "node" ) )
		{
			cout << "ERROR: " << "unknow layer " << x_node->name() << endl;
			exit(0);
		}
		int pos;
		string node_id, node_type;
		for( auto attr = x_node->first_attribute(); attr != NULL; attr = attr->next_attribute() )
			if( !strcmp( attr->name(), "ID" ) )
			{
				node_id = attr->value();
				if( node_id != "0."+to_string( terminal_info.size() ) )
				{
					cout << "ERROR: " << "gap in terminal nodes" << endl;
					exit(0);
				}
				pos = terminal_info.size();
				terminal_info.push_back( TerminalInfo() );
			}
			else if( !strcmp( attr->name(), "type") )
			{
				node_type = attr->value();
				terminal_info[pos].type = node_type;
			}
			else
			{
				cout << "ERROR: " << "unknow node attr " << attr->name() << endl;
				exit(0);
			}
		if( id2node.find( node_id ) != id2node.end() )
		{
			cout << "ERROR: " << "same node id" << endl;
			exit(0);
		}
		Node *u_node = new Node(node_id, node_type);
		nodes.push_back( u_node );
		id2node.insert( make_pair( node_id, u_node ) );
		for( x_node = x_node->first_node(); x_node != NULL; x_node = x_node->next_sibling() )
			if( !strcmp( x_node->name(), "attributes" ) )
				for( auto attr = x_node->first_attribute(); attr != NULL; attr = attr->next_attribute() )
					if( !strcmp( attr->name(), "paragraph_position" ) )
					{
						u_node->position = atoi( attr->value() );
						if( "0."+to_string(u_node->position) != u_node->id )
						{
							cout << "ERROR: " << "postion is not equal to id " << u_node->position << " " << u_node->id << endl;
							exit(0);
						}
					}
					else if( !strcmp( attr->name(), "text" ) )
					{
						u_node->text = attr->value();
						terminal_info[pos].text = attr->value();
					}
					else if( !strcmp( attr->name(), "paragraph") )
					{
						if( strcmp( attr->value(), "1" ) )
						{
							cout << "ERROR: " << "paragraph is not 1" << endl;
							exit(0);
						}
					}
					else
					{
						cout << "ERROR: " << "unkown L0 node attr " << attr->name() << endl;
						exit(0);
					}
			else if( strcmp( x_node->name(), "extra" ) )
			{
				cout << "ERROR: " << "extra node info" << endl;
				exit(0);
			}
		return;	
	}
	void XML_L1_Node(rapidxml::xml_node<> *x_node, vector<Edge> &edges, vector<Edge> &remotes)
	{
		if( !strcmp( x_node->name(), "attributes" ) )
			return;
		string node_id, node_type;
		if( strcmp( x_node->name(), "node" ) )
		{
			cout << "ERROR: " << "unknow layer" << endl;
			exit(0);
		}
		for( auto attr = x_node->first_attribute(); attr != NULL; attr = attr->next_attribute() )
			if( !strcmp( attr->name(), "ID" ) )
				node_id = attr->value();
			else if( !strcmp( attr->name(), "type") )
				node_type = attr->value();
			else
			{
				cout << "ERROR: " << "unknow node attr" << endl;
				exit(0);
			}
		if( id2node.find( node_id ) != id2node.end() )
		{
			cout << "ERROR: " << "same node id" << endl;
			exit(0);
		}
		if( node_type=="LKG" )
			return;
		Node *u_node = new Node(node_id, node_type);
		nodes.push_back( u_node );
		id2node.insert( make_pair( node_id, u_node ) );
		for( x_node = x_node->first_node(); x_node != NULL; x_node = x_node->next_sibling() )
			if( !strcmp( x_node->name(), "edge" ) )
			{
				string to_id, edge_type;
				for( auto attr = x_node->first_attribute(); attr != NULL; attr = attr->next_attribute() )
					if( !strcmp( attr->name(), "toID" ) )
						to_id = attr->value();
					else if( !strcmp( attr->name(), "type" ) )
						edge_type = attr->value();
					else
					{
						cout << "ERROR: " << "unknow edge attr " << attr->name() << endl;
						exit(0);
					}
				if( x_node->first_node()!=NULL && !strcmp( x_node->first_node()->name(), "attributes")
					&& x_node->first_node()->first_attribute()!=NULL
					&& !strcmp( x_node->first_node()->first_attribute()->name(), "remote" )
					&& !strcmp( x_node->first_node()->first_attribute()->value(), "True" ) )
					remotes.push_back( Edge( node_id, to_id, edge_type ) );
				else
					edges.push_back( Edge( node_id, to_id, edge_type ) );
			}
			else if( !strcmp( x_node->name(), "attributes" ) )
			{
				if( x_node->first_attribute()!=NULL 
					&& !strcmp( x_node->first_attribute()->name(), "implicit" )
					&& !strcmp( x_node->first_attribute()->value(), "True" ) )
				{
					u_node->is_implicit = true;
					implicit_cnt += 1;
				}
			}
			else if( strcmp( x_node->name(), "extra" ) )
			{
				cout << "ERROR: " << "extra node info " << x_node->name() << endl;
				exit(0);
			}
		return;	
	}
	vector<Node*>::iterator FindGap(Node *cur, vector<Node*>::iterator inf_it)
	{
		if( !cur->children.size() )
		{
			if( cur->is_implicit || cur->is_mirror )
				cur->left = cur->right = 0;
			else if( ((cur->type=="Word" || cur->type=="Punctuation") && cur->edge_tag=="Terminal") || cur->edge_tag=="fake" )
			{
				cur->left = cur->position;
				cur->right = cur->position+1;
				span2node[ make_pair( cur->left, cur->right ) ] = cur;
			}
			else
			{
				cout << "ERROR: " << "unkown leaf node " << cur->id << endl;
				exit(0);
			}
			return inf_it;
		}
		auto min_gap_it = inf_it;
		for( auto child = cur->children.begin(); child != cur->children.end(); child++ )
		{
			auto it = FindGap( *child, inf_it );
			if( (*it)->right < (*min_gap_it)->right )
				min_gap_it = it;
		}
		sort( cur->children.begin(), cur->children.end(), 
			[](const Node *x, const Node *y)
			{ return x->left < y->left || (x->left == y->left && x->right < y->right) || (x->left == y->left && x->right==y->right && x->edge_tag < y->edge_tag); } );
		Node *pre_child = NULL;
		cur->left = 99999999;
		cur->right = 0;
		for( auto child = cur->children.begin(); child != cur->children.end(); child++ )
		{
			if( pre_child && (*child)->right && pre_child->right < (*child)->left && pre_child->right < (*min_gap_it)->right )
				min_gap_it = child-1;
			if( (*child)->right )
			{
				pre_child = *child;
				cur->left = min( cur->left, (*child)->left );
				cur->right = max( cur->right, (*child)->right );
			}
		}
		span2node[ make_pair( cur->left, cur->right ) ] = cur;
		if( cur->right==0 )
		{
			
			cout << "ERROR: " << "not found span " << cur->id << endl;
			cout << cur->children.size() << endl;
			cout << cur->children[0]->left << " " << cur->children[0]->right << endl;
			exit(0);
			
			if( !span_error_flag )
				span_error_flag = true;
		}
		return min_gap_it;
	}
	void DfsPrint(Node *x, bool first, int depth, FILE *fout) 
	{
		//x->id = "";
		if( !first )
			for( int i=0; i<depth; i++ )
				fprintf(fout, "\t");
		fprintf(fout, "%s-%s ", x->edge_tag.c_str(), x->id.c_str());
		fprintf(fout, "[%d,%d)", x->left, x->right);
		if( x->children.size()==0 )
			fprintf( fout, "\n" );
		else
		{
			fprintf( fout, "\t" );
			first = true;
			for( auto child = x->children.begin(); child != x->children.end(); child++ )
			{
				DfsPrint( *child, first, depth+1, fout );
				first = false;
			}
		}
		return;
	}
};

#endif

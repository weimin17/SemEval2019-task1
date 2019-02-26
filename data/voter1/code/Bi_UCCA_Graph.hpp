#pragma once
#ifndef BI_UCCA_GRAPH_HPP
#define BI_UCCA_GRAPH_HPP

class Bi_UCCA_Graph
{
public:
	class Node
	{
	public:
		int left, right;
		string tag;
		Node *children[2], *father;
		int id;
		Node(int l, int r, int i): left(l), right(r), id(i), tag(""), father(NULL)
		{
			children[0] = children[1] = NULL;
			return;
		}
	};
	class InnerNodePoint
	{
	public:
		Node *node;
		int dfs_num;
		InnerNodePoint(Node *n, int d): node(n), dfs_num(d) {}
	};
	Node *root;
	vector<Node*> nodes;
	vector< pair<InnerNodePoint,InnerNodePoint> > remote_edges;
	vector< pair<InnerNodePoint,InnerNodePoint> > mirror_edges;
	Bi_UCCA_Graph() {}
	~Bi_UCCA_Graph()
	{
		for( auto it = nodes.begin(); it != nodes.end(); it++ )
			delete *it;
		return;
	}
	void Print(FILE *fout)
	{
		DfsPrint( root, true, 0, fout );
		for( auto it = remote_edges.begin(); it != remote_edges.end(); it++ )
			fprintf(fout, "remote: %s-%d[%d,%d)-%d  %s-%d[%d,%d)-%d\n",
				it->first.node->tag.c_str(), it->first.node->id, it->first.node->left, it->first.node->right, it->first.dfs_num,
				it->second.node->tag.c_str(), it->second.node->id, it->second.node->left, it->second.node->right, it->second.dfs_num);
		for( auto it = mirror_edges.begin(); it != mirror_edges.end(); it++ )
			fprintf(fout, "mirror: %s-%d[%d,%d)-%d  %s-%d[%d,%d)-%d\n",
				it->first.node->tag.c_str(), it->first.node->id, it->first.node->left, it->first.node->right, it->first.dfs_num,
				it->second.node->tag.c_str(), it->second.node->id, it->second.node->left, it->second.node->right, it->second.dfs_num);
		return;
	}
private:
	void DfsPrint(const Node *x, bool first, int depth, FILE *fout) const
	{
		if( !first )
			for( int i=0; i<depth; i++ )
				fprintf(fout, "\t");
		fprintf(fout, "%s-%d ", x->tag.c_str(), x->id);
		fprintf(fout, "[%d,%d)", x->left, x->right);
		if( x->children[0]==NULL )
			fprintf( fout, "\n" );
		else
		{
			fprintf( fout, "\t" );
			first = true;
			for( int i=0; i<2; i++ )
			{
				DfsPrint( x->children[i], first, depth+1, fout );
				first = false;
			}
		}
		return;
	}
};


bool operator==( const Bi_UCCA_Graph::InnerNodePoint &x, const Bi_UCCA_Graph::InnerNodePoint &y )
{
	return x.node==y.node && x.dfs_num==y.dfs_num;
}
template <> struct hash<Bi_UCCA_Graph::InnerNodePoint>
{
	size_t operator()( const Bi_UCCA_Graph::InnerNodePoint &x ) const
	{ return hash<Bi_UCCA_Graph::Node*>()(x.node) * 107 + hash<int>()(x.dfs_num); }
};

#endif

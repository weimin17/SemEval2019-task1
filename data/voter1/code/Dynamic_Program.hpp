#pragma once
#ifndef DYNAMIC_PROGRAM_HPP
#define DYNAMIC_PROGRAM_HPP


class Dynamic_Program
{
public:
	class Item
	{
	public:
		Chart_Node node;
		double score;
		Item* from[2];
		Item(Chart_Node n, double s, Item *c1, Item *c2): node(n), score(s)
		{
			from[0] = c1;
			from[1] = c2;
			return;
		}
	};
	int length;
	unordered_map< Chart_Node, Item* > best;
	vector< pair< Chart_InnerNodePoint, Chart_InnerNodePoint > > remote_edges;
	vector< pair< Chart_InnerNodePoint, Chart_InnerNodePoint > > mirror_edges;
	Item *final;
	~Dynamic_Program()
	{
		for( auto it = best.begin(); it != best.end(); it++ )
			delete it->second;
		return;
	}
	Dynamic_Program(const Chart_Score &score): length( score.Length() ), final(NULL)
	{
		for( int pos=1; pos<=length; pos++ )
			for( auto it = score.terminal_score[pos].begin(); it != score.terminal_score[pos].end(); it++ )
			{
				Chart_Node chart( pos, pos+1, it->first );
				best[ chart ] = new Item( chart, it->second, NULL, NULL ); 
			}
		for( int width=2; width<=length; width++ )
			for( int left=1; left+width<=length+1; left++ )
			{
				int right = left + width;
				for( auto it = score.cmb_score[left][right].begin(); it != score.cmb_score[left][right].end(); it++ )
					Update( left, right, it->first, it->second );
			}
		unordered_set<Chart_InnerNodePoint> nrm_inner, imp_inner, mrr_inner, fake_inner;
		DfsInner( final, nrm_inner, imp_inner, mrr_inner, fake_inner );
		/*
		printf("remote cd: %lu,  mirror cd: %lu\n", score.remote_score.size(), score.mirror_score.size());
		for( auto it = imp_inner.begin(); it != imp_inner.end(); it++ )
			printf("imp: [%d %d %s]-%d\n", it->node.left, it->node.right, it->node.tag.c_str(), it->dfs_num);
		for( auto it = mrr_inner.begin(); it != mrr_inner.end(); it++ )
			printf("mrr: [%d %d %s]-%d\n", it->node.left, it->node.right, it->node.tag.c_str(), it->dfs_num);
		for( auto it = fake_inner.begin(); it != fake_inner.end(); it++ )
			printf("fake: [%d %d %s]-%d\n", it->node.left, it->node.right, it->node.tag.c_str(), it->dfs_num);
		*/
		Link( score.remote_score, imp_inner, nrm_inner, true, remote_edges );
		Link( score.mirror_score, mrr_inner, fake_inner, false, mirror_edges );
		return;
	}
	void Print(FILE *fout)
	{
		fprintf(fout, "length: %d\n", length);
		DfsPrint( final, true, 0, fout );
		for( auto it = remote_edges.begin(); it != remote_edges.end(); it++ )
			fprintf( fout, "remote: [%d %d %s]-%d  ->  [%d %d %s]-%d\n", 
				it->first.node.left, it->first.node.right, it->first.node.tag.c_str(), it->first.dfs_num,
				it->second.node.left, it->second.node.right, it->second.node.tag.c_str(), it->second.dfs_num);
		for( auto it = mirror_edges.begin(); it != mirror_edges.end(); it++ )
			fprintf( fout, "mirror: [%d %d %s]-%d  ->  [%d %d %s]-%d\n", 
				it->first.node.left, it->first.node.right, it->first.node.tag.c_str(), it->first.dfs_num,
				it->second.node.left, it->second.node.right, it->second.node.tag.c_str(), it->second.dfs_num);
		/*
		for( auto it = best.begin(); it != best.end(); it++ )
			fprintf(fout, "[%d %d %s] = %.2lf\n", it->first.left, it->first.right, it->first.tag.c_str(), it->second->score);
			*/
		return;
	}
private:
	void Update( int left, int right, Chart_Combination cmb, double add )
	{
		Item *c1 = best[ Chart_Node(left, cmb.mid, cmb.l_tag) ];
		Item *c2 = best[ Chart_Node(cmb.mid, right, cmb.r_tag) ];
		Chart_Node chart( left, right, cmb.m_tag );
		double sum = c1->score + c2->score + add;
		if( best.find( chart )!=best.end() && best[chart]->score >= sum )
			return;
		if( best.find( chart )!=best.end() )
			delete best[ chart ];
		Item *get = new Item( chart, sum, c1, c2 );
		best[ chart ] = get;
		if( left==1 && right==length+1 && ( !final || sum > final->score ) )
			final = get;
		return;
	}
	void DfsInner( Item *x, unordered_set<Chart_InnerNodePoint> &nrm_inner, unordered_set<Chart_InnerNodePoint> &imp_inner,
		unordered_set<Chart_InnerNodePoint> &mrr_inner, unordered_set<Chart_InnerNodePoint> &fake_inner )
	{
		if( x==NULL )
			return;
		int dfs_num = 0;
		string inner_tag = "";
		for( int i=0; i<=x->node.tag.size(); i++ )
			if( i<x->node.tag.size() && isalpha( x->node.tag[i] ) )
				inner_tag += x->node.tag[i];
			else
			{
				if( inner_tag != "" )
				{
					dfs_num += 1;
					//printf("[%d %d %s]-%d : %s\n", x->node.left, x->node.right, x->node.tag.c_str(), dfs_num, inner_tag.c_str());
					Chart_InnerNodePoint p( Chart_Node(x->node), dfs_num );
					if( inner_tag[0]=='i' )
						imp_inner.insert( p );
					else if( inner_tag[0]=='m' )
						mrr_inner.insert( p );
					else
					{
						nrm_inner.insert( p );
						if( inner_tag=="fake" )
							fake_inner.insert( p );
					}
				}
				inner_tag = "";
			}
		DfsInner( x->from[0], nrm_inner, imp_inner, mrr_inner, fake_inner );
		DfsInner( x->from[1], nrm_inner, imp_inner, mrr_inner, fake_inner );
		return;
	}
	void Link( const unordered_map< pair<Chart_InnerNodePoint,Chart_InnerNodePoint>, double > &score,
		unordered_set<Chart_InnerNodePoint> &out_set, unordered_set<Chart_InnerNodePoint> &in_set, bool reentrance,
		vector< pair< Chart_InnerNodePoint, Chart_InnerNodePoint > > &edges  )
	{
		vector< pair< pair<Chart_InnerNodePoint,Chart_InnerNodePoint>, double > > list;
		for( auto it = score.begin(); it != score.end(); it++ )
			list.push_back( *it );
		sort( list.begin(), list.end(), [](const pair< pair<Chart_InnerNodePoint,Chart_InnerNodePoint>, double > &x,
				pair< pair<Chart_InnerNodePoint,Chart_InnerNodePoint>, double > &y)
				{ return x.second > y.second; } );
		for( auto it = list.begin(); it != list.end(); it++ )
		{
			auto out = it->first.first, in = it->first.second;
			if( out_set.find( out ) == out_set.end() || in_set.find( in ) == in_set.end() )
				continue;
			edges.push_back( it->first );
			out_set.erase( out );
			if( !reentrance )
				in_set.erase( in );
		}
		return;
	}
	void DfsPrint(Item *x, bool first, int depth, FILE *fout) 
	{
		if( !first )
			for( int i=0; i<depth; i++ )
				fprintf(fout, "\t");
		fprintf(fout, "[%d %d %s] = %.2lf", x->node.left, x->node.right, x->node.tag.c_str(), x->score);
		if( x->from[0]==NULL )
			fprintf( fout, "\n" );
		else
		{
			fprintf( fout, "\t" );
			DfsPrint( x->from[0], true, depth+1, fout );
			DfsPrint( x->from[1], false, depth+1, fout );
		}
		return;
	}

};

#endif
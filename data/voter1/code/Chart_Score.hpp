#pragma once
#ifndef CHART_SCORE_HPP
#define CHART_SCORE_HPP

class Chart_Node
{
public:
	int left, right;
	string tag;
	Chart_Node(int l, int r, string t): left(l), right(r), tag(t) {}
};

bool operator==(const Chart_Node &x, const Chart_Node &y)
{ return x.tag==y.tag && x.left==y.left && x.right==y.right; }
template <> struct hash<Chart_Node>
{
    size_t operator()(const Chart_Node &x) const
    { return hash<string>()(x.tag) * 7 + hash<int>()( x.left*1007 + x.right ); }
};


class Chart_Combination
{
public:
	int mid;
	string l_tag, r_tag, m_tag;
	Chart_Combination(int m, string lt, string rt, string mt): mid(m), l_tag(lt), r_tag(rt), m_tag(mt) {}
};

bool operator==(const Chart_Combination &x, const Chart_Combination &y)
{ return x.mid==y.mid && x.l_tag==y.l_tag && x.r_tag==y.r_tag && x.m_tag==y.m_tag; }
template <> struct hash<Chart_Combination>
{
    size_t operator()(const Chart_Combination &x) const
    { return hash<string>()(x.l_tag+"@"+x.r_tag+"@"+x.m_tag) * 107 + hash<int>()(x.mid); }
};


class Chart_InnerNodePoint
{
public:
	Chart_Node node;
	int dfs_num;
	string inner_tag;
	//Chart_InnerNodePoint(Chart_Node n, int d, string t): node(n), dfs_num(d), inner_tag(t) {}
	Chart_InnerNodePoint(Chart_Node n, int d): node(n), dfs_num(d) {}
};

bool operator==(const Chart_InnerNodePoint &x, const Chart_InnerNodePoint &y)
{ return x.node==y.node && x.dfs_num==y.dfs_num; }
template <> struct hash<Chart_InnerNodePoint>
{
    size_t operator()(const Chart_InnerNodePoint &x) const
    { return hash<Chart_Node>()(x.node)*17 + x.dfs_num; }
};
bool operator==(const pair<Chart_InnerNodePoint,Chart_InnerNodePoint> &x, const pair<Chart_InnerNodePoint,Chart_InnerNodePoint> &y)
{ return x.first==y.first && x.second==y.second; }
template <> struct hash< pair<Chart_InnerNodePoint,Chart_InnerNodePoint> >
{
	size_t operator()(const pair<Chart_InnerNodePoint,Chart_InnerNodePoint> &x) const
	{ return hash<Chart_InnerNodePoint>()( x.first ) * 17 + hash<Chart_InnerNodePoint>()( x.second ); }
};

class Chart_Score
{
public:
	vector< unordered_map<string,double> > terminal_score;
	vector< vector< unordered_map<Chart_Combination, double> > > cmb_score;
	unordered_map< pair<Chart_InnerNodePoint,Chart_InnerNodePoint>, double > remote_score, mirror_score;
	Chart_Score( int len )
	{
		terminal_score.resize( len+1 );
		cmb_score.resize( len+1 );
		for( int i=1; i<=len; i++ )
			cmb_score[i].resize( len+2 );
		return;
	}
	int Length() const
	{
		return terminal_score.size()-1;
	}
	void Print( FILE *fout )
	{
		fprintf(fout, "Terminal:\n");
		for( int pos=1; pos < terminal_score.size(); pos++ )
		{
			auto mp = terminal_score[pos];
			for( auto it = mp.begin(); it != mp.end(); it++ )
				fprintf( fout, "[%d %s] = %.2lf\n", pos, it->first.c_str(), it->second );
		}
		fprintf(fout, "Combination:\n");
		for( int left=1; left < cmb_score.size(); left++ )
			for( int right=left+1; right < cmb_score[left].size(); right++ )
			{
				auto mp = cmb_score[left][right];
				for( auto it = mp.begin(); it != mp.end(); it++ )
					fprintf( fout, "[%d,%d,%d)-[%s %s %s] = %.2lf\n", left, it->first.mid, right, 
						it->first.m_tag.c_str(), it->first.l_tag.c_str(), it->first.r_tag.c_str(), it->second );
			}
		fprintf(fout, "Remote: %lu\n", remote_score.size());
		for( auto it = remote_score.begin(); it != remote_score.end(); it++ )
		{
			Chart_InnerNodePoint x = it->first.first, y = it->first.second;
			fprintf( fout, "[%d %d %s]-%d  ->  [%d %d %s]-%d  =  %.2lf\n", 
				x.node.left, x.node.right, x.node.tag.c_str(), x.dfs_num,
				y.node.left, y.node.right, y.node.tag.c_str(), y.dfs_num, it->second );
		}
		fprintf(fout, "Mirror: %lu\n", mirror_score.size());
		for( auto it = mirror_score.begin(); it != mirror_score.end(); it++ )
		{
			Chart_InnerNodePoint x = it->first.first, y = it->first.second;
			fprintf( fout, "[%d %d %s]-%d  ->  [%d %d %s]-%d  =  %.2lf\n", 
				x.node.left, x.node.right, x.node.tag.c_str(), x.dfs_num,
				y.node.left, y.node.right, y.node.tag.c_str(), y.dfs_num, it->second );
		}
		return;
	}
};

#endif
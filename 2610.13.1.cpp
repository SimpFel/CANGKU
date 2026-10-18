#include <bits/stdc++.h>
using namespace std;
char a[60];			//字符串
int c[60];			//转换数字数组
long long n,s=1,m;
int b[10][16];		//规则类型		
/*
	0		1	5	6	d[0]=3 ----0 1 2 3 4 5 6 9 8
	1	 	2			d[1]=1 ----1 2 3 4 8 9
	2		3	1	4	d[2]=3 ----2 1 3 8 9 4
	3		9	8		d[3]=2
*/
int d[11];			//规则数量
int r[11];			
bool g[11][10];		//数位上有没有用过
void jw(int p,int h)
{
	int i,j,k;
	for(i=1;i<=d[h];i++)
	{
		j=b[h][i];
		if(g[p][j]==0)
		{
			//cout<<j<<" ";
			g[p][j]=1;
			jw(p,j);
		}
	}
}
void sou(int p)		
{
	g[p][p]=1;
	if(p>9)
		return ;
	int i,j,k=0;
	for(i=1;i<=d[p];i++)
	{
		j=b[p][i];
		g[p][j]=1;
		//cout<<j<<" ";
		jw(p,j);
	}
	//cout<<endl;
	for(i=0;i<=9;i++)
	{
		if(g[p][i]==1)
			k++;
	}
		
	r[p]=k;
	sou(p+1);
}
void js()
{
	int i,j;
	for(i=1;i<=m;i++)
	{
		j=c[i];
		s*=r[j];
	}
}
int main()//1 
{
	int i,j,k;
	cin>>a;
	m=strlen(a);
	cin>>n;
	for(i=1;i<=n;i++)
	{
		cin>>k;
		d[k]+=1;			//这个数的规则加一
		cin>>j;				//规则是()
		b[k][d[k]]=j;		//数K的第d[k]个规则
	}
	for(i=0;i<m;i++)		//转化
	{
		j=a[i]-'0';
		c[i+1]=j;
	}		//数字可以的变化			
	sou(0);
	/*	for(i=0;i<=9;i++)
	{
		cout<<r[i]<<" ";
	}*/
	js();
	cout<<s<<endl;
	return 0;
}/*
2456 9
0 1 0 5 0 6
1 2
2 3 2 1 2 4
3 8 3 9

*/
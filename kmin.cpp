#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "kmin.h"
#include "Matrix.cpp"
using namespace std;


void sort(vector<i_j_s_weight >& v)
{
	double temp_weight=0;
	int temp_s,temp_i,temp_j=0;
	vector<int> temp_path;
	int	count = v.size();
	i_j_s_weight temp;
	for (int round = 1; round <= count; round++)
	{
        for (int t = 0; t < count - round; t++)
        {
			if(v[t]._weight>v[t+1]._weight)
			{
				//cout<<"before sort"<<v[t]._weight<<"  "<<v[t+1]._weight<<endl ;			
				temp.clear();
				temp=v[t];
				v[t]=v[t+1];
				v[t+1]=temp;
				//cout<<"after sort"<<v[t]._weight<<"  "<<v[t+1]._weight<<endl ;	
			}
		}
	}
}


void Matrix::build_weight(double **w, Matrix &pre, int step)//move check_diagonal into here
{
	vector<i_j_s_weight> v;
	i_j_s_weight temp;
	for(int j=0;j<_size;j++)
	{	
		for(int current_i=0;current_i<_size;current_i++)
		{
			v.clear();
			for(int i=0;i<_size;i++)
			{
				for(int s=0;s<_k;s++)
				{
					if(pre._array[i][j][s]!=1e9&&w[current_i][i]!=1e9)
					{
						temp.set_w(pre._array[i][j][s]+w[current_i][i]);
						temp.set_i(i);
						temp.set_j(j);
						temp.set_s(s);
						temp.set_step(step);
						temp.set_path(pre._path[i][j][s],current_i);
						v.push_back(temp);
					}
				}
			}
			sort(v);
			for(int x=0;x<v.size()&&x<_k;x++)
			{
				_array[current_i][j][x]=v[x]._weight;
				_path[current_i][j][x]=v[x]._path;
				/*if(v[x]._weight!=1e9)
				{
					cout<<"i="<<current_i<<" ";
					cout<<v[x]._weight<<" "<<endl;
				}
				cout<<"path:";
				for(int j=0;j<v[x]._path.size();j++)
					cout<<v[x]._path[j]<<" ";
				if(v[x]._weight!=1e9)
				{
				cout<<"i="<<current_i<<" j="<<j<<" ";
				cout<<"w="<<_array[current_i][j][x]<<" ";
				}*/
				
			}
		}
	}
}



void get_k_min(vector<i_j_s_weight> &v ,int  k, vector<vector <int> > &loop)
{
	i_j_s_weight *min;
	//double i_j_s_weight::*weight = &i_j_s_weight::_weight;
	//vector<int> i_j_s_weight::*path = &i_j_s_weight::_path;  why not???
	vector<int> temp_loop;
	vector<double> weight;
	while(true)
	{
		temp_loop.clear();
		min=&v[0];
		for(int i=0;i<v.size();i++)
		{
			if(v[i+1]._weight<min->get_w())
			{	
				min=&v[i+1];
			}
		}
		temp_loop=min->get_path();
		weight.push_back(min->get_w());
		min->clear();
		loop.push_back(temp_loop);
		if(loop.size()==k)
			break;
	}
}


void check_diagonal(Matrix &current,int step, vector<i_j_s_weight> &candidate)
{
	double weight=0;
	for(int i=0;i<current._size;i++)
	{
		for(int s=0;s<current._k;s++)
		{
			weight = 0;
			if(current._array[i][i][s]!=1e9)
			{
				weight = current._array[i][i][s] / step;
				i_j_s_weight diag(i,i,s,weight);
				diag.set_step(step);
				reverse(current._path[i][i][s].begin(),current._path[i][i][s].end());
				diag._path=current._path[i][i][s];
				candidate.push_back(diag);
				current._array[i][i][s]=1e9;
				current._path[i][i][s].clear();//question
			}
		}
	}	
}


int main()
{
	const int V = 5;
	int k=3;
	double **w;
	
	w= new double *[V];
	for(int i=0;i<V;i++)
	w[i]= new double[V];

	for(int i=0;i<V;i++){
		for(int j=0;j<V;j++){
			w[i][j]=1e9;
		}
	}

	w[0][1] = -8;
    w[1][0] = -8;
	w[1][2] = 1;
	w[2][3] = -8;
	w[3][2] = -8;
	w[3][0] = 1;
	w[0][2] = -7;
	w[2][0] = -7;
/*	
w[0][1]=0;
w[0][7]=0;
w[1][2]=-1440;
w[2][3]=0;
w[3][4]=-1495;
w[4][5]=0;
w[4][185]=0;
w[5][0]=1616;
w[6][9]=0;
w[6][13]=0;
w[6][17]=0;
w[6][19]=0;
w[6][75]=0;
w[7][6]=-3253;
w[8][11]=0;
w[9][8]=898;
w[10][15]=0;
w[11][6]=-1160;
w[12][21]=0;
w[12][73]=0;
w[13][10]=891;
w[14][23]=0;
w[14][25]=0;
w[14][71]=0;
w[15][8]=52;
w[16][27]=0;
w[16][79]=0;
w[16][81]=0;
w[16][189]=0;
w[17][6]=-20;
w[18][29]=0;
w[19][12]=988;
w[20][31]=0;
w[21][14]=56;
w[22][33]=0;
w[23][14]=-956;
w[24][35]=0;
w[25][16]=-96;
w[26][37]=0;
w[27][18]=155;
w[28][39]=0;
w[29][20]=-84;
w[30][41]=0;
w[31][22]=-53;
w[32][43]=0;
w[32][45]=0;
w[33][24]=-88;
w[34][47]=0;
w[35][26]=-96;
w[36][49]=0;
w[36][51]=0;
w[37][28]=108;
w[38][53]=0;
w[39][30]=-8;
w[40][55]=0;
w[41][32]=-8;
w[42][57]=0;
w[42][59]=0;
w[42][63]=0;
w[43][32]=-48;
w[44][61]=0;
w[44][87]=0;
w[44][103]=0;
w[44][107]=0;
w[44][155]=0;
w[44][171]=0;
w[45][34]=-76;
w[46][65]=0;
w[47][36]=112;
w[48][67]=0;
w[49][36]=-8;
w[50][69]=0;
w[51][38]=-16;
w[52][77]=0;
w[52][179]=0;
w[53][40]=-16;
w[54][83]=0;
w[55][42]=-76;
w[56][85]=0;
w[56][89]=0;
w[57][42]=-88;
w[58][91]=0;
w[59][44]=116;
w[60][93]=0;
w[61][42]=-16;
w[62][95]=0;
w[63][46]=-28;
w[64][97]=0;
w[65][48]=-16;
w[66][99]=0;
w[67][50]=-56;
w[68][101]=0;
w[68][105]=0;
w[68][109]=0;
w[68][111]=0;
w[69][14]=1624;
w[70][113]=0;
w[70][115]=0;
w[71][12]=-1732;
w[72][117]=0;
w[73][6]=-1216;
w[74][119]=0;
w[75][52]=1184;
w[76][121]=0;
w[77][16]=-56;
w[78][123]=0;
w[79][16]=-96;
w[80][125]=0;
w[81][54]=80;
w[82][127]=0;
w[83][56]=-16;
w[84][129]=0;
w[85][44]=-12;
w[86][131]=0;
w[87][56]=-12;
w[88][133]=0;
w[89][58]=-28;
w[90][135]=0;
w[91][60]=60;
w[92][137]=0;
w[92][139]=0;
w[93][62]=-48;
w[94][141]=0;
w[95][64]=-8;
w[96][143]=0;
w[97][66]=-8;
w[98][145]=0;
w[99][68]=-8;
w[100][147]=0;
w[101][44]=62;
w[102][149]=0;
w[103][68]=-24;
w[104][151]=0;
w[105][44]=-8;
w[106][153]=0;
w[106][157]=0;
w[106][159]=0;
w[106][161]=0;
w[107][68]=-4;
w[108][163]=0;
w[109][44]=-8;
w[110][165]=0;
w[111][70]=8;
w[112][167]=0;
w[113][70]=-20;
w[114][169]=0;
w[114][173]=0;
w[115][72]=-4;
w[116][175]=0;
w[117][74]=-8;
w[118][177]=0;
w[119][76]=-16;
w[120][181]=0;
w[121][78]=8;
w[122][183]=0;
w[122][187]=0;
w[123][80]=-8;
w[124][191]=0;
w[125][82]=-24;
w[127][84]=-4;
w[129][86]=-8;
w[131][88]=40;
w[133][90]=-4;
w[135][92]=-8;
w[137][92]=-8;
w[139][94]=-12;
w[141][96]=28;
w[143][98]=-24;
w[145][100]=-8;
w[147][102]=-112;
w[149][104]=-96;
w[151][106]=-303;
w[153][44]=-4;
w[155][106]=-20;
w[157][106]=-18;
w[159][44]=-4;
w[161][108]=-28;
w[163][110]=-4;
w[165][112]=-4;
w[167][114]=-8;
w[169][44]=-8;
w[171][114]=28;
w[173][116]=-28;
w[175][118]=-4;
w[177][52]=772;
w[179][120]=-476;
w[181][122]=-196;
w[183][2]=-78;
w[185][122]=1636;
w[187][16]=-1788;
w[189][124]=-95;
w[191][126]=-40;*/
	
	Matrix current;
	current.initial(V,k);
	Matrix previous;
	previous.initial(V,k);
	vector<i_j_s_weight> candidate;
	vector<vector <int> > loop;
	
	current.build_zero(w);
	cout<<"start build graph"<<endl;
	for(int i=2;i<V;i++)
	{
		previous.clone(current);
		current.build_weight(w,previous,i);
		cout<<"build "<<i<<" table"<<endl;
		check_diagonal(current,i,candidate);
	}
	cout<<"build table finish"<<endl;
	get_k_min(candidate,k,loop);
	for(int i=0;i<loop.size();i++)
	{
		cout<<"path=";
		for(int j=0;j<loop[i].size();j++)
			cout<<loop[i][j]<<" ";
		cout<<endl;
	}
	//sort(candidate);
	/*for(int i=0;i<10;i++)//change
	{
		cout<<"w="<<candidate[i]._weight<<" ";
		cout<<candidate[i]._path.size()<<" ";
		cout<<"path:";
		for(int j=0;j<candidate[i]._path.size();j++)
			cout<<candidate[i]._path[j]<<" ";
		cout<<endl;
	}*/
}

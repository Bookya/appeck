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
using namespace std;


Matrix::Matrix()
{
	
}
void Matrix::initial(int Size, int K)
{
	_size=Size;
	_k=K;
	_array = new double **[_size];
	for(int i=0;i<_size;i++)
	_array[i] = new double *[_size];
	for(int i=0;i<_size;i++)
	{
		for(int j=0;j<_size;j++)
		{
			_array[i][j] = new double[_k];
			for (int s=0;s<_k;s++)
				_array[i][j][s]=1e9;
		}
	}
	//kmin_vec = new int[length];
	//因為不知道怎麼開V個有傳入值給constructor的matrix
}

void Matrix::build_zero(double **w)
{
	for(int i=0;i<_size;i++)
	{
		for(int j=0;j<_size;j++)
		{
			_array[i][j][0]=w[i][j];
		}
	}
}

void sort(s_i_weight v[], int count)
{
	double temp_weight=0;
	int temp_s=0;
	int temp_i=0;
	int t=0;
	for (int round = 1; round <= count; round++)
	{
        for (int t = 0; t < count - round; t++)
        {
			if(v[t]._weight>v[t+1]._weight)
			{
				temp_s=v[t]._s;
				temp_i=v[t]._i;
				temp_weight=v[t]._weight;
				v[t].set_i(v[t+1]._i);
				v[t].set_s(v[t+1]._s);
				v[t].set_w(v[t+1]._weight);
				v[t+1].set_i(temp_i);
				v[t+1].set_s(temp_s);
				v[t+1].set_w(temp_weight);
			}
		}
	}
}

void s_i_weight_clear(s_i_weight v[], int count)
{
	for(int a=0;a<count;a++)
	{
		v[a].clear();
	}
}

void Matrix::build_weight(double **w, Matrix &pre)
{
	int v_count=0;
	s_i_weight v[_k*_size];
	for(int j=0;j<_size;j++)
	{
		for(int current_i=0;current_i<_size;current_i++)
		{
			s_i_weight_clear(v,v_count);		
			v_count=0;
			for(int i=0;i<_size;i++)
			{
				for(int s=0;s<_k;s++)
				{
					if(pre._array[i][j][s]!=1e9&&w[current_i][i]!=1e9)
					{
					 	v[v_count].set_w(pre._array[i][j][s]+w[current_i][i]);
						v[v_count].set_i(i);
						v[v_count].set_s(s);
						v_count++;
					}
				}
			}
			sort(v,v_count);
			for(int x=0;x<_k;x++)
			{
				_array[current_i][j][x]=v[x]._weight;
				/*if(v[x]._weight!=1e9)
				{
				cout<<"i="<<current_i<<" j="<<j<<" ";
				cout<<"w="<<_array[current_i][j][x]<<" ";
				}*/
				
			}
		}
	}
}

void check_diagonal(Matrix &current,int num, vector<Matrix> &candidate)
{
	for(int i=0,j=0;i<current._size&&j<current._size;i++,j++)
	{
		for(int s=0;s<current._k;s++)
		if(current._array[i][j][s]!=1e9)
		{
			candidate.push_back(current);//bug
			current._array[i][j][s]=1e9;
		}
	}	
}


Matrix::~Matrix() {
	delete [] _array;
    //kmin_vec.clear();
}


void s_i_weight::set_s(int s)
{
	_s=s;
}
void s_i_weight::set_i(int i)
{
	_i=i;
}
void s_i_weight::set_w(double w)
{
	_weight=w;
}

void s_i_weight::clear()
{
	_weight=1e9;
	_s=0;
	_i=0;
}

int main()
{
	const int V = 5+1;
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
//    w[2][3] = 100;
//    w[3][2] = 100;

	Matrix matrix[V];
	vector<Matrix> candidate;
	for(int i=0;i<V;i++)
	{
		matrix[i].initial(V,k);
	}
	matrix[1].build_zero(w);
	for(int i=2;i<V;i++)
	{
		matrix[i].build_weight(w,matrix[i-1]);
		check_diagonal(matrix[i],i,candidate);
	}
	cout<<endl;
	for(int num=0;num<V;num++)
	{
		for(int j=0;j<V;j++)
		{
			for(int i=0;i<V;i++)
			{
				for(int s=0;s<k;s++)
				{
					if(matrix[num]._array[i][j][s]!=1e9)
					{					
						cout<<"num "<<num<<" i="<<i<<" j="<<j<<" ";
						cout<<matrix[num]._array[i][j][s]<<endl;
					}
				}
			} 
		}
	}

}

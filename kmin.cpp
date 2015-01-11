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
	{
		_array[i] = new double *[_size];
	}
	for(int i=0;i<_size;i++)
	{
		for(int j=0;j<_size;j++)
		{
			_array[i][j] = new double[_k];
			for (int s=0;s<_k;s++)
			{
				_array[i][j][s]=1e9;
			}
		}
	}
	_path.resize(_size);
	for (int i = 0; i < _size; ++i) 
	{
    	_path[i].resize(_size);
		for (int j = 0; j < _size; ++j) 
		{
    		_path[i][j].resize(_size);
		}
	}

}


void Matrix::build_zero(double **w)
{
	for(int i=0;i<_size;i++)
	{
		for(int j=0;j<_size;j++)
		{
			_array[i][j][0]=w[i][j];
			_path[i][j][0].push_back(j);//opposite
			_path[i][j][0].push_back(i);
		}
	}
}

void sort(vector<i_j_s_weight >v, int count)
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

void i_j_s_weight::set_path(vector<int> pre, int current_i)
{
	_path=pre;
	_path.push_back(current_i);
}

void Matrix::build_weight(double **w, Matrix &pre, int step)
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
			sort(v,v.size());
			for(int x=0;x<v.size()&&x<_k;x++)
			{
				_array[current_i][j][x]=v[x]._weight;
				_path[current_i][j][x]=v[x]._path;
				cout<<"path:";
				for(int j=0;j<v[x]._path.size();j++)
					cout<<v[x]._path[j]<<" ";
				/*if(v[x]._weight!=1e9)
				{
				cout<<"i="<<current_i<<" j="<<j<<" ";
				cout<<"w="<<_array[current_i][j][x]<<" ";
				}*/
				
			}
		}
	}
}


Matrix Matrix::clone() 
{
	Matrix m;
	m.initial(this->_size, this->_k);
	for (int i = 0 ; i < _size ; ++i) 
	{
	    for (int j = 0 ; j < _size ; ++j) 
		{
			for (int s=0 ;s <_k;s++)
			{
				m._array[i][j][s] = this->_array[i][j][s];
			}
		}
	}
	return m;
}

void check_diagonal(Matrix &current,int step, vector<i_j_s_weight> &candidate)
{
	for(int i=0;i<current._size;i++)
	{
		for(int s=0;s<current._k;s++)
		{
			if(current._array[i][i][s]!=1e9)
			{
				i_j_s_weight diag(i,i,s,current._array[i][i][s]);
				diag.set_step(step);
				reverse(current._path[i][i][s].begin(),current._path[i][i][s].end());
				diag._path=current._path[i][i][s];
				candidate.push_back(diag);
				current._array[i][i][s]=1e9;
			}
		}
	}	
}


Matrix::~Matrix() 
{
	for (int i = 0 ; i < _size ; ++i) 
	{
	    for (int j = 0 ; j < _size ; ++j) 
		{
	        delete [] _array[i][j];
	    }
	    delete [] _array[i];
	}
	delete [] _array;
}


void i_j_s_weight::set_s(int s)
{
	_s=s;
}
void i_j_s_weight::set_i(int i)
{
	_i=i;
}

void i_j_s_weight::set_j(int j)
{
	_j=j;
}

void i_j_s_weight::set_step(int step)
{
	_step=step;
}

void i_j_s_weight::set_w(double w)
{
	_weight=w;
}

void i_j_s_weight::clear()
{
	_weight=1e9;
	_s=0;
	_i=0;
	_j=0;
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
	w[0][2] = -7;
	w[2][0] = -7;
//    w[2][3] = 100;
//    w[3][2] = 100;

	Matrix matrix[V];
	vector<i_j_s_weight> candidate;

	for(int i=0;i<V;i++)
	{
		matrix[i].initial(V,k);
	}
	matrix[1].build_zero(w);
	for(int i=2;i<V;i++)
	{
		matrix[i].build_weight(w,matrix[i-1],i);
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
	for(int i=0;i<candidate.size();i++)
	{
		cout<<"w="<<candidate[i]._weight<<" ";
		//cout<<candidate[i]._path.size()<<" ";
		cout<<"path:";
		for(int j=0;j<candidate[i]._path.size();j++)
			cout<<candidate[i]._path[j];
		cout<<endl;
	}

}

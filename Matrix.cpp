

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
    		_path[i][j].resize(_k);
		}
	}
}



void i_j_s_weight::set_path(vector<int> pre, int current_i)
{
	_path=pre;
	_path.push_back(current_i);
}

void Matrix::clone(Matrix &current ) 
{
	for (int i = 0 ; i < _size ; ++i) 
	{
	    for (int j = 0 ; j < _size ; ++j) 
		{
			for (int s=0 ;s <_k;s++)
			{
				_array[i][j][s]=current._array[i][j][s];
				if(!current._path[i][j][s].empty())
					_path[i][j][s]=current._path[i][j][s];
			}
		}
	}
	/*
	for(int i=0;i<3;i++){
		for(int j=0;j<_size;j++){
			for(int s=0;s<_k;s++){
				if(this->_array[i][j][s]==1e9){
					cout<<_array[i][j][s]<<' ';
				}

			}

		}
		cout<<endl ;

	}*/
	current.clear();
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



void Matrix::clear()
{
	for(int i=0;i<_size;i++)
	{
		for(int j=0;j<_size;j++)
		{
			for (int s=0;s<_k;s++)
			{
				_array[i][j][s]=1e9;
				_path[i][j][s].clear();
			}
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
	_path.clear();
	_step=0;
	_weight=1e9;
	_s=0;
	_i=0;
	_j=0;
}


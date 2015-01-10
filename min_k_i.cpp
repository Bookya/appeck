using namespace std;



Min_k_i::Min_k_i()
{
	_i=0;
	_k=0;
}

double Min_k_i::mean()
{
	return _mean;
}

int Min_k_i::k()
{
	return _k;
}

int Min_k_i::i()
{
	return _i;
}

void Min_k_i::k(int k)
{
	_k=k;
}

void Min_k_i::i(int i)
{
	_i=i;
}
void Min_k_i::mean(double mean)
{
	_mean=mean;
}



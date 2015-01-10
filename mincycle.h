using namespace std;

class Min_k_i
{
	public:
		Min_k_i();
		//Min_k_i(double,int,int);

		double mean();	
		int k();
		int i();

		void mean(double);
		void k(int);
		void i(int);
	private:
		double _mean;
		int _k;
		int _i;

};


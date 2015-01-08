using namespace std;

class Matrix
{
	public:
		Matrix();
		~Matrix();

		int _size;
		double ***_array;
		int _k;
		//double **_sum;
		//vector<vector <double> > kmin_vec;

		
		void initial(int,int);
		void build_weight(double**,Matrix);
		void build_zero(double**);
		//Min_k_i(double,int,int);

		/*double mean();	
		int k();
		int i();

		void mean(double);
		void k(int);
		void i(int);
 */
		

	private:
		/*double _mean;
		int _k;
		int _i;*/

};

class s_i_weight
{
	public:
		s_i_weight(int s = 0, int i = 0, double w = 1e9) : _s(s), _i(i),_weight(w) { }

	double _weight;
	int _s;
	int _i;
	void set_s(int);
	void set_i(int);
	void set_w(double);
	void sort(s_i_weight,int);
	void clear();
	//friend void build_weight(double**,int,Matrix,s_j_weight);

};


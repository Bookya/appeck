using namespace std;

class Matrix
{
	public:
		Matrix();
		~Matrix();

		int _size;
		double ***_array;
		int _k;
		vector<vector<vector<int> > > _path;
		
		void initial(int,int);
		void build_weight(double**,Matrix&,int);
		void build_zero(double**);
		Matrix clone(); 
		

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

class i_j_s_weight
{
	public:
		i_j_s_weight(int i= 0 , int j = 0, int s = 0, double w = 1e9) :_i(i), _j(j), _s(s),_weight(w) { }

	double _weight;
	int _s;
	int _i;
	int _j;
	int _step;
	vector<int> _path;

	void set_s(int);
	void set_i(int);
	void set_j(int);
	void set_step(int);
	void set_w(double);
	void set_path(vector<int>,int);
	void clear();
	
};


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
using namespace std;

//----

///./graph test.svm limit.txt 3+5+6 20

//----

std::string exec(std::string cmd);
vector<string> ls(string dir);
vector<string> split(const string &s, char delim);
vector<string>& split(const string &s, char delim, vector<string>& elems);



std::string exec(std::string cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
	return "ERROR";

    char buffer[128];
    std::string result = "";

    try {
	while(!feof(pipe)) {
	    if(fgets(buffer, 128, pipe) != NULL)
		result += buffer;
	}
    } catch (...) {
	std::cerr << "[Warning] Exception caught in " << __FUNCTION__ << endl;
    }

    pclose(pipe);
    return result;
}

vector<string>& split(const string &s, char delim, vector<string>& elems) {
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim))
	elems.push_back(item);
    return elems;
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    return split(s, delim, elems);
}

string erasespace(string str)
{
	int i=0;
	for(;i<str.size();i++){
		if(str[i]>='0'&&str[i]<='9'){
			break ;
		}
	}
	return str.substr(i);
}

int find_max(char* file)
{
	ifstream is ;
	is.open (file,std::ifstream::in);
	string temp;
	getline(is,temp) ;
	getline(is,temp) ;
	getline(is,temp) ;
	const char* camma=",";
	const char* garb[6]={"{","}",";","state",":"," "};
	string::size_type pos = 0;
	

	for(int j=0;j<7;j++)
	{		  
		pos=0; 
	    while ((pos = temp.find_first_of(garb[j], pos)) != string::npos) 
		{
		    temp[pos] = 0;
    	}
	}
	int _max = 0 ;	
	pos = 0;
    while ((pos = temp.find_first_of(camma, pos)) != string::npos) 
	{
	    temp[pos] = '\n';
	}
	
	vector<string> nums=split(temp,'\n');
	
	
	for(int i=0;i<nums.size();i++)
	{
		nums[i]=erasespace(nums[i]);
		if(atoi((nums[i]).c_str())>_max)
			{
				_max=atoi((nums[i]).c_str());
			}
	}
	is.close();
	return _max;
}

void build_graph(int *dir,int max)//???
{
	
	string temp;
	string grep_node = " grep '=' test.svm | sed s%state%%g|sed '1,2d' | sed s%=%%g | sed s%';'%%g | sed s%'{'%%g |sed s%'}'%%g | sed s%':'%%g | sed s%','%%g | sed s%'  '%' '%g |sed s%'   '%%g" ;

	vector<string> node_line =split(exec(grep_node),'\n');
		
	//for(int i=0; i<node_line.size() ;i++)
	//{
		//cout<<node_line[i]<<endl;
	//}

	int size=node_line.size();
	vector<string> node[size];
	for(int i=0;i<size;i++)
	{
		vector<string> temp1=split(node_line[i],' ');
		for(int j=0;j<temp1.size();j++)
		{
			node[i].push_back(temp1[j]);
		}		
	}
	int first_num=0, second_num=0;

	for(int iy=0;iy<size-1;++iy)
	{
		vector<string>:: iterator iter = node[iy].begin()+1;
		first_num=atoi(node[iy][0].c_str());
		for(; iter != node[iy].end(); ++iter)
		{
			second_num=atoi((*iter).c_str());
			//cout<<"first_num= "<<first_num;
			//cout<<" second_num= "<<second_num<<endl;
			//dir[first_num][second_num]=1;
			dir[(first_num*(max+1))+second_num]=1;			
			//cout << *iter << " ";
		 } 
	}
}

void* new2d(int h, int w, int size) 
{ 
	register int i; 
	void **p; 

	p = (void**)new char[h*sizeof(void*) + h*w*size]; 
	for(i = 0; i < h; i++) 
	{ 
		p[i] = ((char *)(p + h)) + i*w*size; 
	} 
} 


void LimitNode(char* file,vector <int> &inc,vector <int>& exc){
	ifstream is(file);
	int num1,num2;
	is >> num1 ;
	for(int i=0;i<num1;i++){
		int temp;
		is>>temp ;
		inc.push_back(temp);
	}
	is>>num2 ;
	//cout<<num2<<endl ;
	for(int i=0;i<num2;i++){
		int temp;
		is>>temp ;
		exc.push_back(temp);
		//cout<<exc[i]<<" ";
	}
	is.close();
}
void CancelNode(int* dir,vector <int> exc,int max)
{
	for(int i=0;i<exc.size();i++)
	{
		for(int j=1;j<=max;j++)
		dir[exc[i]*(max+1)+j]=0;
	}
}

void LimitLoop(vector<vector <int> >&output,int*A,vector<int>& temp,int max,vector <int> inc,vector <int> exc,vector< vector<int> >&loop){
	vector<int>::iterator it;
	int loopstart =0;
	//cout<<"in the loope  "<<output.size()<<endl ;
	for(int i=output.size()-1;i>=0;i--){
		//it = output[i].begin();
		int loopsize = output[i].size();
		for(int j=0;j<loopsize;j++){
			if(output[i][j]==output[i][loopsize-1]){
				
				loopstart = j ;
				break ;
			}
		}
		if(inc.size()!=0&&output.size()!=0){		
			for(int j=0;j<inc.size();j++){
				 it = find(output[i].begin()+loopstart,output[i].end(),inc[j]);
				 if(it==output[i].end()){ // the path doesn't include all wanted nodes
					output.erase(output.begin()+i);//delete ilegal path
					loopstart = -1 ;
					break;
				 }
				 //cout<<" j is "<<j<<endl ;
			}
		}
		/*if(exc.size()!=0&output.size()!=0&&loopstart!=0){
			for(int j=0;j<exc.size();j++){
				it = find(output[i].begin()+loopstart,output[i].end(),exc[j]);
				if(it!=output[i].end()){
					output.erase(output.begin()+i);//delete ilegal path
					loopstart = 0 ;
					break;
				}
			}	
		}*/
		if(loopstart!=-1){
			vector <int> temploop ;
			//cout<<"is me "<<endl ;
			for(int j=loopstart+1;j<loopsize;j++){
				temploop.push_back(output[i][j]);
			}
			loop.push_back(temploop) ;
		}
	}
	reverse(loop.begin(),loop.end());
}


void print(vector<vector<int> > &lasso, vector<vector <int> > &loop)
{
	for(int i=0;i<lasso.size();i++)
	{
		cout<<"The "<<i<<" loop begins with ";;
		for(int j=0;j<lasso[i].size()-1;j++)
		{	
			cout<<lasso[i][j]<<" ";
		}	
				
		cout<<endl ;
		cout<<"repeat loop:  ";	
		for(int j=0;j<loop[i].size();j++)
			{	
				cout<<loop[i][j]<<" ";
			}		
		cout<<endl ;
	}
}


void delete_add(string str,vector <string> &files)
{

	size_t found = str.find("+");
	string whitespaces (" \t\f\v\n\r");
	size_t end = str.find_last_not_of(whitespaces);
	while(found!=std::string::npos)
	{
		found=str.find("+");
		if (found <= end)
			str.replace(found,1," ");
	}
	files=split(str,' ');
	for(int i=0;i<files.size();i++)
	{
		erasespace(files[i]);
	}
}
int whichinc(vector<int> firstloop,vector<int> inc){
	for(int i=0;i<firstloop.size();i++){
		for(int j=0;j<inc.size();j++){
			if(firstloop[i]==inc[j]){
				return inc[j];
			}
		}
	}


} 
void gen_adb(vector<string> &files, int times, vector<vector <int> >output, vector<vector<int> >loop,vector<int> inc)
{
	ofstream os;
	os.open("loop",std::ofstream::out);
	int file_name=atoi((files[0]).c_str());
	int files_size=0;
	int count=0;
	vector<int> adb;
	vector<int>::iterator it;
	bool firstoutput = false ;
	for(int j=0;j<output[file_name].size()-1;j++)
	{	
			adb.push_back(output[file_name][j]);	
			if(j==output[file_name].size()-1){
				for(int k=0;k>inc.size();k++){
					if(output[file_name][j]==inc[k])
						firstoutput= true;
				}
			}
	}
	adb.push_back(-1);
	if(files.size()==1)//只選一個loop
	{
		for(int k=0;k<times;k++)
		{
			for(int j=0;j<loop[file_name].size();j++)
			{
				adb.push_back(loop[file_name][j]);
			}
			adb.push_back(-1);
		}
	}
	else{
		//os<<loopint<<" is loopint"<<endl ;
		int loopint = whichinc(adb,inc);
		if(adb[adb.size()-1]!=loopint){
			it=loop[file_name].begin();
			while(*it!=loopint){
				if(it==loop[file_name].end()){
					it=loop[file_name].begin();
				}
				adb.push_back(*it);
				++it;
			}
		}
		int first=0;
		for(int k=0;k<times;k++)
		{		
			for(int i=0;i<files.size();i++)
			{
				if(first==1)
				{
					file_name=atoi(files[i].c_str());
					it=find(loop[file_name].begin(),loop[file_name].end(),loopint);
					vector<int>::iterator tempit;
					tempit=it;
					while(it!=loop[file_name].end()){
						if(!firstoutput)
							adb.push_back(*it);
						else
							firstoutput=false;
						it++;
					}
					it = loop[file_name].begin();
					
					while(it!=tempit){
						adb.push_back(*it);
						it++;
					}
				}
				first=1;
			}
			
		}
	}
	files_size=adb.size();
		os<<files_size<<" ";
		for(int i=0;i<files_size;i++)
		{
			os<<adb[i]<<" ";
		}
	os.close();
}

void copy(int*dir, int*dirnew, int max)
{
	for(int i=0;i<=max;i++)
	{
		for(int j=0;j<=max;j++)
		{
			dirnew[i*(max+1)+j]=dir[i*(max+1)+j];
		}
	}
}
	

void delete_loop(vector<vector <int> >output, vector<vector <int> >&loop, int max)
{
    vector <int> temp;
	cout<<"=====find_all_loops====="<<endl;
	for(int i=0;i<loop.size();i++){
		for(int j=0;j<loop[i].size();j++)
			cout<<loop[i][j]<<" ";
		cout<<endl ;
	}	
	for (int i=0 ; i < loop.size() ; i++) 
	{
		int j=i+1;
		while(j<loop.size())
		{
			//cout<<" i is "<<i<<" j is "<<j<<" "<<loop.size()<<endl ;			
			if(loop[i].size()==loop[j].size())
			{
				for (int k = 0 ; k < loop[j].size() ; ++k) 
				{
					if (loop[i][k] != loop[j][k] )
					{
						//cout<<"Come in Please"<<endl;						
						j++;
						break;
					}					
					else if(k==loop[j].size()-1)
					{
						loop.erase(loop.begin()+j);				
					}
				}
			}
			else
				j++;

		}
		
	}
	cout<<loop.size()<<endl ;
	for(int i=0;i<loop.size();i++){
		for(int j=0;j<loop[i].size();j++)
			cout<<loop[i][j]<<" ";
		cout<<endl ;
	}
	
}
void delete_repeat(vector<vector <int> >&loop)
{
	for(int i=0;i<loop.size();i++)
	{
		int j=i+1;
		int n,m,k,t=0;
		while(j<loop.size())
		{
			if(loop[i].size()==loop[j].size())
			{
				n=0;
				k=0;
				t=0;
				m=k+1;
				while(n<loop[j].size())
				{	
					if(loop[i][k]==loop[j][m])
					{
						if(n==loop[j].size()-1)
						{
							loop.erase(loop.begin()+j);
							break;
						}
						k=(k+1)%loop[j].size();
						m=(m+1)%loop[j].size();
						n++;
					}
					else
					{
						m=(m+1)%loop[j].size();
						t++;
						n=0;
						if(t==loop[j].size()-1)
						{
							j++;
							break;
						}
					}			
				}
			}
			j++;
		}
	}
	cout<<"=====delete_repeat_loops======="<<endl;
	cout<<loop.size()<<endl ;
	for(int i=0;i<loop.size();i++){
		for(int j=0;j<loop[i].size();j++)
			cout<<loop[i][j]<<" ";
		cout<<endl ;
	}
}
void DFS_loop(vector<vector <int> >&output,int i,int j,bool *visit,int*A,vector<int>& temp,int max)
{
	if(visit[i])
	{	
		temp.push_back(i) ;
		/*for(int a=0;a<temp.size();a++)
			cout<<temp[a]<<" " ;
		cout<<endl;*/
		output.push_back(temp) ;
		//cout<<"I am a loop."<<endl ;
		temp.pop_back();
		return ;
	}
	else if(A[i*(max+1)+j]==1)
	{
		//cout<<"i is "<<i<<"  j is "<<j<<endl ;
		temp.push_back(i) ;
		visit[i] =true ;
		DFS_loop(output,j,0,visit,A,temp,max);
		temp.pop_back();
		visit[i] = false ;
	}
	if(j==max) return ;
	DFS_loop(output,i,j+1,visit,A,temp,max) ;
}

void DFS_lasso(vector<vector <int> >&output,int i,int j,bool *visit,int*A,vector<int>& temp,int max, int loop_first)
{
	if(visit[i] && i==loop_first)
	{	
		temp.push_back(i) ;
		/*for(int a=0;a<temp.size();a++)
			cout<<temp[a]<<" " ;
		cout<<endl;*/
		output.push_back(temp) ;
		//cout<<"I am a loop."<<endl ;
		temp.pop_back();
		return ;
	}
	else if(visit[i])
		return;
	else if(A[i*(max+1)+j]==1)
	{
		//cout<<"i is "<<i<<"  j is "<<j<<endl ;
		temp.push_back(i) ;
		visit[i]=true;
		DFS_lasso(output,j,0,visit,A,temp,max,loop_first);
		temp.pop_back();
		visit[i] = false ;
	}
	if(j==max) return ;
	DFS_lasso(output,i,j+1,visit,A,temp,max,loop_first) ;
}


void find_lasso(int *dirnew, vector< vector <int> >&lasso,vector< vector<int> > &loop, int max)
{
	vector <int> temp4;
	vector< vector <int> > all_lasso;
	int loop_first=0;
	int all_lasso_size=0;
	int shortest=0;
	bool visit[max+1];
	for(int i=0;i<loop.size();i++)
	{
		if(!all_lasso.empty())
			all_lasso.clear();
		for(int j=0;j<=max;j++)
		{
			visit[j] =false ;
		}
		loop_first=loop[i][0];
		visit[loop_first]=true;
		DFS_lasso(all_lasso,0,0,visit,dirnew,temp4,max,loop_first);
		/*for(int j=0;j<lasso[1].size();j++)
			cout<<lasso[1][j]<<" ";
		cout<<endl;*/
		
		all_lasso_size=all_lasso[0].size();	
		shortest=0;
		cout<<"=====find_all_lasso====="<<endl;
		for(int j=0;j<all_lasso.size();j++)
		{
			for(int k=0;k<all_lasso[j].size();k++)
			{
				cout<<all_lasso[j][k]<<" ";
			}
			if(all_lasso[j].size() <= all_lasso_size)
				{
					all_lasso_size=all_lasso[j].size();
					shortest=j;
				}
			cout<<endl ;

		}
		lasso.push_back(all_lasso[shortest]);
	}
}
/*
void min_mean_cycle(int* dir)
{
    for (int i=0; i<V+1; ++i)
        for (int j=0; j<V; ++j)
            d[i][j] = 1e9;
 
	for(int i=0;i<V;i++){
		for(int j=0;j<V;j++){
			w[i][j]=i-j ;
		}
	}
    int s = V;
    for (int i=0; i<V; ++i)
        w[s][i] = 0;
 
    d[0][s] = 0;
    for (int k=0; k<V; ++k){
        for (int i=0; i<=V; ++i){
            for (int j=0; j<=V; ++j){
                if (d[k][i] + w[i][j] < d[k+1][j]){
                    d[k+1][j] = d[k][i] + w[i][j];
					//cout<<"k+1 = "<<k+1<<"  "<<d[k+1][j]<<endl;
				}
			}
		}
	}
	for(int i=0;i<V;i++){
		for(int j=0;j<V;j++){

			cout<<d[i][j]<<" ";
		}
		cout<<endl ;
	}
 
    double mean = 1e9;
    for (int i=0; i<V-1; ++i)
    {
        if (d[V][i] == 1e9) continue;
 
        double maxw = -1e9;
        for (int k=0; k<V-1; ++k){
			cout<< (d[V][i] - d[k][i]) / (V - k) <<endl ;           
			maxw = max(maxw, (d[V][i] - d[k][i]) / (V - k));
		}
        mean = min(mean, maxw);
    }
 
    if (mean == 1e9)
        cout << "平均權重無限大";
    else
        cout << "平均權重為" << mean;
}
*/
int main(int argc, char *argv[])
{	
	char* file=argv[1];
	int max=find_max(file);
	char* limit = argv[2];
	vector <int> inc, exc ;
	LimitNode(limit,inc,exc) ;	
	int dir[max+1][max+1];
	//int **dir = (int**)new2d(max+1,max+1,sizeof(int)); //???
	vector <int> temp ;
	vector <vector <int> > output , loop , lasso;	
	memset(dir,0,sizeof(dir));
	build_graph(*dir,max);
	int dirnew[max+1][max+1];
	memset(dirnew,0,sizeof(dirnew));
	copy(*dir,*dirnew,max);
	bool visit[max+1] ;
	for(int i=0;i<=max;i++){
		visit[i] =false;
	}
	CancelNode(*dir,exc,max);//new
	DFS_loop(output,inc[0],0,visit,*dir,temp,max);
	LimitLoop(output,*dir,temp,max,inc,exc,loop);  //delete what we don't want
	delete_loop(output,loop,max);
	delete_repeat(loop);
	find_lasso(*dirnew,lasso,loop,max);
	cout<<"================"<<endl;
	print(lasso,loop);

	


	if(argv[3]!=NULL)
	{
		string loop_name = argv[3];
		int times = atoi(argv[4]);
		vector <string> action;
		delete_add(loop_name,action);//把加法去掉
		gen_adb(action,times,lasso,loop,inc);
	}
	
	/*for(int i=0;i<=max;i++)
	{
		for(int j=0;j<=max;j++)
		{
			if(dir[i][j]==1)
			{
				cout<<"dir["<<i<<"]["<<j<<"]="<<dir[i][j]<<endl;
			}
		}
	}
	

	ifstream is(file);
	is.open (file,std::ifstream::in);
	

	

	is.close();*/


}

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
#include <map>
#include "mincycle.h"
#include "kmin.cpp"
using namespace std;

//----

///./graph test.svm trace_num k

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

void build_graph(double **dir,int max)
{
	
	string temp;
	string grep_node = " grep '=' test.svm | sed s%state%%g|sed '1,2d' | sed s%=%%g | sed s%';'%%g | sed s%'{'%%g |sed s%'}'%%g | sed s%':'%%g | sed s%','%%g | sed s%'  '%' '%g |sed s%'   '%%g" ;

	vector<string> node_line =split(exec(grep_node),'\n');
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
			dir[first_num][second_num]=1;			
		 } 
	}
}

void print(vector<vector<int> > &lasso, vector<vector <int> > &loop)
{
	cout<<"lasso size="<<lasso.size()<<endl;
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
	//if(files.size()==1)//只選一個loop
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
	files_size=adb.size();
		os<<files_size<<" ";
		for(int i=0;i<files_size;i++)
		{
			os<<adb[i]<<" ";
		}
	os.close();
}

void DFS_lasso(vector<vector <int> >&output,vector<vector <int> >&loop,int i,int j,bool *visit,double **A,vector<int>& temp,int max, int num)
{
	//cout<<visit[i]<<endl;
	if(visit[i])
	{	
		for(int k=0;k<loop[num].size();k++)
		{
			if(i==loop[num][k])
			{
				temp.push_back(i) ;
				//for(int a=0;a<temp.size();a++)
					//cout<<temp[a]<<" " ;
				//cout<<endl;
				output.push_back(temp) ;
				//cout<<"I am a loop."<<endl ;
				temp.pop_back();
				return ;
			}	
		}
		return;
	}
	//else if(visit[i])
		//return;
	else if(A[i][j]!=1e9)
	{
		//cout<<"i is "<<i<<"  j is "<<j<<endl ;
		temp.push_back(i) ;
		visit[i]=true;
		DFS_lasso(output,loop,j,0,visit,A,temp,max,num);
		temp.pop_back();
		visit[i] = false ;
	}
	if(j==max) return ;
	DFS_lasso(output,loop,i,j+1,visit,A,temp,max,num) ;
}


void find_lasso(double **dirnew, vector< vector <int> >&lasso,vector< vector<int> > &loop, int max)
{
	vector <int> temp_lasso;
	vector< vector <int> > all_lasso;
	int loop_first=0;
	int all_lasso_size=0;
	int shortest=0;
	bool visit[max+1];
	for(int i=0;i<loop.size();i++)
	{
		if(!all_lasso.empty())
			all_lasso.clear();
		for(int j=0;j<max;j++)
		{
			visit[j] =false ;
		}
		/*for(int j=0;j<loop[i].size();j++)
		{
			visit[loop[i][j]]=true;
		}*/
		DFS_lasso(all_lasso,loop,0,0,visit,dirnew,temp_lasso,max,i);
		/*for(int j=0;j<all_lasso[1].size();j++)
			cout<<all_lasso[1][j]<<" ";
		cout<<endl;
		*/
		all_lasso_size=all_lasso[0].size();	
		shortest=0;
		cout<<"=====find_all_lasso====="<<endl;
		for(int j=0;j<all_lasso.size();j++)
		{
			for(int k=0;k<all_lasso[j].size();k++)
			{
				//cout<<all_lasso[j][k]<<" ";
			}
			if(all_lasso[j].size() < all_lasso_size)
			{
					all_lasso_size=all_lasso[j].size();
					shortest=j;
			}

		}
		lasso.push_back(all_lasso[shortest]);
		for(int j=0;j<lasso[i].size();j++)
			cout<<lasso[i][j]<<" ";
		cout<<endl;
	}
}

int filenum(string filename){
	int start = 0 ;
	for(int i=0;i<filename.size();i++)
	{
		if(filename[i]=='_')
		{
			start= i+1  ;

		}
		else if(filename[i]=='.')
		{
			return atoi(filename.substr(start,i-start+1).c_str());	
		}		
	}
}

void grep_mem(map<int,int> mem_temp, map<int,int> &mem_map)
{
	string grep_total;
	string ls = "ls mem";
	vector<string> file_name = split(exec(ls), '\n');
	for(int i=0;i<file_name.size();i++)
	{
		grep_total = "grep TOTAL mem/" + file_name[i] + " | sed -r \"s/[ ]+/ /g\" ";			
		vector<string> temp = split(exec(grep_total),' ');
		mem_temp[filenum(file_name[i])] = atoi(temp[2].c_str());
	}
	map<int,int>::iterator it2=mem_temp.begin();
	it2++;
	for(map<int, int>::iterator it = mem_temp.begin();it!=mem_temp.end(),it2!=mem_temp.end();it++,it2++)
	{		
		//cout<<it->first<<" ";
		//cout<<it->second<<" ";
		mem_map[it->first] = it->second - it2->second ;
		//cout<<mem_map[it->first]<<endl;
	}
}

string transition(string trace_num, int trans) //trace_num為資料夾名字
{
	string get_move;
	ifstream files;
	std::string trans_string= to_string(trans);
	//files.open(trans_string,std::ifstream::in);
	get_move = "grep " + trace_num + "_ /var/www/appeck/Beta/automata/tester1_android/dd/" + trans_string +" | sed s%" + trace_num + "\\_%%g";
	//exec(get_move);
	vector<string> move = split(exec(get_move),'\n');
	return move[0];
}

void state2move(char* trace_num, map<int,int> &mem_map,map<int,int> &state_move, int max)
{
	int state=0;
	for(int i=0;i<=max;i++)
	{
		if(i%2==0)
		{
			//state_move[i]=0;
		}
		else
		{
			state=(i+1)/2;
			state_move[i]=atoi(transition(trace_num,state).c_str());
			state_move[i]=mem_map[state_move[i]];
		}
	}
	for(map<int, int>::iterator it = state_move.begin();it!=state_move.end();it++)
	{
		//cout<<it->first<<" ";
		//cout<<it->second<<endl;
	}
}




void print_w(double **dir, int max, map<int,int> &state_move)
{
fstream fp;
    fp.open("delta_mem.txt", ios::out);

	for(int i=0;i<max;i++)
	{
		for(int j=0;j<max;j++)
		{
			if(dir[i][j]==1&&i%2==1)
			{
				dir[i][j]=state_move[i];//set weight to state
				
			}
			else if(dir[i][j]==1&&i%2==0)
			{
				dir[i][j]=0;//set 0 to connect
			}
			if(dir[i][j]!=1e9)
			{
				fp<<"w["<<i<<"]["<<j<<"]="<<dir[i][j]<<";"<<endl;
			}
		}
	}
	fp.close();
}


void gen_adb(int loop_num, int times, vector<vector <int> >output, vector<vector<int> >loop)
{
	ofstream os;
	os.open("loop",std::ofstream::out);
	vector<int> adb;
	for(int j=0;j<output[loop_num].size()-1;j++)
	{	
		adb.push_back(output[loop_num][j]);	
	}
	adb.push_back(-1);
	for(int k=0;k<times;k++)
	{
		for(int j=0;j<loop[loop_num].size();j++)
		{
			adb.push_back(loop[loop_num][j]);
		}
		adb.push_back(-1);
	}
	int files_size=adb.size();
	os<<files_size<<" ";
	for(int i=0;i<files_size;i++)
	{
		os<<adb[i]<<" ";
	}
	os.close();
}

int main(int argc, char *argv[])
{	
	char* file=argv[1];
	char* trace_num=argv[2];
	int k=atoi(argv[3]);
	int max=find_max(file)+1;
	//cout<<"max="<<max<<endl;
	map<int,int> mem_temp;
	map<int,int> state_move;
	map<int,int> mem_map;
	grep_mem(mem_temp,mem_map);
	state2move(trace_num,mem_map,state_move,max);
	double **dir,**d,**path;
	dir = new double *[max];
	for(int i=0;i<max;i++)
		dir[i]= new double[max];
	for(int i=0;i<max;i++)   //initialize 1e9 to all nodes in array
	{
		for(int j=0;j<max;j++)
		{
			dir[i][j]=1e9;
		}
	}
	cout<<"build memory gragh"<<endl;
	build_graph(dir,max);
	print_w(dir,max,state_move);
	vector<vector <int> > loop, lasso;

	Matrix current;
	current.initial(max,k);
	Matrix previous;
	previous.initial(max,k);
	vector<i_j_s_weight> candidate;
	
	
	current.build_zero(dir);
	cout<<"build k-min table"<<endl;
	for(int i=2;i<max;i++)
	{
		previous.clone(current);
		build_weight(current,dir,previous,i);
		//cout<<"build "<<i<<" table"<<endl;
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
	find_lasso(dir,lasso,loop,max);
	cout<<"================"<<endl;
	print(lasso,loop);

	if(argv[3]!=NULL)
	{
		int loop_num = atoi(argv[3]);
		int times = atoi(argv[4]);
		gen_adb(loop_num,times,lasso,loop);
	}
	return 0;
}

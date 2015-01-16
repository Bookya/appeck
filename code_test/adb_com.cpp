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
using namespace std;


//./adb_com loop 864


std::string exec(std::string cmd);
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

template <typename T>
string NumberToString(T pNumber)
{
 ostringstream oOStrStream;
 oOStrStream << pNumber;
 return oOStrStream.str();
}

void parse(string trace_num, vector <vector <string> > &corres)
{
	ifstream state;
	state.open("state",std::ifstream::in);
	string delete_num;
	delete_num ="grep " + trace_num + " state | sed s%" + trace_num + "_%%g";
	vector<string> corres_line =split(exec(delete_num),'\n');
	for(int i=0; i<corres_line.size();i++)
	{
		vector<string> temp=split(corres_line[i],' ');
		corres.push_back(temp);
	}
}

void parse_adb(string find_adb, vector <vector <string> > &text_adb)
{
	vector<string> several_adb=split(exec(find_adb),'\n');
	for(int i=0; i<several_adb.size();i++)
	{
		vector<string> temp=split(several_adb[i],'\n');
		text_adb.push_back(temp);
	}	
}
string transition(string trace_num, int trans)
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

int main(int argc, char *argv[])
{
	ifstream is;
	is.open(argv[1],std::ifstream::in);
	ofstream os;
	os.open("tempCE",std::ofstream::out);
	string trace_num=argv[2];
	vector <vector<string> > corres;
	parse(trace_num,corres);
	vector <vector <string> > text_adb;
	int nums=0;
	is>>nums;
	vector<string> files;
	int temp;
	os<<endl<<endl;
	bool first_temp = true ;
	for(int i=0;i<nums;i++)
	{
		is>>temp;
		if(temp==-1)
		{		
			if(first_temp)
			{
				os<<"adb shell dumpsys meminfo > mem.txt"<<endl;
				first_temp = false ;
			}				
			else
			{
				os<<"adb shell dumpsys meminfo >> mem.txt"<<endl;
			}
		}		
		else if(temp%2==0)
		{

		}
		else if(temp%2==1)
		{	
			temp=(temp+1)/2;
			temp=atoi(transition(trace_num,temp).c_str());
			text_adb.clear();
			if(temp!=0)
			{
			string find_adb="grep adb " + NumberToString(temp) +" | sed s%AndroidSystem%%g | sed s%\\:%%g | sed s%\\ adb%adb%g | sed s%^.*adb%adb%g ";
			//sed s%\"adb shell input keyevent 4\\$\"%%g
			os<<exec(find_adb);
			//parse_adb(find_adb,text_adb);
			/*for(int j=0;j<text_adb.size();j++)
			{
				for(int k=0;k<text_adb[j].size();k++)
				{
					os<<text_adb[j][k]<<endl;
				}
			}*/
			}
		}	
	}
	
	os.close();
	is.close();
return 0;
}

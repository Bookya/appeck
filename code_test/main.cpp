#include "tinyxml.h"
#include "tinystr.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <string>
using namespace std;


vector<string> split(const string &s, char delim);
vector<string>& split(const string &s, char delim, vector<string>& elems);
std::string exec(std::string cmd);
vector<string> ls(string dir);
//system.h


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

//system.h

const unsigned int NUM_INDENTS_PER_SPACE=2;

const char * getIndent( unsigned int numIndents )
{
	static const char * pINDENT="                                      + ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}

// same as getIndent but no "+" at the end
const char * getIndentAlt( unsigned int numIndents )
{
	static const char * pINDENT="                                        ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}

int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent)
{
	if ( !pElement ) return 0;

	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;
	const char* pIndent=getIndent(indent);
	printf("\n");
	while (pAttrib)
	{
		printf( "%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());

		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    printf( " int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) printf( " d=%1.1f", dval);
		printf( "\n" );
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;	
}

void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 )
{
	if ( !pParent ) return;

	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	printf( "%s", getIndent(indent));
	int num;

	switch ( t )
	{
	case TiXmlNode::TINYXML_DOCUMENT:
		printf( "Document" );
		break;

	case TiXmlNode::TINYXML_ELEMENT:
		printf( "Element [%s]", pParent->Value() );
		num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
		switch(num)
		{
			case 0:  printf( " (No attributes)"); break;
			case 1:  printf( "%s1 attribute", getIndentAlt(indent)); break;
			default: printf( "%s%d attributes", getIndentAlt(indent), num); break;
		}
		break;

	case TiXmlNode::TINYXML_COMMENT:
		printf( "Comment: [%s]", pParent->Value());
		break;

	case TiXmlNode::TINYXML_UNKNOWN:
		printf( "Unknown" );
		break;

	case TiXmlNode::TINYXML_TEXT:
		pText = pParent->ToText();
		printf( "Text: [%s]", pText->Value() );
		break;

	case TiXmlNode::TINYXML_DECLARATION:
		printf( "Declaration" );
		break;
	default:
		break;
	}
	printf( "\n" );
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		dump_to_stdout( pChild, indent+1 );
	}
}

void dump_to_stdout(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();
	if (loadOkay)
	{
		printf("\n%s:\n", pFilename);
		dump_to_stdout(&doc);
	}
	else
	{
		printf("Failed to load file \"%s\"\n", pFilename);
	}
}

bool xml2txt(string xml_dir,string txt_dir, vector<string> files)
{
	for(int i=0;i<files.size();i++)
	{
		FILE *fp;
		string txt_File = txt_dir + "/" + files[i];
		fp=freopen(txt_File.c_str(),"w",stdout);
		dump_to_stdout((xml_dir+"/"+files[i]+".xml").c_str());
		fclose(fp);
	}
	cout<<"Done"<<endl;
	return true;
}

bool is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}


void deadlock(string LTL_name, string txt_dir, vector<string> files)
{
		FILE *ptr;
		ptr=freopen(LTL_name.c_str(),"w",stdout);
		cout<<"G  (";
		for(int j=0;j<files.size();j++)
		{
			string class_value = "grep class " + txt_dir + "/" + files[j] +" | sed 's%+%%g' |sed s%:' '%_%g |sed s%=%%g | sed \"s%\\[%%g\" | sed s%\\]%%g | sed s%value%%g | sed s%[[:space:]]%%g";
			vector<string> class_name= split(exec(class_value), '\n');
			string bound_value= "grep bounds " + txt_dir+ "/" + files[j] + " | sed s%+%%g |sed s%:' '%_%g |sed \"s%\\=\\[%%g\" |sed s%]]%]_%g |sed s%value%%g | sed s%[[:space:]]%%g";
			vector<string> bound = split(exec(bound_value),'\n');
		

			for(int i=0;i<bound.size();i++)
			{
				bool temp = true ;
				for(int k=0;k<bound[i].size();k++){
					if(bound[i][k]=='['){
						if(temp){
							bound[i].insert(bound[i].begin()+k,'"');
							temp = false ;
							k++ ;
						}
						else temp = true ;				
					}
					else if(bound[i][k]==']'&&temp){
							bound[i].insert(bound[i].begin()+k+1,'"');
							k= bound[i].size();
					}
				}
				if(bound.size()==1)
				{
					cout<<"(  "<<bound[i]<<class_name[i]<<".clickable  )";
				}
				else if(i==0&&bound.size()!=1)
				{
					cout<<"(  "<<bound[i]<<class_name[i]<<".clickable  |";
				}
				else if(i==bound.size()-1&&i!=0)
				{
						cout<<"  "<<bound[i]<<class_name[i]<<".clickable  )";
				}			
				else
					cout<<"  "<<bound[i]<<class_name[i]<<".clickable  |";
			}
			if (j!=files.size()-1)
				cout<<"  |  ";
			else
				cout<<")";
			class_name.clear();
			bound.clear();
		}
		fclose(ptr);		
}
bool Search(string txt_dir,vector<string> files){
	for(int i=0;i<files.size();i++){
			string temp ;
			string txt_File = txt_dir + "/" + files[i];
			ifstream is ;
			is.open (txt_File.c_str(), std::ifstream::in);
			while(!is.eof()){
				getline(is,temp,' ');
				//cout<<temp<<endl ;
				string value = "responding" ;
				string value2 = "stopped" ;
				std::size_t found = temp.find(value);
				std::size_t found2 = temp.find(value2);
  				if(found!=std::string::npos){
						cout<<files[i]<<" is not responding"<<endl ;
						return true ;					
				}
				if(found2!=std::string::npos){
						cout<<files[i]<<" has stopped"<<endl ;
						return true ;
				}	
					
			}
			is.close();			
	}




}
int main(int argc, char *argv[])
{
	if(argc!=3)//判斷幾個input
	{
		cout<<"input error!"<<endl;
	}
	else
	{
		string xml_dir=argv[1];
		string ls = "ls "+ xml_dir+ "/uidump*.xml | sed \"s/\\.xml//g\" " ;
 	    vector<string> files = split(exec(ls), '\n');
		for(int i=0;i<files.size();i++)
		{			
			string temp=files[i];
			unsigned pos = temp.find("uidump");         // position of "live" in str
			files[i]=temp.substr(pos);
		}

		string txt_dir=argv[2];
		
						
		if(!is_dir(txt_dir.c_str()))//判斷txt dir是否存在
		{					
			string mkdir="mkdir -p "+txt_dir;
			exec(mkdir);
			cout<<"No txt_dir exists. Create "<< txt_dir<<"."<<endl;
			xml2txt(xml_dir,txt_dir,files);	//將xml轉成txt檔
		}
		cout<<"Making a LTL properties file."<<endl;
		deadlock("test",txt_dir,files);//輸出deadlock的LTL

		if(!Search(txt_dir,files))
		{
			cout<<"No warning founded."<<endl;
			
		}

		return 0;
	}
}


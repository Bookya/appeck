#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

int Au_transto_Nu(string automata_dir, string LTL_filename, string Nu_filename );

int str_to_Prop(string prop_str, string &prop_name, string &prop_type, bool &prop_value);

int prop_tocheck_xml(string xml_line, string prop_name, string prop_type, bool prop_value);

string get_xml_path(string user_app, string state_express );
//from construct.cpp
int read_trans(string trans_line, int &src_state, int &dst_state);
//int return trans's id
string get_transNum_path(string automata_dir, string trans_line);
string get_transNum_result(string trans_path); //the info that which step to find in result is already in path
//0109 in order to fill struct trans

struct state {
    string xml_path;
    //1207 id direct use vector #, trace_num & trace_step change to xml_path. Stick with trace.
    vector<int> next_id; //trans, 
};

struct trans {
    string trans_path;
    string ResultRecord;// 0109 modify appeck to put partial result in the front of each trans file
                        // 0_0[D]... use _ & [ to separate
                        // At this moment, still read result
    int next_id;//must only one
};

// $ ./transtoNu /var/www/appeck/Beta/automata/[user_apps]/[automata_dir name] LTL_1 Nu.svm
//                                         [1]                                  [2]    [3]
//in order to use the struct in testNu (testRec.cpp), maybe exec testNu when ./transtoNu
//two prog in one cycle
int main( int argc, char* argv[] ){
    if (argc!=4) { 
        cout <<"insufficient input or error input!"<<endl; 
        cout <<"usage: $ ./transtoNu [automata directory] [specify LTL's file name] [NuSMV input's file name]"<<endl;
        return 0; }
    else {    
        string a_dir = argv[1], l_dir = argv[2], n_dir = argv[3];
        Au_transto_Nu(a_dir, l_dir, n_dir);
        }
    return 0;
}






int Au_transto_Nu(string automata_dir, string LTL_filename, string Nu_filename ){
    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(automata_dir.c_str())) == NULL){
        cout << "Error(" << errno << ") opening " << automata_dir << endl;
        return errno; 
    }//automata doesn't exist

    //first, analyze LTL to determine proposition 
    //TODO 1208 name might have to change since class doesn't always holds
    vector<string> name;
    vector<string> type;
    vector<bool> value; 

    fstream LTL_f;
    LTL_f.open(LTL_filename.c_str(), ios::in);
    string LTL_str, temp_str, temp_name, temp_type;
    bool                      temp_value;
	int temp_int;
    getline(LTL_f, LTL_str);
    LTL_f.close();
//  <----------------------------LTL formula to property---------------------------------->
    //1203 ignore space, use _ to divide
    //var read
    // G, F, U, V, X, () and &, &&, |, ||, !, ->, --> ...
    //var out
    // =
    // don't have to do reasoning here, just get Prop, build map
    //1206 ! must stick with prop, not ().    
    size_t temp_found;
    vector<size_t> LTL_space_found;
    vector<size_t> LTL_quote_found;
    vector<size_t> LTL_prop_pos; //pos & len!!!

    //0113 find quote
    temp_found = LTL_str.find('\"'); 
    while (temp_found != string::npos) {
        LTL_quote_found.push_back(temp_found);
        temp_found = LTL_str.find('\"', LTL_quote_found[LTL_quote_found.size()-1]+1);
    }
    //for (int i=0; i<LTL_quote_found.size(); i++ ) cout<<LTL_quote_found[i]<<' ';
    //cout<<endl;

    //1205 require space @ each element (G, F, X, Prop, ->, &, |)
    //0222 still use 2space to distinct
    temp_found = LTL_str.find("  "); 
    while (temp_found != string::npos) {
        LTL_space_found.push_back(temp_found+1);
        temp_found = LTL_str.find("  ", LTL_space_found[LTL_space_found.size()-1]+1);
    }
    //for (int i=0; i<LTL_space_found.size(); i++ ) cout<<LTL_space_found[i]<<' ';
    //cout<<endl;

    //0113 eliminate space_found between quote_found
    temp_int=LTL_space_found.size()-1;
    for (int i=LTL_quote_found.size()-1; i>=0 && temp_int>=0; i=i-2) {
        while (LTL_space_found[temp_int]>LTL_quote_found[i]) temp_int--;
        if (LTL_space_found[temp_int]<LTL_quote_found[i-1]) ;
        else {
            LTL_space_found.erase(LTL_space_found.begin()+temp_int);
            temp_int--;
        }
    }
    //for (int i=0; i<LTL_space_found.size(); i++ ) cout<<LTL_space_found[i]<<' ';
    //cout<<endl;

    //TODO LTL check if legal
    //if (LTL_space_found.size()%2==0) { cout<<"LTL writing error!"<<endl; return -1; }

    temp_str = LTL_str.substr(0, LTL_space_found[0]);
    if (str_to_Prop(temp_str, temp_name, temp_type, temp_value)) {
        name.push_back(temp_name);
        type.push_back(temp_type);
        value.push_back(temp_value);

        LTL_prop_pos.push_back(0); 
        LTL_prop_pos.push_back(LTL_space_found[0]);
    }
    for (int i=0; i<LTL_space_found.size()-1; i++) {
        temp_str = LTL_str.substr(LTL_space_found[i]+1, LTL_space_found[i+1]-LTL_space_found[i]-1);
        if (str_to_Prop(temp_str, temp_name, temp_type, temp_value)) {
            name.push_back(temp_name);
            type.push_back(temp_type);
            value.push_back(temp_value);

            LTL_prop_pos.push_back(LTL_space_found[i]+1);
            LTL_prop_pos.push_back(LTL_space_found[i+1]-LTL_space_found[i]-1);
        }
    }
    temp_str = LTL_str.substr(LTL_space_found[LTL_space_found.size()-1]+1, LTL_space_found.size()-LTL_space_found[LTL_space_found.size()-1]);
    if (str_to_Prop(temp_str, temp_name, temp_type, temp_value)) {
        name.push_back(temp_name);
        type.push_back(temp_type);
        value.push_back(temp_value);

        LTL_prop_pos.push_back(LTL_space_found[LTL_space_found.size()-1]+1);
        LTL_prop_pos.push_back(LTL_space_found.size()-LTL_space_found[LTL_space_found.size()-1]);
    }

    //init prop-state table here TODO 0109 this will change or add Prop_trans since trans be introduce to NuSMV's state
    vector<int> oneProp_states;
    vector< vector<int> > allProp_states;
    for (int i=0; i<name.size(); i++) allProp_states.push_back(oneProp_states);
    //0114 one props have how much trans
    vector<int> oneProp_transs;
    vector< vector<int> > allProp_transs;

/*    cout<<name.size()<<endl;
    for (int i=0; i<name.size(); i++) {
        cout<<name[i]<<' '<<type[i]<<' '<<value[i]<<endl;
    }
    return 0;*/

//  <------------Read automata, build the state struct and coop with LTL prop------------->
    //1206 except exists, other prop type can be found in xml
    fstream state_f, trans_f, xml_f;
    vector<state> states;// {0, 2, 4, 6...}
    state temp_state;

    size_t found = automata_dir.find("automata/"),
           found2= automata_dir.find("/", found+9);
    string user_app = automata_dir.substr(found+9, found2-found-9), temp_path;

    if (automata_dir[automata_dir.size()-1]=='/') temp_path = automata_dir+"state";
    else                                          temp_path = automata_dir+"/state";

    state_f.open(temp_path.c_str(), ios::in);
    getline(state_f, temp_str);
    getline(state_f, temp_str);
    while (temp_str[0]!='\0'){
        temp_path = get_xml_path(user_app, temp_str);
        temp_state.xml_path = temp_path;
        states.push_back(temp_state);//real state id = vector# + 1 
        xml_f.open(temp_path.c_str(), ios::in);
        getline(xml_f, temp_str); 
        for (int i=0; i<name.size(); i++) //table record vector# !!!!!!!!!!
            if (prop_tocheck_xml(temp_str, name[i], type[i], value[i])) allProp_states[i].push_back(states.size()-1);
            
        xml_f.close();
        getline(state_f, temp_str); 
    }
    state_f.close();
/*
    cout<<states.size()<<endl;
    for (int i=0; i<states.size(); i++) cout<<states[i].xml_path<<endl;

    cout<<allProp_states.size()<<endl;
    for (int i=0; i<allProp_states.size(); i++) {
        cout<<"1P: "<<allProp_states[i].size()<<endl;
        for (int j=0; j<allProp_states[i].size(); j++)
            cout<<allProp_states[i][j]<<" ";
        cout<<endl;
    }
*/

    //TODO read trans
    //0109 modify next_id, state->transs->states
    //introduce vector<trans> transs
    vector<trans> transs;//{1, 3, 5, 7... }
    trans temp_trans;
    int trans_id, src, dst;
    bool if_du_trans;
    if (automata_dir[automata_dir.size()-1]=='/') temp_path = automata_dir+"trans";
    else                                          temp_path = automata_dir+"/trans";
    trans_f.open(temp_path.c_str(), ios::in);
    getline(trans_f, temp_str);
    getline(trans_f, temp_str);
    while (temp_str[0]!='\0'){
        trans_id = read_trans(temp_str, src, dst);
        temp_trans.trans_path=get_transNum_path(automata_dir, temp_str);
        temp_trans.next_id=dst;
        transs.push_back(temp_trans);
        states[src].next_id.push_back(trans_id-1);//already minus one @ read_trans
        //TODO Prop_trans
        getline(trans_f, temp_str);
    }
    trans_f.close(); //cout<<transs.size()<<endl;

    //0222 patch formula's trans here
    for (int i=0; i<name.size(); i++) 
        if (name[i]=="M") {
            for (int j=0; j<transs.size(); j++) {
                temp_path=automata_dir+"/"+to_string(j+1);
                trans_f.open(temp_path, ios::in);
                getline(trans_f, temp_str); //cout<<temp_str<<endl;
                if (temp_str.find(type[i])!=string::npos) allProp_states[i].push_back(j);
                trans_f.close();
            }
        }

/*
    cout<<states.size()<<endl;
    for (int i=0; i<states.size(); i++) {
        cout<<" sid"<<i<<endl;
        cout<<states[i].xml_path<<endl;
        for (int j=0; j<states[i].next_id.size(); j++){
            cout<<states[i].next_id[j]<<" ";
        }
        cout<<endl;
    }

    cout<<transs.size()<<endl;
    for (int i=0; i<transs.size(); i++) {
        cout<<" tid"<<i<<endl;
        cout<<transs[i].trans_path<<endl;
        cout<<transs[i].next_id<<endl;
    }
*/

    //generate svm file & exec Nusmv, get result.
    //1207 result to test case is another prog's bussiness
    //TODO 1208 define init state?
    int init_state = 0;
    fstream Nu_f;
    Nu_f.open(Nu_filename.c_str(), ios::out);
    Nu_f<<"MODULE main"<<endl; 
    Nu_f<<"VAR"<<endl;
        Nu_f<<" state : {";
        //0114 add transs to NuSMV's state, s->2X; t->2X+1
        for (int i=0; i<transs.size(); i++) Nu_f<<i*2+1<<", ";

        for (int i=0; i<states.size()-1; i++) Nu_f<<i*2<<", ";
        Nu_f<<states.size()*2-2<<"};"<<endl;

    Nu_f<<"ASSIGN"<<endl;
        Nu_f<<" init(state) := "<<init_state<<";"<<endl;
    Nu_f<<" next(state) :="<<endl;
    Nu_f<<"  case"<<endl;
        //0114 add transs to NuSMV's state
        for (int i=0; i<transs.size(); i++)
            Nu_f<<"   state = "<<i*2+1<<" : "<<transs[i].next_id*2<<";"<<endl;

        for (int i=0; i<states.size(); i++) {
            if (states[i].next_id.size()>1) {
                Nu_f<<"   state = "<<i*2<<" : {";
                for (int j=0; j<states[i].next_id.size()-1; j++) Nu_f<<states[i].next_id[j]*2+1<<", ";
                Nu_f<<states[i].next_id[states[i].next_id.size()-1]*2+1<<"};"<<endl;
            }
            else if (states[i].next_id.size()==1) {
                Nu_f<<"   state = "<<i*2<<" : "<<states[i].next_id[0]*2+1<<";"<<endl;
            }
        }
    Nu_f<<"   TRUE : state;"<<endl;
    Nu_f<<"  esac;"<<endl;
    Nu_f<<endl;

    //cout<<states.size()<<endl;
    for (int i=LTL_prop_pos.size()-2; i>=0; i=i-2) {
        //cout<<allProp_states[i/2].size()<<endl;
        if (allProp_states[i/2].size()==states.size()) temp_str="TRUE";
        else if (allProp_states[i/2].size()==0)        temp_str="FALSE";
        else if (name[i/2]=="M") {
            temp_str="(";
            for (int j=0; j<allProp_states[i/2].size()-1; j++)
                temp_str = temp_str + "state=" + to_string(allProp_states[i/2][j]*2+1) + " | ";
            temp_str = temp_str + "state=" + to_string(allProp_states[i/2][allProp_states[i/2].size()-1]*2+1) + ")";
        }
        else {
            temp_str="(";
            for (int j=0; j<allProp_states[i/2].size()-1; j++)
                temp_str = temp_str + "state=" + to_string(allProp_states[i/2][j]*2) + " | ";
            temp_str = temp_str + "state=" + to_string(allProp_states[i/2][allProp_states[i/2].size()-1]*2) + ")";
        }
        LTL_str.replace(LTL_prop_pos[i], LTL_prop_pos[i+1], temp_str);
    }
    Nu_f<<"LTLSPEC "<<LTL_str<<endl;
    Nu_f.close();

    //for (int i=0; i<LTL_prop_pos.size(); i++) cout<<LTL_prop_pos[i]<<endl;

    //exec NuSMV

    return 0;
}

//0113 allow text & bound, content modify from New message to "New message" 
//need vector<size_t> LTL_quote_found to identify above case.
int str_to_Prop(string prop_str, string &prop_name, string &prop_type, bool &prop_value){
    if (prop_str.size()<5 || prop_str[0]=='(' || prop_str[0]==')'
     || prop_str[0]=='X' || prop_str[0]=='F' || prop_str[0]=='G') return 0; //(G X F -> & |)'s size all <5
    else if (prop_str[0]=='M') {
        prop_name="M";
        prop_type=prop_str.substr(3, prop_str.length()-5);
        return 2;
    }

    size_t temp_found = prop_str.rfind(".");
    if (prop_str[0]=='!') {
        prop_value= 0;
        prop_name = prop_str.substr(1, temp_found-1);
    }
    else {
        prop_value= 1;
        prop_name = prop_str.substr(0, temp_found);
    }
    //eliminate last character space
    prop_type = prop_str.substr(temp_found+1, prop_str.size()-temp_found-2);
/*
    cout<<prop_name<<endl;
    cout<<prop_type<<endl;
    cout<<"pv"<<prop_value<<endl;
*/
    return 1;
}

//0222 note that if prop_name=="M", this func should no be called.
int prop_tocheck_xml(string xml_line, string prop_name, string prop_type, bool prop_value){
    size_t found_contain;
    if (prop_type=="contain"){
        found_contain = xml_line.find(prop_name);
        if (found_contain==string::npos) return !prop_value;
        else return prop_value;
    }

    //left_border might not be used
    size_t left_border, right_border,
           start_found,
           found = prop_name.find('_'),
           found2= prop_name.find('_', found+1);
    //string temp_str = prop_name.substr(0, found)+"=\""+prop_name.substr(found+1, found2-found-1)+"\"";
    string temp_str = prop_name.substr(0, found)+'='+prop_name.substr(found+1, found2-found-1);
    //cout<<temp_str<<endl;
    found = xml_line.find(temp_str);
    start_found = found;
    //0117 !exists should output all states not containing this property 
    if (found==string::npos) return !prop_value;
    //if (found==string::npos && prop_type=="exists")
    else { 
        left_border = xml_line.rfind("<node", found) /*start_found-5*/; //cout<<left_border<<endl;
        right_border = xml_line.find("/node>",found); //cout<<right_border<<endl;
        found = found2;
        //if contents have _ ? -> then need change divid word '_'
        found2= prop_name.find('_', found+1);
        temp_str = prop_name.substr(found+1, found2-found-1)+"=\""+prop_name.substr(found2+1, prop_name.size()-found2)+"\"";
        //cout<<" "<<temp_str<<endl;
        found = xml_line.find(temp_str, start_found);
        //cout<<left_border<<'_'<<found<<'_'<<right_border<<endl;
        //0117 change order of if & else if
        if (found!=string::npos && prop_type=="exists") return prop_value;
        else if (found==string::npos || found>right_border || found<left_border) return 0;
        else {
            if (prop_value) temp_str = prop_type+"=\"true\"";
            else temp_str = prop_type+"=\"false\"";
            temp_str = " "+temp_str+" ";
            //cout<<"  "<<temp_str<<endl;
            found = xml_line.find(temp_str, start_found);
            if (found==string::npos || found>right_border || found<left_border) return 0;
        }
    }
    return 1;
}

string get_xml_path(string user_app, string state_express ){
    if (state_express[0]=='[') return "null";
    
    string xml_path = "/var/www/appeck/Beta/upload/"+user_app+"/";
    int found = state_express.find(' '),
        found2= state_express.find('_');
    string temp_str = state_express.substr(found+1, found2-found-1);
    xml_path = xml_path + temp_str + "/uidump";
    found = state_express.find(' ', found2);
    temp_str = state_express.substr(found2+1, found-found2-1);
    xml_path = xml_path + temp_str +".xml";
    return xml_path;
}

int read_trans(string trans_line, int &src_state, int &dst_state){
    size_t found = trans_line.find(' '),
           found2= trans_line.find('_');
    string temp_str = trans_line.substr(0, found);
    int id = stoi(temp_str);
    //since state# is vector# + 1, here we minus one.
    temp_str = trans_line.substr(found+1, found2-found-1);
    src_state = stoi(temp_str) - 1;
    temp_str = trans_line.substr(found2+1, trans_line.size()-found2);
    dst_state = stoi(temp_str) - 1;//though there might be more than one trans record in one trans_id, stoi can handle 
    return id;
}

string get_transNum_path(string automata_dir, string trans_line) {
    if (trans_line[0]=='[') return "null"; //trans_line from "trans"

    size_t found = automata_dir.find("automata/"),
           found2= automata_dir.find("/", found+9);
    string user_app = automata_dir.substr(found+9, found2-found-9),
           temp_str,
           temp_path;

    found = trans_line.find(' ');
    temp_str = trans_line.substr(0, found);
    if (automata_dir[automata_dir.size()-1]=='/') temp_path = automata_dir+temp_str;
    else                                          temp_path = automata_dir+"/"+temp_str;

    fstream aTrans_f; //open the specify trans# file
    aTrans_f.open(temp_path.c_str(), ios::in);
    getline(aTrans_f, temp_str);
    getline(aTrans_f, temp_str);
    aTrans_f.close();
    found = temp_str.find('_');
    
    return "/var/www/appeck/Beta/upload/"+user_app+"/"+temp_str.substr(0, found)
                                                  +"/"+temp_str.substr(found+1, temp_str.size()-found);
}

string get_transNum_result(string trans_path) {
    
}

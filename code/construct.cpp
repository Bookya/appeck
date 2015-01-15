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

int build(string trace_dir, string automata_dir);
// @build, trace# must be found and maintained
int getdir(string dir, vector<string> &files);
// . & .. removed in getdir, int return how many xml files.
bool if_du_screen(string dir, string _2ndir);
// 0-> different, 1-> the same
bool if_du_action(string act1_re, string act2_re, int act1_num, int act2_num);
//bool if_du_action(string xml1_dir, string xml2_dir, string act1_re, string act2_re, int act1_num, int act2_num);
//xml_dir can be replace since one can get xml path by act_re 
string get_xml_path(string user_app, string state_express );
//state express stand for a line in file state
int get_move_number(string user_app, string automata_folder_name, string trans_express, string &result_path ); 
//trans_express stand for a line in file trans @ automata
int get_trace_num(string dir_path);
//0222 get nMotionIndex: 0, nViewIndex: 1
string get_nIndex (string user_app, int trace_num, int step_num );

struct automata_state {
    int id;// if using vector, this can be ignored?
    int trace_step_num;
    
};// file path doesn't introduce here

struct automata_trans {
    int id;
    int src_state, dst_state;
    
};// file path and bound pointer doesn't introduce here

struct transition_detail {

};

int main( int argc, char* argv[] ){
    if (argc!=3) { 
        cout <<"insufficient input or error input!"<<endl; 
        cout <<"usage: $ ./construct [trace directory] [automata folder name]"<<endl;
        return 0; }
    else {    
        string t_dir = argv[1], a_dir = argv[2];
        build(t_dir, a_dir);
        }
    return 0;
}





//  /appeck/Beta/automata/[username_appname]/[automata name]/state
//                                                           trans
//                                                           automata_info
//                                                           automata_record
int build(string trace_dir, string automata_dir){
    DIR *dp;//創立資料夾指標
    struct dirent *dirp;
    fstream t_f, a_f;
    int state_num, trans_num;//for info
    string user_app, temp_str, temp_path,
           automata_folder_name=automata_dir;
    
    if((dp = opendir(trace_dir.c_str())) == NULL){
        cout<<"error: trace doesn't exist!"<<endl;
        return 0; }

    size_t found = trace_dir.find("upload/");
    size_t found2 = trace_dir.find("/", found+7);
    user_app = trace_dir.substr(found+7, found2-found-7);
    if((dp = opendir(user_app.c_str())) == NULL){ //base folder doesn't build yet
        cout<<"aware: base folder doesn't exist, first time create."<<endl;
        mkdir(user_app.c_str(), S_IRWXU);
    }

    time_t now;
    time(&now);
    struct tm * timeinfo = localtime(&now);
    string output_time=asctime(timeinfo); output_time.pop_back();

    int trace_num; 
    if (trace_dir[trace_dir.size()-1]=='/') trace_dir.pop_back();
    found = trace_dir.find_last_of("/");
    temp_str = trace_dir.substr(found+1, trace_dir.size()-1-found);
    trace_num = stoi(temp_str);

    automata_dir=user_app+"/"+automata_dir;
    if((dp = opendir(automata_dir.c_str())) == NULL){ //automata doesn't build yet
        cout<<"aware: automata doesn't exist, first time create."<<endl;
        mkdir(automata_dir.c_str(), S_IRWXU);

        temp_path=automata_dir+"/state";
        a_f.open(temp_path.c_str(), fstream::in | fstream::out | fstream::app);
        a_f<<"[state id] [trace id_trace step]"<<endl;
        a_f.close();

        temp_path=automata_dir+"/trans";
        a_f.open(temp_path.c_str(), fstream::in | fstream::out | fstream::app);
        a_f<<"[transition id] [source state id_destination state id]"<<endl;
        a_f.close();

        temp_path=automata_dir+"/automata_info";
        a_f.open(temp_path.c_str(), fstream::in | fstream::out | fstream::app);
        a_f<<"[modify time]%[state number_transition_number]"<<endl;
        a_f.close();

        temp_path=automata_dir+"/automata_record";
        a_f.open(temp_path.c_str(), fstream::in | fstream::out | fstream::app);
        a_f<<"[modify time]%[incoming trace path]"<<endl;
        a_f<<output_time<<'%'<<trace_dir<<endl;
        a_f.close();
        
    }
    else { //already have automata, so does 4files and atNum. -> atNum need?
        temp_path=automata_dir+"/automata_record";
        a_f.open(temp_path.c_str(), ios::out|ios::app);
        a_f<<output_time<<'%'<<trace_dir<<endl;
        a_f.close();
    }
//  <--------------start construct state--------------------->  
    vector<string> files = vector<string>();
    vector<string> print_content = vector<string>();
    int xmlnum = getdir(trace_dir, files);
    vector<int> xml_mapto_stateid; // for trans construct's convenience
    string xml_a_path, xml_t_path;
    bool old_state_found;

    temp_path=automata_dir+"/state";
    a_f.open(temp_path.c_str(), ios::in);
    while (temp_str[0]!='\0'){
        getline(a_f, temp_str); print_content.push_back(temp_str);
    }
    a_f.close();
    print_content.pop_back();

    for (int i=1; i<=xmlnum; i++) { //trace -> state&trans
        if (print_content.size()==1) {// no record state yet 
            temp_str="1 "+to_string(trace_num)+"_1";
            print_content.push_back(temp_str);

            xml_mapto_stateid.push_back(1);
        }
        else {
            old_state_found=0;
            xml_t_path=trace_dir+"/uidump"+to_string(i)+".xml";
            for (int j=1; j<print_content.size(); j++) { // aware!
                xml_a_path=get_xml_path(user_app, print_content[j]);
                if (if_du_screen(xml_t_path, xml_a_path)) {
                    temp_str=print_content[j]+" "+to_string(trace_num)+"_"+to_string(i);
                    print_content[j]=temp_str;
                    old_state_found=1;

                    xml_mapto_stateid.push_back(j);
                }
            }
            if (old_state_found==0) {
                temp_str=to_string(print_content.size())+" "+to_string(trace_num)+"_"+to_string(i);
                print_content.push_back(temp_str);

                xml_mapto_stateid.push_back(print_content.size()-1);
            }
        }        
    }
    //state and trans are ios::out(overlay), info and record ios::app
    //for (int j=0; j<xml_mapto_stateid.size(); j++) cout<<xml_mapto_stateid[j]<<endl;
    //size of state = print_content.size()-1
    a_f.open(temp_path.c_str(), ios::out);
    for (int j=0; j<print_content.size(); j++) a_f<<print_content[j]<<endl;
    a_f.close();
    state_num = print_content.size() - 1;
    print_content.clear();

//  <--------------start construct trans--------------------->  
    bool old_trans_found;
    int check_step_num;
    vector<int> movenum_mapto_transid;
    string trace_result_path=trace_dir+"/result",
           atm_result_path;
    temp_path=automata_dir+"/trans";
    a_f.open(temp_path.c_str(), ios::in);
    while (temp_str[0]!='\0'){
        getline(a_f, temp_str); print_content.push_back(temp_str);
    }
    a_f.close();
    print_content.pop_back();
    
    for (int i=1; i<xmlnum; i++) {
        if (print_content.size()==1) {// no record trans yet 
            temp_str="1 "+to_string(xml_mapto_stateid[0])+"_"+to_string(xml_mapto_stateid[1]);
            print_content.push_back(temp_str);
            
            temp_path=automata_dir+"/1"; // trace#_step# info for each transition, 
                                         // can get the specify trans is which xml to xml
            a_f.open(temp_path.c_str(), ios::out);

//0222 try to put "nMotionIndex: 0, nViewIndex: 1" such info into each trans#

            a_f<<"[trace #]_[move #]"<<get_nIndex(user_app, trace_num, i)<<endl;
            a_f<<to_string(trace_num)<<"_"<<to_string(i)<<endl;
            a_f.close();

            movenum_mapto_transid.push_back(1);
        }
        else {
            old_trans_found=0;
            for (int j=1; j<print_content.size(); j++) {

                check_step_num=get_move_number(user_app, automata_folder_name, print_content[j], atm_result_path);

                if (if_du_action(trace_result_path, atm_result_path, i, check_step_num)) {//GGGGGGGGG
                    temp_str=print_content[j]+" "+to_string(xml_mapto_stateid[i-1])+"_"+
                                                  to_string(xml_mapto_stateid[i]);
                    print_content[j]=temp_str;
                    old_trans_found=1;

                    temp_path=automata_dir+"/"+to_string(j);
                    a_f.open(temp_path.c_str(), ios::out|ios::app);
                    a_f<<to_string(trace_num)<<"_"<<to_string(i)<<endl;
                    a_f.close();

                    movenum_mapto_transid.push_back(j);
                }
            }
            if (old_trans_found==0) {
                temp_str=to_string(print_content.size())+" "+to_string(xml_mapto_stateid[i-1])+"_"+
                                                             to_string(xml_mapto_stateid[i]);
                print_content.push_back(temp_str);

                temp_path=automata_dir+"/"+to_string(print_content.size()-1);
                a_f.open(temp_path.c_str(), ios::out);
                a_f<<"[trace #]_[move #]"<<get_nIndex(user_app, trace_num, i)<<endl;
                //a_f<<"[trace #]_[move #]"<<endl;
                a_f<<to_string(trace_num)<<"_"<<to_string(i)<<endl;
                a_f.close();

                movenum_mapto_transid.push_back(print_content.size()-1);
            }
        }
    }
    //for movenum_mapto_transid, create file for each transition
    temp_path=automata_dir+"/trans";
    a_f.open(temp_path.c_str(), ios::out);
    for (int j=0; j<print_content.size(); j++) a_f<<print_content[j]<<endl;
    a_f.close();
    trans_num = print_content.size() - 1;
    print_content.clear();

    //print info
    temp_path=automata_dir+"/automata_info";
    a_f.open(temp_path.c_str(), ios::out|ios::app);
    a_f<<output_time<<'%'<<state_num<<"_"<<trans_num<<endl;
    a_f.close();

    return 0;
}

string get_xml_path(string user_app, string state_express ){
    if (state_express[0]=='[') return "null";
    
    string xml_path = "../upload/"+user_app+"/";
    int found = state_express.find(' '),
        found2= state_express.find('_');
    string temp_str = state_express.substr(found+1, found2-found-1);
    xml_path = xml_path + temp_str + "/uidump";
    found = state_express.find(' ', found2);
    temp_str = state_express.substr(found2+1, found-found2-1);
    xml_path = xml_path + temp_str +".xml";
    return xml_path;
}

//need open the specify transiton file 1, 2...
// trans          -> specify 1
// 1 1_2             3_1
int get_move_number(string user_app, string automata_folder_name, string trans_express, string &result_path ){
    if (trans_express[0]=='[') return -1;

    result_path = "./"+user_app+"/"+automata_folder_name+"/";//still @ automata folder!
    int found=trans_express.find(' ');
    string temp_str = trans_express.substr(0, found);//trans id
    result_path = result_path + temp_str;
    
    fstream a_f;
    a_f.open(result_path.c_str(), ios::in);
    getline(a_f, trans_express);
    getline(a_f, trans_express);// get specify info, finally ZZzzz..
    a_f.close();
    
    result_path = "../upload/"+user_app+"/";
    found=trans_express.find('_');
    temp_str = trans_express.substr(0, found);//trace #
    result_path = result_path + temp_str + "/result";
        
    temp_str = trans_express.substr(found+1,trans_express.size());
    return stoi(temp_str);
}

int getdir(string dir, vector<string> &files){
    DIR *dp;//創立資料夾指標
    struct dirent *dirp;
    string temp; int count = 0;
    if((dp = opendir(dir.c_str())) == NULL){
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno; 
    }
    while((dirp = readdir(dp)) != NULL){//如果dirent指標非空
        temp=string(dirp->d_name); //res1.compare(res2)
        if (temp.find(".xml")!=string::npos) count++;
        if (!temp.compare(".")||!temp.compare("..")) /*cout<<temp<<endl*/;
        else files.push_back(string(dirp->d_name));//將資料夾和檔案名放入vector
    }
    closedir(dp);//關閉資料夾指標
    return count;
}

int get_trace_num(string dir_path){
    size_t found;
    if (dir_path[dir_path.size()-1]!='t') { //resul't'
        if (dir_path[dir_path.size()-1]=='/') dir_path.pop_back();
        found = dir_path.find_last_of("/");
        dir_path = dir_path.substr(found+1, dir_path.size()-1-found);
        return stoi(dir_path);
    }
    else if (dir_path[dir_path.size()-1]=='t') {
        dir_path.erase(dir_path.end()-7, dir_path.end()); 
        found = dir_path.find_last_of("/");
        dir_path = dir_path.substr(found+1, dir_path.size()-1-found);
        return stoi(dir_path);
    }
}

bool if_du_screen(string dir, string _2ndir){
    fstream f1, f2; string _1stxml, _2ndxml;
    char cs1[100], cs2[100]; strcpy(cs1,dir.c_str()); strcpy(cs2,_2ndir.c_str());
    f1.open(cs1, ios::in); getline(f1, _1stxml);
    f2.open(cs2, ios::in); getline(f2, _2ndxml);
    
    //0222 to fix the home screen's clock prob
    size_t found_home1=_1stxml.find("com.android.launcher"), 
           found_home2=_2ndxml.find("com.android.launcher");
    if (found_home1!=string::npos && found_home2!=string::npos) return 1;

    found_home1=_1stxml.find("com.android.systemui");
    found_home2=_2ndxml.find("com.android.systemui");
    if (found_home1!=string::npos && found_home2!=string::npos) return 1;

    found_home1=_1stxml.find("com.android.browser");
    found_home2=_2ndxml.find("com.android.browser");
    if (found_home1!=string::npos && found_home2!=string::npos) return 1;

    found_home1=_1stxml.find("package=\"android\"");
    found_home2=_2ndxml.find("package=\"android\"");
    if (found_home1!=string::npos && found_home2!=string::npos) return 1;

    found_home1=_1stxml.find("com.android.gallery");
    found_home2=_2ndxml.find("com.android.gallery");
    if (found_home1!=string::npos && found_home2!=string::npos) return 1;

    found_home1=_1stxml.find("com.android.contacts");
    found_home2=_2ndxml.find("com.android.contacts");
    if (found_home1!=string::npos && found_home2!=string::npos) return 1;

    //check xml content method, too weak
    for (int i=0; i<_1stxml.size(); i++) if (_1stxml[i]!=_2ndxml[i]) return 0;

    return 1;
}

// need add more compare besides index
// 0117 try to add function to compare trans file in trace.
bool if_du_action(string act1_re, string act2_re, int act1_num, int act2_num){
    string user_app, act1_xml_dir, act2_xml_dir; 
    size_t found = act1_re.find("upload/");
    size_t found2 = act1_re.find("/", found+7);
    user_app = act1_re.substr(found+7, found2-found-7);
    
    int act1_trace_num = get_trace_num(act1_re), 
        act2_trace_num = get_trace_num(act2_re),
        index1=0, index2=0;
    string line, line1, line2; //[D][getNextMove] nMotionIndex: 0, nViewIndex: 0

    act1_xml_dir= "../upload/"+user_app+"/"+to_string(act1_trace_num)+"/uidump"+to_string(act1_num)+".xml";
    act2_xml_dir= "../upload/"+user_app+"/"+to_string(act2_trace_num)+"/uidump"+to_string(act2_num)+".xml";
    if (!if_du_screen(act1_xml_dir, act2_xml_dir)) return 0;//screen different, action different

    //1122 meeting increase
    act1_xml_dir= "../upload/"+user_app+"/"+to_string(act1_trace_num)+"/uidump"+to_string(act1_num+1)+".xml";
    act2_xml_dir= "../upload/"+user_app+"/"+to_string(act2_trace_num)+"/uidump"+to_string(act2_num+1)+".xml";
    if (!if_du_screen(act1_xml_dir, act2_xml_dir)) return 0;//dst screen different, action different

    fstream f1, f2; string re1, re2; // may change when result's form change!!!!

    if (act1_trace_num==act2_trace_num){ // case: two act in the same trace
        f1.open(act1_re.c_str(), ios::in);
        getline(f1, line);
        while (/*line[0]!='\0'*/!f1.eof()) {
            if (line[0]=='M') {
                index1++;
                if (index1==act1_num) {
                    getline(f1, line); getline(f1, line);
                    line1 = line;
                }
                if (index1==act2_num) {
                    getline(f1, line); getline(f1, line);
                    line2 = line;
                }
            }
            getline(f1, line);
        }
        f1.close();
//cout<<index1<<' '<<act1_num<<' '<<line1<<endl;
//cout<<index1<<' '<<act2_num<<' '<<line2<<endl;
        if (line1.compare(line2)) return 0;
        else return 1;
    }
    else { 
        f1.open(act1_re.c_str(), ios::in);
        getline(f1, line);
        while (!f1.eof()) {
            if (line[0]=='M') {
                index1++;
                if (index1==act1_num) {
                    getline(f1, line); getline(f1, line);
                    line1 = line;
                }
            }
            getline(f1, line);
        }
        f1.close();

        f2.open(act2_re.c_str(), ios::in);
        getline(f2, line);
        while (!f2.eof()) {
            if (line[0]=='M') {
                index2++;
                if (index2==act2_num) {
                    getline(f2, line); getline(f2, line);
                    line2 = line;
                }
            }
            getline(f2, line);
        }
        f2.close();
        if (line1.compare(line2)) return 0;
        else return 1;
    }
}

string get_nIndex (string user_app, int trace_num, int step_num ) {
    string Tresult_path="../upload/"+user_app+"/"+to_string(trace_num)+"/result",
           temp_str;
    int index=0;

    fstream f_result;
    f_result.open(Tresult_path, ios::in);

    //0327 when file doesn't exist
    if (f_result.is_open()) ;
    else return "null";

    getline(f_result, temp_str);
    while (!f_result.eof()){
        if (temp_str[0]=='M') {
            index++;
            if (index==step_num) {
                getline(f_result, temp_str); getline(f_result, temp_str);
                if (temp_str[0]=='[') return temp_str.substr(17, temp_str.length()-17);
                else return temp_str;
            }
        }
        getline(f_result, temp_str);
    }

}

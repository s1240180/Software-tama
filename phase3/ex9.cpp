#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <algorithm>
#include <iomanip>

#include <sqlite3.h>

#define K 20
using namespace std;

struct PAGE{
  string name;
  vector<string> link;
  vector<string> word;
  double page_score;
  double word_score;
};

void getHTMLFileName(string dir,vector<string> &filename){
  DIR *dp;
  dirent *entry;
  
  dp = opendir(dir.c_str());

  if(dp == NULL)
    cout << "Directory was not found" << endl;
  else{
    entry = readdir(dp);
    while(entry != NULL){
      string sample = entry->d_name;
      if(sample.find(".html") != string::npos && sample[sample.size() - 1] == 'l')
	filename.push_back(sample);
      entry = readdir(dp);
    }
  }
}

void InitSet(string dir,vector<string> filename,vector<PAGE> &DATA){
  ifstream fin;

  for(vector<string>::size_type i = 0;i < filename.size();i++){
    PAGE in;
    string text;
    
    fin.open((dir + "/" + filename[i]).c_str());
    in.name = filename[i];
    in.page_score = 1.0;
    in.word_score = 0;
    
    while(fin >> text){
      if(text.find(".html") == string::npos)
	in.word.push_back(text);
      else
	in.link.push_back(text);
    }
    
    DATA.push_back(in);
    fin.close();
  }
}

double sum(string name,vector<PAGE> &DATA){
  double result = 0;
  
  for(vector<PAGE>::size_type i = 0;i < DATA.size();i++)
    for(vector<string>::size_type j = 0;j < DATA[i].link.size();j++)
      if(name == DATA[i].link[j])
	result += DATA[i].page_score / DATA[i].link.size();
  
  return result; 
}

void calculate(vector<PAGE> &DATA){
  for(int i = 0;i < K;i++)
    for(vector<PAGE>::size_type j = 0;j < DATA.size();j++)
      DATA[j].page_score = 0.15 + 0.85 * sum(DATA[j].name,DATA);
}

bool compare_ws(const PAGE &d1,const PAGE &d2){
  return d1.word_score > d2.word_score;
}

bool compare_ps(const PAGE &d1,const PAGE &d2){
  return d1.page_score > d2.page_score;
}

bool compare_ts(const PAGE &d1,const PAGE &d2){
  return d1.page_score + d1.word_score > d2.page_score + d2.word_score;
}

void result_out(vector<PAGE> &DATA){
  sort(DATA.begin(),DATA.end(),compare_ts);
  cout << "word" << "   " << "page" << "   " << "socre" << endl;
  for(vector<PAGE>::size_type i = 0;i < DATA.size();i++){
    cout << fixed << setprecision(2) << DATA[i].word_score << " : ";
    cout << fixed << setprecision(2) << DATA[i].page_score << " : ";
    cout << fixed << setprecision(2) << DATA[i].word_score + DATA[i].page_score << " : ";
    cout << DATA[i].name << endl;
  }
}

void search_word(vector<string> &word){
  string line;
  string::size_type s = 0;
  string::iterator iter;
  
  cout << "input word" << endl;
  getline(cin,line);

  for(string::size_type i = 0;i < line.size();i++){
    if(line[i] == ' '){
      bool same = false;
      for(vector<string>::size_type j = 0;j < word.size();j++)
	if(word[j] == line.substr(s,i - s))
	  same = true;
      if(!same)
	word.push_back(line.substr(s,i - s));
      s = i + 1;
    }
    
    if(i == line.size() - 1){
      bool same = false;
      for(vector<string>::size_type j = 0;j < word.size();j++)
	if(word[j] == line.substr(s,i + 1 - s))
	  same = true;
      if(!same)
	word.push_back(line.substr(s,i + 1 - s));
    }
  }
  
}

void word_score(vector<string> word,vector<PAGE> &DATA){
  for(vector<PAGE>::size_type i = 0;i < DATA.size();i++)
    for(vector<string>::size_type j = 0;j < DATA[i].word.size();j++)
      for(vector<string>::size_type k = 0;k < word.size();k++)
	if(DATA[i].word[j] == word[k])
	  DATA[i].word_score += 1.0;
}

void normalize(vector<PAGE> &DATA){
  double word_max,page_max;
  
  sort(DATA.begin(),DATA.end(),compare_ws);
  word_max = DATA[0].word_score;
  for(vector<PAGE>::size_type i = 0;i < DATA.size();i++)
    DATA[i].word_score = DATA[i].word_score / word_max;

  sort(DATA.begin(),DATA.end(),compare_ps);
  page_max = DATA[0].page_score;
  for(vector<PAGE>::size_type i = 0;i < DATA.size();i++)
    DATA[i].page_score = DATA[i].page_score / page_max;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName){
  for(int i = 0; i < argc - 1;i++)
    printf("%s|", argv[i]);
  printf("%s\n",argv[argc - 1]);
  return SQLITE_OK;
}

vector<string> CreateWordList(vector<PAGE> &DATA){
  vector<string> word_list;
  
  for(vector<PAGE>::size_type i = 0;i < DATA.size();i++){
    for(vector<string>::size_type j = 0;j < DATA[i].word.size();j++){
      bool check = false;
      for(vector<string>::size_type k = 0;k < word_list.size();k++){
	if(word_list[k] == DATA[i].word[j]){
	  check = true;
	  break;
	}
      }
      if(!check)
	word_list.push_back(DATA[i].word[j]);
    }
  }

  return word_list;
}

void SQL_create_table(sqlite3 *db){
  char *error = 0;
  sqlite3_exec(db,"create table url (url varchar(100))",NULL,NULL,&error);
  sqlite3_exec(db,"create table word (word varchar(100))",NULL,NULL,&error);
  sqlite3_exec(db,"create table link (source varchar(100), target varchar(100))",NULL,NULL,&error);
  sqlite3_exec(db,"create table location (wordid varchar(100), urlid varchar(100))",NULL,NULL,&error);
  sqlite3_exec(db,"create table rank(pageid varchar(100), score float)",NULL,NULL,&error);
}

void SQL_insert_table(sqlite3 *db,vector<string> &filename,vector<PAGE> &DATA){
  string order;
  vector<string> word_list;
  char *error = 0;
  
  for(vector<string>::size_type i = 0;i < filename.size();i++){
    order = "insert into url(url) values (\"" + filename[i] + "\")";
    sqlite3_exec(db, order.c_str(), NULL, NULL, &error);
  }

  word_list = CreateWordList(DATA);
  for(vector<string>::size_type i = 0;i < word_list.size();i++){
    order = "insert into word(word) values (\"" + word_list[i] + "\")";
    sqlite3_exec(db, order.c_str(), NULL, NULL, &error);
  }

  
  for(vector<PAGE>::size_type i = 0;i < DATA.size();i++){
    for(vector<string>::size_type j = 0;j < DATA[i].link.size();j++){
      order = "insert into link(source, target) values (\"" + DATA[i].name + "\",\"" + DATA[i].link[j] + "\")";
      sqlite3_exec(db, order.c_str(), NULL, NULL, &error);
    }
  }

  for(vector<string>::size_type k = 0;k < word_list.size();k++){
    for(vector<PAGE>::size_type i = 0;i < DATA.size();i++){
      for(vector<string>::size_type j = 0;j < DATA[i].word.size();j++){
	if(word_list[k] == DATA[i].word[j]){
	  order = "insert into location (wordid, urlid) values(\"" + word_list[k] + "\",\"" + DATA[i].name + "\")";
	  sqlite3_exec(db, order.c_str(), NULL, NULL, &error);
	}
      }
    }
  }

  for(vector<PAGE>::size_type i = 0;i < DATA.size();i++){
    order = "insert into rank (pageid,score) values(\"" + DATA[i].name + "\"," + to_string(DATA[i].page_score) + ")";
    sqlite3_exec(db, order.c_str(), NULL, NULL, &error);
  }
  
}

void SQL_select_from_table(sqlite3 *db){
  char *error = 0;

  cout << endl << "select ROWID, * from url" << endl;
  sqlite3_exec(db,"select ROWID, * from url",callback,NULL,&error);
  cout << endl << "select ROWID, * from word" << endl;
  sqlite3_exec(db,"select ROWID, * from word",callback,NULL,&error);
  cout << endl << "select ROWID, * from link" << endl;
  sqlite3_exec(db,"select ROWID, * from link",callback,NULL,&error);
  cout << endl << "select ROWID, * from location" << endl;
  sqlite3_exec(db,"select ROWID, * from location",callback,NULL,&error);
  cout << endl << "select ROWID, * from rank" << endl;
  sqlite3_exec(db,"select ROWID, * from rank",callback,NULL,&error);
  cout << endl;
}

int main(){
  vector<string> filename;
  vector<PAGE> DATA;
  vector<string> input_word;
  sqlite3 *db;
  sqlite3_open("recom_info.db",&db);
 
  search_word(input_word);
  getHTMLFileName("source",filename);
  InitSet("source",filename,DATA);
  calculate(DATA);
  word_score(input_word,DATA);
  normalize(DATA);
  result_out(DATA);
  
  SQL_create_table(db);
  SQL_insert_table(db,filename,DATA);
  SQL_select_from_table(db);

  sqlite3_close(db);
  return 0;
}

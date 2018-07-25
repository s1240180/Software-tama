#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <algorithm>

#define K 20

using namespace std;

struct PAGE{
  string name;
  vector<string> link;
  vector<string> word;
  double score;
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
      if(sample.find(".html") != string::npos)
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
    in.score = 1.0;
    
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
	result += DATA[i].score / DATA[i].link.size();
  
  return result; 
}

void calculate(vector<PAGE> &DATA){
  for(int i = 0;i < K;i++)
    for(vector<PAGE>::size_type j = 0;j < DATA.size();j++)
      DATA[j].score = 0.15 + 0.85 * sum(DATA[j].name,DATA);
}

bool compare_score(const PAGE &d1,const PAGE &d2){
  return d1.score > d2.score;
}

void result_out(vector<PAGE> &DATA){
  sort(DATA.begin(),DATA.end(),compare_score);
  for(vector<PAGE>::size_type i = 0;i < DATA.size();i++)
    cout << DATA[i].score << " " << DATA[i].name << endl;
}

int main(){
  vector<string> filename;
  vector<PAGE> DATA;

  getHTMLFileName("source",filename);
  InitSet("source",filename,DATA);
  calculate(DATA);
  result_out(DATA);
  
  return 0;
}

#include <fstream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

int Range_Rand(int min,int max){
  if(max < min)
    swap(max,min);
  
  return rand() % (max - min + 1) + min;
}

int main(){
  srand((unsigned) time(NULL));
  int NUM = Range_Rand(1,1000);
  int W,U;
  ofstream fout;
  ifstream fin;
  string tmp;
  string name;
  vector<string> word;

  fin.open("wordlist.txt");
  while(fin >> tmp)
    word.push_back(tmp);
  fin.close();
  
  for(int c = 0;c < NUM;c++){
    name = "data_ex7/" + word[Range_Rand(0,word.size() - 1)] + ".html";
    fout.open(name.c_str());

    U = Range_Rand(1,10);
    for(int i = 0;i < U;i++){
      string d = word[Range_Rand(0,word.size() - 1)];
      string d2 = Range_Rand(0,2) == 0 ? d + ".html" : d; 
      fout << d2 << endl;
    }

    fout.close();
  }
  
  return 0;
}


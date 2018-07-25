#include <fstream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

#define NUM 5

using namespace std;

struct DATA{
  int user;
  int book;
  double eval;
}; 

int Range_Rand(int min,int max){
  if(max < min)
    swap(max,min);
  
  return rand() % (max - min + 1) + min;
}

bool compare(const DATA d1,const DATA d2){
  if(d1.user != d2.user)
    return d1.user < d2.user;
  else
    return d1.book < d2.book;
}

int main(){
  srand((unsigned) time(NULL));
  int N,M,E;
  ofstream fout;
  stringstream ss;
  vector<DATA> data;
  
  for(int c = 0;c < NUM;c++){
    N = Range_Rand(2,100000);
    M = Range_Rand(1,100000);
    E = Range_Rand(0,100000);

    for(int i = 0;i < E;i++){
      DATA tmp;
      tmp.user = Range_Rand(1,N);
      tmp.book = Range_Rand(1,M);
      tmp.eval = (double)Range_Rand(0,50) / 10;
      data.push_back(tmp);
    }

    sort(data.begin(),data.end(),compare);

    ss << "data_ex3&4/data" << c + 1 << ".txt";
    fout.open(ss.str().c_str());

    fout << N << " " << M << " " << E << endl;
    
    for(vector<DATA>::size_type i = 0;i < data.size();i++)
      fout << data[i].user << " " << data[i].book << " " << fixed << setprecision(1) << data[i].eval << endl;

    fout.close();
    ss.str("");
    data.clear();
  }
  
  return 0;
}


#include <fstream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

#define NUM 5

using namespace std;

int Range_Rand(int min,int max){
  if(max < min)
    swap(max,min);
  
  return rand() % (max - min + 1) + min;
}

int main(){
  srand((unsigned) time(NULL));
  int N,M,E,R;
  ofstream fout;
  stringstream ss;
  
  for(int c = 0;c < NUM;c++){
    N = Range_Rand(1,100000);
    M = Range_Rand(1,100000);
    E = Range_Rand(1,100000);
    R = Range_Rand(1,100000);
    
    ss << "data_ex6/data" << c + 1 << ".txt";
    fout.open(ss.str().c_str());

    fout << N << " " << M << " " << E << endl;
    for(int i = 0;i < E;i++){
      fout << Range_Rand(1,N) << " " << Range_Rand(1,M) << " ";
      fout << fixed << setprecision(1) << (double)Range_Rand(0,50) / 10 << endl;
    }
      
    fout << R << endl;
    for(int i = 0;i < R;i++)
      fout << Range_Rand(1,N) << " " << Range_Rand(1,N) << endl;

    fout.close();
    ss.str("");
  }
  
  return 0;
}


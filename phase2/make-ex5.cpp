#include <fstream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

#define NUM 5

using namespace std;

struct two_int{
  int d1;
  int d2;
}; 

int Range_Rand(int min,int max){
  if(max < min)
    swap(max,min);
  
  return rand() % (max - min + 1) + min;
}

int main(){
  srand((unsigned) time(NULL));
  int N,R,Q;
  ofstream fout;
  stringstream ss;
  vector<two_int> r_data;
  vector<two_int> q_data;
  
  for(int c = 0;c < NUM;c++){
    N = 100000;//Range_Rand(2,100000);
    R = 100000;//Range_Rand(1,100000);
    Q = 100000;//Range_Rand(1,100000);

    for(int i = 0;i < R;i++){
      two_int tmp;
      tmp.d1 = Range_Rand(1,N);
      tmp.d2 = Range_Rand(1,N);
      r_data.push_back(tmp);
    }

    for(int i = 0;i < Q;i++){
      two_int tmp;
      tmp.d1 = Range_Rand(1,N);
      tmp.d2 = Range_Rand(1,N);
      q_data.push_back(tmp);
    }

    ss << "data_ex5/data" << c + 1 << ".txt";
    fout.open(ss.str().c_str());

    fout << N << " " << R << endl;
    for(vector<two_int>::size_type i = 0;i < r_data.size();i++)
      fout << r_data[i].d1 << " " << r_data[i].d2 << endl;
    
    fout << Q << endl;
    for(vector<two_int>::size_type i = 0;i < q_data.size();i++)
      fout << q_data[i].d1 << " " << q_data[i].d2 << endl;

    fout.close();
    ss.str("");
    r_data.clear();
    q_data.clear();
  }
  
  return 0;
}


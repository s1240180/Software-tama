#include <fstream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

int Range_Rand(int min,int max){
  if(max < min)
    swap(max,min);
  
  return rand() % (max - min + 1) + min;
}

int main(){
  srand((unsigned) time(NULL));
  int N;
  int M;
  double **eval;
  ofstream fout;
  stringstream ss;
  
  for(int c = 0;c < 5;c++){
    N = Range_Rand(2,100);
    M = Range_Rand(1,100);
    
    eval = (double**)malloc(sizeof(double*) * N);
    for(int i = 0;i < N;i++)
      eval[i] = (double*)malloc(sizeof(double) * M);

    for(int i = 0;i < N;i++)
      for(int j = 0;j < M;j++)
	eval[i][j] = Range_Rand(0,9) == 0 ? -1.0 : (double)Range_Rand(0,50) / 10;

    ss << "data_ex1&2/data" << c + 1 << ".txt";
    fout.open(ss.str().c_str());

    fout << N << " " << M << endl;
    
    for(int i = 0;i < N;i++){
      for(int j = 0;j < M;j++)
	fout << fixed << setprecision(1) << eval[i][j] << " ";
      fout << endl;
    }
    
    fout.close();
    ss.str("");
    for(int i = 0;i < N;i++)
      free(eval[i]);
    free(eval);
  }
  
  return 0;
}


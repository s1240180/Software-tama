#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>

#define MAX 100

using namespace std;

struct Book{
  double Score;
  int num;
};

bool compare_book(Book,Book);

int main(){
  int N,M;
  double e[MAX][MAX] = {-1};
  double d[MAX] = {0};
  double s[MAX] = {0};
  double r[MAX][MAX] = {0};
  double R[MAX] = {0};
  double S[MAX] = {0};

  vector<Book> B;
  string space_2(2,' ');
  string space_4(4,' ');
  string space_8(8,' ');
  string space_18(18,' ');
  
  cout << "input the number of users(2 ~ 100):" << endl;
  cin >> N;
  cout << "input the number of books(1 ~ 100):" << endl;
  cin >> M;

  cout << "input the data" << endl;
  for(int i = 0;i < N;i++)
    for(int j = 0;j < M;j++)
      cin >> e[i][j];

  for(int i = 1;i < N;i++){
    for(int j = 0;j < M;j++)
      if((0 <= e[i][j] && e[i][j] <= 5.0) && (0 <= e[0][j] && e[0][j] <= 5.0))
	d[i] += pow(e[0][j] - e[i][j],2);
    d[i] = sqrt(d[i]);
    s[i] = 1 / (d[i] + 1);
  }

  for(int i = 1;i < N;i++){
    for(int j = 0;j < M;j++){
      if(e[0][j] == -1.0 && 0 <= e[i][j] && e[i][j] <= 5)
	r[i][j] = e[i][j] * s[i];
      R[j] += r[i][j];
      if(!(r[i][j] == 0))
	S[j] += s[i];
    }
  }

  /*for(int i = 0;i < N;i++){
    if(i == 0){
      cout << "       ";
      cout << "Score" << space_2;
    }
    else{
      cout << "user" << i + 1 << space_2;
      cout << fixed << setprecision(2) << s[i] << space_2;
    }
    
    for(int j = 0;j < M;j++){
      if(i == 0){
	cout << "B" << j + 1 << space_4;
	cout << "R" << j + 1 << space_4;
      }
      else{
	if(0 <= e[i][j] && e[i][j] <= 5)
	  cout << fixed << setprecision(2) << e[i][j] << space_2;
	else	  
	  cout <<  "----" << space_2;
	
	if(r[i][j] == 0)
	  cout <<  "----" << space_2;
	else
	  cout << fixed << setprecision(2) << r[i][j] << space_2;
      }
    }
    cout << endl;
  }

  cout << "S" << space_18;
  for(int j = 0;j < M;j++){
    if(R[j] == 0)
      cout <<  "----" << space_8;
    else
      cout << fixed << setprecision(2) << R[j] << space_8;
  }
  cout << endl;

  cout << "A" << space_18;
  for(int j = 0;j < M;j++){
    if(S[j] == 0)
      cout <<  "----" << space_8;
    else
      cout << fixed << setprecision(2) << S[j] << space_8;
  }
  cout << endl;

  cout << "S / A" << "              ";
  for(int j = 0;j < M;j++){
    if(S[j] == 0)
      cout <<  "----" << space_8;
    else
      cout << fixed << setprecision(2) << R[j] / S[j] << space_8;
  }
  cout << endl << endl;*/


  for(int j = 0;j < M;j++){
    Book data;
    if(S[j] != 0){
      data.Score = R[j] / S[j];
      data.num = j + 1;
      B.push_back(data);
    }
  }

  sort(B.begin(),B.end(),compare_book);

  cout << "[Result]" << endl;
  for(vector<Book>::size_type i = 0;i < B.size();i++)
    cout << B[i].num << " " << B[i].Score << endl;
  
  return 0;
}

bool compare_book(const Book b1,const Book b2){
  return b1.Score > b2.Score;
}

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <queue>
using namespace std;

struct DATA {
  int user;
  int book;
  double eval;
};

struct RES {
  int num;
  double score;
};

void read_data(DATA&, vector<DATA>&,bool [100001]);
void calculate(const DATA, vector<DATA>&,bool [100001]);
bool compare(const RES, const RES);
typedef vector<DATA>::size_type idx;
void f_num(bool [100001]);

int main() {
  DATA max;
  vector<DATA> data;
  bool used[100001]={};
  read_data(max, data, used);
  calculate(max, data, used);

  return 0;
}

bool compare(const RES d1, const RES d2) {
  return d1.score > d2.score;
}

void calculate(const DATA max, vector<DATA> &data, bool used[100001]) {
  vector<DATA> user1_data;
  vector<RES> result;
  vector<double> score(max.user);
  vector<double> dist(max.user, 0);
  vector<double> recomm(max.book, 0);
  vector<double> S(max.book, 0);
  bool user1_read[100001]={};
  bool b[100001];

  
  for (idx i = 0; i < data.size(); i++)
    if (data[i].user == 1)
      user1_data.push_back(data[i]);


  for (idx i = 0; i < data.size(); i++) {
    if (data[i].user != 1 && used[data[i].user]==1) {//
      for (idx j = 0; j < user1_data.size(); j++)
	if (user1_data[j].book == data[i].book)
	  dist[data[i].user - 1] += pow(user1_data[j].eval - data[i].eval, 2.0);
    }
  }

  for (vector<double>::size_type i = 1; i < max.user; i++){
    score[i] = 1 / (sqrt(dist[i]) + 1);
  }

  for(int i=0;i<user1_data.size();i++)
    user1_read[user1_data[i].book]=1;
  
  
  for (idx i = 0; i < data.size(); i++) {
    if (data[i].user != 1 && used[data[i].user]==1 && user1_read[data[i].book]==0) {//
      recomm[data[i].book - 1] += score[data[i].user - 1] * data[i].eval;
      S[data[i].book - 1] += score[data[i].user - 1];
    }
  }

  for (int i = 0; i < max.book; i++) {
    RES tmp;
    bool insert = true;
    if (user1_read[i+1]==0) {
      tmp.num = i + 1;
      tmp.score = recomm[i] / S[i];
      result.push_back(tmp);
    }
  }

  sort(result.begin(), result.end(), compare);

  for (vector<RES>::size_type i = 0; i < result.size(); i++)
    cout << result[i].num << ' '<< result[i].score << endl;
  
}
	

void read_data(DATA &max, vector<DATA> &data, bool used[100001]) {
  cout << "input the number of users, books, evaluation" << endl;
  cin >> max.user >> max.book >> max.eval;

  cout << "input each data(user number,book,evaluation)" << endl;
  for (int i = 0; i < max.eval; i++) {
    DATA tmp;
    bool u = false, b = false, e = false;

    cin >> tmp.user >> tmp.book >> tmp.eval;
    u = 1 <= tmp.user && tmp.user <= max.user;
    b = 1 <= tmp.book && tmp.book <= max.book;
    e = 0 <= tmp.eval && tmp.eval <= 5;

    if (u && b && e)
      data.push_back(tmp);
    else {
      cout << "input data is not correct" << endl;
      cout << "please input again" << endl;
      i--;
    }
  }
  f_num(used);
}


void f_num(bool used[100001]){
  vector<int > g[100001];
  queue<int > q;
  int m,a,b;
  cin>>m;
  for(int i=0;i<m;i++){
    cin>>a>>b;
    g[a].push_back(b);
    g[b].push_back(a);
  }
  q.push(1);
  used[1]=true;

  while(q.empty()==false){
    int n=q.front();
    q.pop();

    for(int i=0;i<g[n].size();i++)
      if(used[g[n][i]]==false){
	q.push(g[n][i]);
	used[g[n][i]]=true;
      } 
  }

}
 

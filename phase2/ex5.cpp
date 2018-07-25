#include<iostream>
#include <vector>

using namespace std;
int main(){
  int n,r,q,cnt=0;
  int a[100001]={};
  cin >> n >> r;
  vector<pair<int, int> > s(r);

  for(int i=0;i<r;i++){
    cin>>s[i].first>>s[i].second;
    if(s[i].first>s[i].second)swap(s[i].first,s[i].second);
  }
  sort(s.begin(), s.end());


  for(int i=0;i<r;i++){
    if(a[s[i].first]==0){
      cnt++;
      a[s[i].first]=cnt;
      a[s[i].second]=cnt;
    }
    else 
      a[s[i].second]=a[s[i].first];
  }
  cin>>q;
  for(int i=0;i<q;i++){
    cin>>n>>r;
    if(a[n]==a[r])
      cout << "yes"<<endl;
    else cout<<"no"<<endl;
  }
  return 0;
}
    
      



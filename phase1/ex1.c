#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct{
  double point;
  int number;
}Score;

void Sort_Score(Score*,int);

int main(){
  int N,M,i,j,k;
  double **e,*dist;
  Score *score;
  int count = 0;

  printf("input the number of users(2 ~ 100)：");
  scanf("%d",&N);
  printf("input the number of books(1 ~ 100)：");
  scanf("%d",&M);

  e = (double**)malloc(sizeof(double) * N);
  score = (Score*)malloc(sizeof(Score) * (N - 1));
  dist = (double*)malloc(sizeof(double) * (N - 1));
  for(i = 0;i < N;i++)
    e[i] = (double*)malloc(sizeof(double) * M);
  for(i = 0;i < N - 1;i++)
    dist[i] = 0;

  printf("input data\n");
  for(i = 0;i < N;i++)
    for(j = 0;j < M;j++)
      scanf("%lf",&e[i][j]);

  for(i = 1;i < N ;i++){
    for(j = 0;j < M;j++)
      dist[i - 1] += pow(e[0][j] - e[i][j], 2);
    score[i - 1].point = 1 / (sqrt(dist[i - 1]) + 1);
    score[i - 1].number = i + 1;
  }

  Sort_Score(score,N - 1);
  
  for(i = 0;i < N - 1;i++)
    printf("user%d:%f\n",score[i].number,score[i].point);

  free(dist);
  free(score);
  for(i = 0;i < N;i++)
    free(e[i]);

  return 0;
}

void Sort_Score(Score *score,int num){
  int i,j;
  Score tmp;

  for(i = 0;i < num - 1;i++){
    for(j = num - 1;j > i;j--){
      if(score[j].point > score[j - 1].point){
	tmp = score[j];
	score[j] = score[j - 1];
	score[j - 1] = tmp;
      }
    }
  }
}

  

  
  

#include<stdio.h>
#include<stdbool.h>

#define max_resource 50
#define max_process 50

int n;
int m;
int available_resource[max_resource];
int maximum[max_process][max_resource];
int allocated[max_process][max_resource];
int need[max_process][max_resource];


void input(){
  printf("Enter Number of Processes: ");
  scanf("%d",&n);
  printf("Enter the number of each types of resources: ");
  scanf("%d",&m);
  printf("Enter Available matrix:\n");
  for(int i=0;i<m;i++){
    scanf("%d",&available_resource[i]);
  }
  
  printf("Enter Maximum matrix:\n");
  for(int i=0;i<n;i++){
    printf("Process %d: ", i);
    for(int j=0;j<m;j++){
      scanf("%d",&maximum[i][j]);
    }
  }
  
  printf("Enter Allocated matrix:\n");
  for(int i=0;i<n;i++){
    printf("Process %d: ", i);
    for(int j=0;j<m;j++){
        scanf("%d",&allocated[i][j]);
    }
}
  
  for(int i=0;i<n;i++){
    for(int j=0;j<m;j++){
      need[i][j]=maximum[i][j]-allocated[i][j];
      if(need[i][j] < 0){
            printf("Error: Allocated > Maximum for P%d Resource %d\n", i, j);
            exit(1);
      }
    }
  }
  
}


void print(){
  printf("Available: ");
  for(int i=0;i<m;i++){
  printf("R%d: ", i); 
    printf("%d ",available_resource[i]);
  }
  printf("\n");
  
  printf("Maximum Matrix:\n");
  for(int i=0;i<n;i++){
  printf("P%d: ", i); 
    for(int j=0;j<m;j++){
      printf("%d ",maximum[i][j]);
    }
    printf("\n");
  }
  
  printf("Allocation Matrix:\n");
  for (int i=0;i<n;i++){
  printf("P%d: ", i); 
    for(int j=0;j<m;j++){
      printf("%d ",allocated[i][j]);
    }  
    printf("\n");
  }
  
  printf("Need Matrix:\n");
  for(int i=0;i<n;i++){
   printf("P%d: ", i); 
    for(int j=0;j<m;j++){
      printf("%d ",need[i][j]);
    }
    printf("\n");
  }

}

int main(){
input();
print();

return 0;
}

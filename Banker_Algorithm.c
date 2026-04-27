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
  printf("\n");
  printf("Enter the number of each types of resources: ");
  scanf("%d",&m);
  printf("\n");
  printf("Enter Available resources:\n");
  for(int i=0;i<m;i++){
    scanf("%d",&available_resource[i]);
  }
  printf("\n");
  printf("Enter Maximum resources wants:\n");
  for(int i=0;i<n;i++){
    printf("Process %d: ", i);
    for(int j=0;j<m;j++){
      scanf("%d",&maximum[i][j]);
    }
  }
  printf("\n");
  printf("Enter Allocated resources for each process:\n");
  for(int i=0;i<n;i++){
    printf("Process %d: ", i);
    for(int j=0;j<m;j++){
        scanf("%d",&allocated[i][j]);
    }
}
  printf("\n");
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
  printf("\n");
  printf("Available resources: \n");
  for(int i=0;i<m;i++){
  printf("R%d: ", i); 
    printf("%d ",available_resource[i]);
    printf("\n");
  }
  
  printf("\n");
  
  printf("Maximum resources wants:\n");
  for(int i=0;i<n;i++){
  printf("P%d: ", i); 
    for(int j=0;j<m;j++){
      printf("%d ",maximum[i][j]);
    }
    printf("\n");
  }
  printf("\n");
  printf("Allocated resources:\n");
  for (int i=0;i<n;i++){
  printf("P%d: ", i); 
    for(int j=0;j<m;j++){
      printf("%d ",allocated[i][j]);
    }  
    printf("\n");
  }
  printf("\n");
  printf("remaining needed resources:\n");
  for(int i=0;i<n;i++){
   printf("P%d: ", i); 
    for(int j=0;j<m;j++){
      printf("%d ",need[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void log_state(int safe,int sequence[]){
  char buffer[500];
  int fd=open("logs.txt",O_WRONLY | O_APPEND | O_CREAT,0644);
  if(fd==-1){
    printf("Error during opening file!\n");
    return;
  }
	if(safe==1){
    snprintf(buffer,sizeof(buffer),"State: Safe\n Sequence: ");
    write(fd,buffer,strlen(buffer));
    for(int i=0;i<n;i++){
      snprintf(buffer,sizeof(buffer),"P%d",sequence[i]);
      write(fd,buffer,strlen(buffer));
      if(i<n-1){
        snprintf(buffer,sizeof(buffer),"->");
        write(fd,buffer,strlen(buffer));
      }
    }
    snprintf(buffer,sizeof(buffer),"\n");
    write(fd,buffer,strlen(buffer));
  }
  
  close(fd);
}

int safety_algorithm(int safe_sequence[]){
  printf("\n");
  int work[max_resource];
  bool finish[max_process];
  int count=0;
  for(int i=0;i<m;i++){
    work[i]=available_resource[i];
  }
  
  for(int i=0;i<n;i++){
    finish[i]=false;
  }

  while(count<n){
    bool found=false;
    for(int i=0;i<n;i++){
      if(finish[i]==false){
        bool run=true;
        for(int j=0;j<m;j++){
          if(need[i][j]>work[j]){
            run=false;
            break;
          }
        }
        if(run){
          for(int j=0;j<m;j++){
            work[j]+=allocated[i][j];
          }
          finish[i]=true;
          safe_sequence[count]=i;
          found=true;
          count++;
          break;
        }
      }
    }
    if(found==false){
      return 0;
    }
}
	printf("\n");
	return 1;
}

void request_resource(int pid,int request[]){
  printf("P%d requested for more resources\n",pid);
  printf("Need resources of each type:\n");
  for(int i=0;i<m;i++){
    printf("%d ",request[i]);
  }
  printf("\n");
  for(int i=0;i<m;i++){
    if(request[i]>need[pid][i]){
      printf("Error! P%d lied about resources.\n",pid);
      return;
    }
  }
  
  for(int i=0;i<m;i++){
    if(request[i]>available_resource[i]){
      printf("P%d have to wait for resources!\n",pid);
      return;
    }
  }
}

int main(){
input();
print();

return 0;
}

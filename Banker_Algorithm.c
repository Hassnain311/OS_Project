#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>

#define max_resource 50
#define max_process 50

int n;
int m;
int total_request=0;
int grant_request=0;
int deny_request=0;
double t_time=0.0;
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
  }else{
    snprintf(buffer,sizeof(buffer),"State: Unsafe\n");
    write(fd,buffer,strlen(buffer));
  }
  snprintf(buffer,sizeof(buffer),"---\n");
  write(fd,buffer,strlen(buffer));
  
  close(fd);
  print("\n");
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
  
  for(int j=0;j<m;j++){
    available_resource[j]-=request[j];
    allocated[pid][j]+=request[j];
    need[pid][j]-=request[j];
  }
  
  clock_t start=clock();
  int sequence[max_process];
  int result=safety_algorithm(sequence);

  clock_t end=clock();
  t_time+=((double)(end-start))/CLOCKS_PER_SEC;

  if(result==1){
    grant_request++;
    printf("request is fullfilled\n");
    log_state(1,sequence);
    printf("Sequence: ");
    for(int i=0;i<n;i++){
      printf("P%d ",sequence[i]);
      if(i<n-1){
        printf(" -> ");
      }
    }
    printf("\n"); 
  }else{
    deny_request++;
    for(int i=0;i<m;i++){
      available_resource[i]+=request[i];
      allocated[pid][i]-=request[i];
      need[pid][i]+=request[i];
    }
    printf("request denied!\n");
    int empty[max_process];
    log_state(0,empty);
    printf("Not a Safe State\n");
  }
  printf("\n");
}

void performance_report(){
  printf("\n");
  printf("Total requests: %d\n",total_request);
  printf("Granted requests: %d\n",grant_request);
  printf("Denied requests: %d\n",deny_request);
  if(total_request>0){
    printf("total time: %f seconds\n",t_time);
    printf("Average per check: %f seconds\n",t_time/total_request);
  }else{
    printf("No requests made yet\n");
  }
  printf("\n");
}

void random_request(){
  printf("\n");
  int eligible[max_process];
  int count=0;
  for(int i=0;i<n;i++){
    for(int j=0;j<m;j++){
      if(need[i][j]>0){
        eligible[count]=i;
        count++;
        break;
      }
    }
  }
  if(count==0){
    printf("All processes are done!");
    return;
  }
  int pid=eligible[rand()%count];
  int request[max_resource];
  for(int j=0;j<m;j++){
    request[j]=rand()%(need[pid][j]+1);
  }
  
  printf("Random request for p%d: ",pid);
  for(int i=0;i<m;i++){
    printf("%d ",request[i]);
  }
  printf("\n");
  request_resource(pid,request);
  printf("\n");
}

void rollback(int pid,int request[]){
  for(int j=0;j<m;j++){
        if(request[j] > need[pid][j]){
            printf("Error! Request exceeds need for R%d\n", j);
            return;
        }
  }
  for(int j=0;j<m;j++){
      if(request[j] > available_resource[j]){
          printf("Error! Request exceeds available for R%d\n", j);
          return;
      }
  }
  printf("State before request:\n");
  printf("Available resource:\n");
  for(int j=0;j<m;j++){
    printf("R%d ",j);
    printf("%d ",available_resource[j]);
  }
  printf("\n");
  printf("P%d Allocated: ",pid);
  for(int j=0;j<m;j++){
    printf("%d ",allocated[pid][j]);
  }
  printf("\n");
  printf("P%d Need: ",pid);
  for(int j=0;j<m;j++){
    printf("%d ",need[pid][j]);
  }
  printf("\n");
  
  
  int backup_available[max_resource];
  int backup_allocated[max_resource];
  int backup_need[max_resource];
  for(int j=0;j<m;j++){
    backup_available[j]=available_resource[j];
    backup_allocated[j]=allocated[pid][j];
    backup_need[j]=need[pid][j];
  }
  
  for(int j=0;j<m;j++){
    available_resource[j]-=request[j];
    allocated[pid][j]+=request[j];
    need[pid][j]-=request[j];
  }
  
  printf("Available resource:\n");
  for(int j=0;j<m;j++){
    printf("R%d ",j);
    printf("%d ",available_resource[j]);
  }
  printf("\n");
  
  printf("P%d Allocated: ",pid);
  for(int j=0;j<m;j++){
    printf("%d ",allocated[pid][j]);
  }
  printf("\n");
  printf("P%d Need: ",pid);
  for(int j=0;j<m;j++){
    printf("%d ",need[pid][j]);
  }
  printf("\n");
  
  int sequence[max_process];
  if(safety_algorithm(sequence)==1){
    printf("SAFE! Request granted.\n");
    printf("Sequence: ");
    for(int i=0;i<n;i++){
      printf("P%d",sequence[i]);
      if(i<n-1){
        printf(" -> ");
      }
    }
    printf("\n");
        log_state(1,sequence);
  }else{
    printf("Unsafe.Rolling Back resources!\n");
    for(int j=0;j<m;j++){
      available_resource[j]=backup_available[j];
      allocated[pid][j]=backup_allocated[j];
      need[pid][j]=backup_need[j];
    }
    
    printf("\n");
    printf("State After rolling back all the resources!\n");
    printf("Available resource:\n");
    for(int j=0;j<m;j++){
      printf("R%d ",j);
      printf("%d ",available_resource[j]);
    }
    printf("\n");
  
    printf("P%d Allocated: ",pid);
    for(int j=0;j<m;j++){
      printf("%d ",allocated[pid][j]);
    }
    printf("\n");
    printf("P%d Need: ",pid);
    for(int j=0;j<m;j++){
      printf("%d ",need[pid][j]);
    }
    printf("\n");
    printf("State Restored!\n");
    int empty[max_process];
    log_state(0,empty);
  }
  
}

int main(){
srand(time(NULL));
printf("------------------------\n");
printf("|  Banker's Algorithm  |\n");
printf("------------------------\n");
printf("\n");
printf("\n");
input();
print();

int sequence[max_process];
if(safety_algorithm(sequence)==1){
  printf("\n");
  printf("System's initial state is in Safe State.\n");
  printf("Sequence: ");
  for(int i=0;i<n;i++){
    printf("P%d",sequence[i]);
    if(i<n-1){
      printf("->");
    }
  }
  printf("\n");
  log_state(1,sequence);
}else{
  printf("Not safe state!\n");
  int empty[max_process];
  log_state(0,empty);
}
printf("\n");
printf("\n");
int ch;
while(1){
  printf("------------------------\n");
  printf("|  Banker's Algorithm  |\n");
  printf("------------------------\n");
  printf("\n");
  printf("\n");
  printf("1.Request resources:\n");
  printf("2.Print current State:\n");
  printf("3.check Safety:\n");
  printf("4.Generate Random Request for Resources:\n");
  printf("5.Rollback simulation:\n");
  printf("6.Generate Performance Report:\n");
  printf("7.Exit\n");
  printf("\n");
  printf("Enter Choice: ");
  scanf("%d",&ch);
  switch(ch){
    case 1:{
      int pid;
      int request[max_resource];
      printf("Enter Process ID: ");
      scanf("%d",&pid);
      printf("\n");
      printf("Enter requested resources:\n");
      for(int i=0;i<m;i++){
        scanf("%d",&request[i]);
      }
      request_resource(pid,request);
      break;
    }
    case 2:
	  print();
      printf("\n");
    break;
    case 3:{
      int sequence[max_process];
      if(safety_algorithm(sequence)==1){
      printf("\n");
      printf("Current state is in Safe State.\n");
      printf("Sequence: ");
      for(int i=0;i<n;i++){
        printf("P%d",sequence[i]);
        if(i<n-1){
          printf("->");
        }
      }
      printf("\n");
      log_state(1,sequence);
      }else{
        printf("Not safe state!\n");
        int empty[max_process];
        log_state(0,empty);
      } 
      printf("\n");
    break;
    }
    case 4:
   
    case 5:
    case 6:
     
    case 7:
    exit(1);
    default:
      printf("Invalid choice! Try Again\n");
  }
  printf("\n");
  printf("\n");
}

return 0;
}

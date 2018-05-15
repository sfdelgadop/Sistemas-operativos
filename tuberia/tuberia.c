#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<string.h> 

int main () {
	int pipefd [2];
	pid_t pid;
	int r,i;
	r = pipe(pipefd);
	pid = fork();
	if(pid==0){
		long double pi = 0.0;
		close(pipefd[0]);
		for(int i=0;i<1000000000;i++){
			if(i%2==0){
				pi += (4.0/(i*2.0+1.0));
			}else{
				pi -= (4.0/(i*2.0+1.0));
			}
		}
		printf("%Lf\n",pi);
		write(pipefd[1],&pi,sizeof(long double));
		close(pipefd[1]);
	}else{
		long double pi = 0,buff;
		int i=0;
		close(pipefd[1]);
		for(int i=1000000000;i<2000000000;i++){
			if(i%2==0){
				pi +=(4.0/(i*2.0+1.0));
			}else{
				pi -=(4.0/(i*2.0+1.0));
			}
		}
		printf("%Lf b\n" , pi );
		r = read(pipefd[0],&buff,sizeof(long double));
		printf("%Lf c\n" , buff );
		pi += buff;
		printf("%Lf d\n" , pi );
		close(pipefd[0]);
	}
}
//nicequest

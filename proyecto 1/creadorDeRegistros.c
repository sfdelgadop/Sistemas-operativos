#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<string.h> 
#include<time.h> 

struct dogType{
	char name[32];
	char type[32];
	int age;
	char race[20];
	int height;
	float weight;
	char gender[2];			
};


int main () {
	time_t t;
	FILE *fp;
	char nombres[1733][14];
	char tipos[6][10];
	char raza[5][15];
	char sexo[2][2];
	fp = fopen("nombresMascotas.txt","r+");
	int i = 0,random = 0;
	while(!feof(fp)){
		fscanf (fp,"%s", nombres[i]);
		i += 1;
		//printf("%s\n",a[i]);
	}
	
	fp = fopen("tipos.txt","r+");
	i = 0;
	while(!feof(fp)){
		fscanf (fp,"%s", tipos[i]);
		i += 1;
	}

	fp = fopen("raza.txt","r+");
	i = 0;
	while(!feof(fp)){
		fscanf (fp,"%s", raza[i]);
		i += 1;
	}
	fp = fopen("sexo.txt","r+");
	i = 0;
	while(!feof(fp)){
		fscanf (fp,"%s", sexo[i]);
		i += 1;
	}
	srand ((unsigned) time(&t));
	srand48 ((unsigned) time(&t));
	fp = fopen("dataDogs.dat","wb+");
	int n=0;
	for(int j=0;j<1700;j++){
		for(int k=0;k<rand()%120000+7000;k++){
			struct dogType newPet;
			memset(&newPet,0,sizeof(struct dogType));
			strcpy(newPet.name,nombres[j]);
			strcpy(newPet.type,tipos[rand()%6]);
			newPet.age = rand()%40;
			strcpy(newPet.race,raza[rand()%5]);
			newPet.height = rand()%40+1;
			newPet.weight = drand48()*(20.0-0,5)+0.5;
			strcpy(newPet.gender,sexo[rand()%2]);
			fwrite(&newPet,sizeof(struct dogType),1,fp);
			n+=1;
			if(n>=10000000)
				break;
		}
		if(n>=10000000)
			break;
	}
	rewind(fp);
	fp = fopen("dataDogs.dat","rb+");
	rewind(fp);
	struct dogType newPet2;
	fread(&newPet2,sizeof(struct dogType),1,fp);
	printf("%s\n",newPet2.name);
	printf("%s\n",newPet2.type);
	printf("%d\n",newPet2.age);
	printf("%s\n",newPet2.race);
	printf("%d\n",newPet2.height);
	printf("%f\n",newPet2.weight);
	printf("%s\n",newPet2.gender);
}


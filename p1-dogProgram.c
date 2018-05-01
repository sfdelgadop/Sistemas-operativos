#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<string.h> 
#include <ctype.h>

struct dogType{//estructura DogType
	char name[32];
	char type[32];
	int age;
	char race[16];
	int height;
	float weight;
	char gender[2];			
};

int longitud(char *str){//función que decuelve la longitud de una cadena char*
	int j=0;
	while(str[j] != '\0'){j++;}
	return j;
}

char* to_lower(char *str){//devuelbe las cadenas en minusculas
	int i = 0;
	while( str[i] ) {
		str[i]=tolower(str[i]);
		i++;
	}
	return str;
}

unsigned long hash(char *str){//La función de hash que se encarga de asignarle a cada carácter un hash
	to_lower(str);
	unsigned long hash = 0;
	int j=0;
	int min = 0;
	for(int j=0 ; j < longitud(str) ; j++){
		hash = hash*33 + str[j]%33 +j*33 - (long)str[j]*13;
	}
	return hash%1000000;
}

void hashing(int hashList[]){//la funsión que se ancarga de llenar el arreglo con los respectivos hash
	FILE *fp;
	int i = 1;
	fp = fopen("dataDogs.dat","rb+");
	rewind(fp);
	while(!feof(fp)){
		struct dogType pet;
		fread(&pet,sizeof(struct dogType),1,fp);
		if(hashList[hash(pet.name)]==0)
			hashList[hash(pet.name)]=i;//hash por defecto
		i += 1;
	}
	fclose(fp);
}

int lugar(char *str){//función que devuele el lugar en el que se acomoda un nuevo nombre
	FILE *fp;
	fp = fopen("dataDogs.dat","rb+");
	rewind(fp);
	struct dogType newPet;
	int n = 0;
	while( fread(&newPet,sizeof(struct dogType),1,fp)!=0 ){// ciclo que se ejecuta hasta encontrar un nombre mayor al digitado
		n += 1;
//		printf("%s\t%s\n",str,newPet.name);
		if(strcmp(str, newPet.name)<0)
			break;
	}
	fclose(fp);
	return n;
}


void crear_registro(struct dogType pet){//función que cre un nuevo registro clonando el archivo y pegando en la posición correcta el nuevo regitro
	int i=1,position;
	position = lugar(pet.name);
//	printf("%s\n",pet.name);						
	printf("quedo en el posición: %d\n",position);//pide la posición del nuevo registro
	FILE *fp, *temp;
	temp = fopen("temp.dat","wb+");
	fp = fopen("dataDogs.dat","rb+");
	struct dogType newPet;
	while( fread(&newPet,sizeof(struct dogType),1,fp)!=0 && position != i){//copia la primera parte del archivo
		fwrite(&newPet,sizeof(struct dogType),1,temp);
		i += 1;
		if (position == i )
			break;
	}

	fwrite(&pet,sizeof(struct dogType),1,temp);//algrega el nuevo elemento

	while( fread(&newPet,sizeof(struct dogType),1,fp)!=0 )//copia la segunda parte del archivo
		fwrite(&newPet,sizeof(struct dogType),1,temp);
	
	fclose(fp);
	fclose(temp);
	remove("dataDogs.dat");//se elimina el original
	rename("temp.dat","dataDogs.dat");//se renombra el segundario
	
}

void eliminar_registro(int dato_a_eliminar){//se pide el dato a eliminar y se elimina del archivo
	FILE *fp, *temp;
	struct dogType newPet;
	temp = fopen("temp.dat","wb+");
	fp = fopen("dataDogs.dat","rb+");
	int i=1;
	while( fread(&newPet,sizeof(struct dogType),1,fp)!=0 && dato_a_eliminar != i){// se copia la primera parte del archivo sin tener el cuenta el que se desea eliminar
		fwrite(&newPet,sizeof(struct dogType),1,temp);
		i += 1;
	}

	while( fread(&newPet,sizeof(struct dogType),1,fp)!=0 )//se copia la segunda parte del archivo
		fwrite(&newPet,sizeof(struct dogType),1,temp);

	fclose(fp);
	fclose(temp);
	remove("dataDogs.dat");// se elimina el original
	rename("temp.dat","dataDogs.dat");//el temporal toma su lugar
}

void crear_registro_aux(){//en este auxiliar se piden los datos
	FILE *fp;
	char name[32];
	char type[32];
	int age;
	char race[16];
	int height;
	float weight;
	char gender[1];
	fp = fopen("dataDogs.dat","rb");
	struct dogType newPet;
	printf("Bienvenido el menú para ingresar una nueva mascota, a continuación ingrese los datos pedidos\n");
	printf("Ingrese el nombre de la mascota\n");
	scanf("%s",name);
	printf("Ingrese el tipo de la mascota\n");
	scanf("%s",type);
	printf("Ingrese la edad de la mascota\n");
	scanf("%d",&age);
	printf("Ingrese la raza de la mascota\n");
	scanf("%s",race);
	printf("Ingrese la estatura de la mascota\n");
	scanf("%d",&height);
	printf("Ingrese el peso de la mascota\n");
	scanf("%f",&weight);
	printf("Ingrese el género de la mascota\n");
	scanf("%s",gender);
	strcpy(newPet.name,name);//los datos se meten en una estructura
	strcpy(newPet.type,type);
	newPet.age = age;
	strcpy(newPet.race,race);
	newPet.height = height;
	newPet.weight = weight;
	strcpy(newPet.gender,gender);
	crear_registro(newPet);//se llama la funció para crear el registro
	fclose(fp);
}

void buscar_registro(){//función para buscar el registro
	FILE *fp;
	int dato_a_buscar,numero_de_registros,eleccion;
	fp = fopen("dataDogs.dat","rb+");
	fseek(fp,0,SEEK_END);//se envía el puntero al final del archivo
	numero_de_registros = ftell(fp)/sizeof(struct dogType);//se obtiene la cantidad de elementos del archivo
	printf("El numero de registros es:%d\n",numero_de_registros);
	printf("Ingrese el registro que desea buscar\n");
	scanf("%i",&dato_a_buscar);
	while(dato_a_buscar > numero_de_registros || dato_a_buscar < 0){//se verifica la correctitud del dato insertado
		printf("Registro Incorrecto\n");
		printf("por favor ingrese el registro que desea buscar\n");
		scanf("%i",&dato_a_buscar);		
	}
	struct dogType newPet;
	fseek(fp,(dato_a_buscar-1)*sizeof(struct dogType),SEEK_SET);//se acomoda el puntero en la posicion deseada
	fread(&newPet,sizeof(struct dogType),1,fp);//se sube a una estructura
	fclose(fp);
	fp = fopen("probe.txt","w+");//todo se sube a la pantalla de vizualisación
	fprintf(fp,"Nombre: %s\n",newPet.name);
	fprintf(fp,"Tipo: %s\n",newPet.type);
	fprintf(fp,"Edad: %i\n",newPet.age);
	fprintf(fp,"Raza: %s\n",newPet.race);
	fprintf(fp,"Tamaño: %i\n",newPet.height);
	fprintf(fp,"Peso: %f\n",newPet.weight);
	fprintf(fp,"Genero: %s\n",newPet.gender);	
	fclose(fp);
	system("gedit probe.txt");
	printf("¿Desea guardar los cambios realizados en el archivo?\n");
	printf("1 si, 0 no\n");
	scanf("%i",&eleccion);
	if(eleccion==1){//se recuperan los datos modificados
		struct dogType pet;
		char name[32];
		char type[32];
		char trh[32];//sirve para guardar la basura nombre: type:... etc
		int age;
		char race[16];
		int height;
		float weight;
		char gender[1];
		fp = fopen("probe.txt","r+");
		rewind(fp);
		fscanf(fp,"%s %s",trh,name);
		fscanf(fp,"%s %s",trh,type);
		fscanf(fp,"%s %s",trh,&age);
		fscanf(fp,"%s %s",trh,race);
		fscanf(fp,"%s %d",trh,&height);
		fscanf(fp,"%s %f",trh,&weight);
		fscanf(fp,"%s %s",trh,gender);
		strcpy(pet.name,name);
		strcpy(pet.type,type);
		pet.age = age;
		strcpy(pet.race,race);
		pet.height = height;
		pet.weight = weight;
		strcpy(pet.gender,gender);
//		fwrite(&pet,sizeof(struct dogType),1,fp);
		fclose(fp);
		eliminar_registro(dato_a_buscar);//se elimina el registro antiguo
		crear_registro(pet);//se agrega el registro nuevo
	}
	system("killall gedit");//se cierra el archivo de vizualisación para que no pueda ser modificado más
	remove("probe.txt");
}

void eliminar_registro_aux(){//pide el registro a ser elimimado
	FILE *fp;
	int dato_a_eliminar,numero_de_registros;
	fp = fopen("dataDogs.dat","rb+");
	fseek(fp,0,SEEK_END);
	numero_de_registros = ftell(fp)/sizeof(struct dogType);//se obtiene el tamoño del archivo
	fclose(fp);
	printf("El numero de registros es:%d\n",numero_de_registros);
	printf("Ingrese el registro que desea eliminar\n");
	scanf("%i",&dato_a_eliminar);
	while(dato_a_eliminar > numero_de_registros || dato_a_eliminar < 0){//se verifica la correctitud del dato insertado
		printf("Registro Incorrecto\n");
		printf("por favor ingrese el registro que desea buscar\n");
		scanf("%i",&dato_a_eliminar);		
	}

	eliminar_registro(dato_a_eliminar);//se llama a la función para que sea eliminado
}


void buscar(int hashList[]){//función para buscar en el archivo
	FILE *fp;
	int aux=0,i,j=0;
	fp = fopen("dataDogs.dat","rb+");
	char name[32];
	printf("Ingrese el nombre que desea buscar\n");//se ingresa el nombre que se desea buscar
	scanf("%s",name);
	i = hashList[hash(name)];//se llama a hash
	if(hashList[hash(name)]==0){
		printf("No existe  el registro solicitado\n");
		return;
	}
	fseek(fp,i*sizeof(struct dogType),SEEK_SET);//se va a la posicion solicitada
	struct dogType pet;
	fread(&pet,sizeof(struct dogType),1,fp);
	printf("%i\n",i);
	while(aux==0){//se imprime toda las lista con el mismo nombre
		struct dogType pet2;
		fread(&pet2,sizeof(struct dogType),1,fp);
		if(strcmp(name,pet2.name) == 0){
			i += 1;
			printf("%i\n",i);
		}else{
			aux = 1;
		}
	}
	fclose(fp);
}

int main () {
	int hashList[1000000];
	for(int j = 0;j<1000000;j++)
		hashList[j]=0;
	hashing(hashList);
	typedef enum {true,false} bool;
	bool bucle = true;
	while (bucle == true){
		printf("Programa de Administración de Mascotas.\n");//menú principal
		printf("Ingrese 1 para agregar un nuevo registro\n");
		printf("2 para consultar registro\n");
		printf("3 para borrar registro\n");
		printf("4 para buscar un registro\n");
		printf("5 para salir\n");
		int optionInMenuAux = 0;
		int option = 0;
		bool optionInMenu = true;
		scanf("%d",&option);//se escanean las opciones
		switch(option){
			case 1:
				while (optionInMenu == true){
					crear_registro_aux();//se llama a la función de crear registro				
					for(int j = 0;j<1000000;j++)//se vuelve a ejecutar la funcion de hash para agregar el nuevo elemento
						hashList[j]=0;
					
					hashing(hashList);
					printf("Ingrese 1 si desea volver al menú principal\n");
					printf("Ingrese otro numero si desea ingresar otra mascota\n");
					scanf("%d",&optionInMenuAux);
					if(optionInMenuAux==1){
						optionInMenu = false;
					}
				}
				break;
			case 2:
				while (optionInMenu == true){
					buscar_registro();//se llama a la función de buscar registro
					printf("Ingrese 1 si desea volver al menú principal\n");
					printf("Ingrese otro numero si desea ver otro registro\n");
					scanf("%d",&optionInMenuAux);//se pregunta si desea salir
					if(optionInMenuAux==1){
						optionInMenu = false;
					}
				}
				break;
			case 3:
				while (optionInMenu == true){
					eliminar_registro_aux();//se llama a la función de eliminar
					for(int j = 0;j<1000000;j++)// al eliminar un dato se actualiza el hash
						hashList[j]= 0;
					hashing(hashList);
					printf("Ingrese 1 si desea volver al menú principal\n");
					printf("Ingrese otro numero si desea ver otro registro\n");
					scanf("%d",&optionInMenuAux);
					if(optionInMenuAux==1){
						optionInMenu = false;
					}
				}
			
				break;
			case 4:
				while (optionInMenu == true){
					buscar(hashList);//se llama a la función de buscar por nombre
					printf("Ingrese 1 si desea volver al menú principal\n");
					printf("Ingrese otro numero si desea ver otro registro\n");
					scanf("%d",&optionInMenuAux);
					if(optionInMenuAux==1){
						optionInMenu = false;
					}
				}
				break;
			case 5:
				bucle = false;// simplemente lo rompe
				break;
			default:
				printf ("valor incorrecto");// valor por defecto
				break;
		}
	}
}

#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <openssl/sha.h>
using namespace std;
/*
FILE *fp;
char strd[500000];//hashed data
};*/
//void insertdatatotracker(string hash,struct my_data details);
//to create XML data

string sizeoffile(int n){
	static const char *SIZ[]={"B","kB","MB","GB"};
	int i=0,res=0;
	string t;
	while(n>=1024 && i<4){
		res=n%1024;
		n/=1024;
		i++;
	}
	t=n;
	//strcat(t,SIZ[i]);
	return t;
}
//client program


int hash_it()//char* cl_URL1,char* tr_URL1,char* tr_URL2 ){
	{
	char* cl_URL1;
	char* tr_URL1;
	char* tr_URL2;
	int i = 0;
	char str[1024*512];
	unsigned char temp[40];
	char buf[40];
	char cc[100000];
	char *cl_IP,*cl_port,*IP1,*port1,*IP2,*port2;
	string fnm;
	int ctr=0;
	FILE *fp;
//	fp3 = fopen("tidata.xml","w");
	IP1=strtok(tr_URL1,":");
	port1=strtok(NULL,":");
	//cout<<"ip-"<<IP1;
	//cout<<"port1"<<port1;
	IP2=strtok(tr_URL2,":");
	port2=strtok(NULL,":");

	cl_IP=strtok(cl_URL1,":");
	cl_port=strtok(NULL,":");

	printf("Enter the filename to open for reading \n"); 
	//scanf("%s", filename); 
	char filename[20] 	;
	char fname[20] 	;
	strcpy(filename,"Rsync");
	 
	 strcpy(fname,filename);
   	fp = fopen(filename, "r"); 
   	if (fp == NULL) 
    { 
       	printf("Cannot open file %s \n", filename); 
       	exit(0); 
    }  	


	fseek(fp, 0, 2);    /* file pointer at the end of file */
	int filekasize = ftell(fp); 
	fclose(fp);
	// d.f_sz=filekasize;
	// d.ip="127.0.1.1";
	// d.port=8080;
	// d.loc=filename;

   
    fp = fopen(filename, "r"); 
    // torrname=strtok(filename,".");

    // strcat(torrname,".mtorrent");
	
    // fp2 = fopen(torrname, "aw");

    //enter data in mtorrent file
    // fprintf(fp2, "%s\n%s",IP1,port1 );/*Tracker URL 1*/
    // fprintf(fp2, "\n%s\n%s",IP2,port2 );/*Tracker URL 2*/
    // fprintf(fp2, "\n%s", fname);  /* filename*/
    // fprintf(fp2, "\n%d\n", filekasize);



    while (!feof(fp))
    { 
    	int k = fread(str,1,1024*512,fp); 	
		memset(buf, 0x0, SHA_DIGEST_LENGTH*2);
		memset(temp, 0x0, SHA_DIGEST_LENGTH);

		SHA1((unsigned char *)str, k, temp);
	
		
		strcat(cc,buf);
		
	} 
	//close read file
  	fclose(fp); 

  	//close .mtorrent file
  	// fclose(fp2);
  //	
  // 	memset(temp, 0x0, SHA_DIGEST_LENGTH);
  // 	memset(buf, 0x0, SHA_DIGEST_LENGTH*2);

  // 	SHA1((unsigned char *)cc, strlen(cc), temp);
  
  // 	for (i=0; i < SHA_DIGEST_LENGTH; i++){ 
		// 	printf("%02x", temp[i]);
		// 	sprintf((char*)&(buf[i*2]), "%02x", temp[i]);
		// }
 	// printf("%s",temp);
  // 	printf("SHA1 %s\n", cc);
  // 	// strcpy(d.strd,cc);
   // insertdatatotracker(cc,d);
	
	printf("\ncc is %s",cc);
//	myformat(fp3,d);
	
	return 0;
}
//int main(int num_args, char** arg_strings);
int main(int argc, char *argv[]) 
{ 
  
   int c;
   c=hash_it();//argv[1],argv[2],argv[3]);

    printf("\n");     
    return 0; 
}

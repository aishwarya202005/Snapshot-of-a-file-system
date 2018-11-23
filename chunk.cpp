#include<bits/stdc++.h>
#include <openssl/md5.h>
using namespace std;
#define BLOCK_SIZE 512

// sending from A and B
// Divide snapshot into fixed size chunks

vector<string> roll_check; //weak checksum
vector<string> md_5; //strong checksum

string calculate_md5(char input_buffer[],size_t bytesread)
{
	unsigned char digest[MD5_DIGEST_LENGTH];
	MD5((unsigned char*)input_buffer, bytesread, (unsigned char*)&digest);    
    char mdString[33];
    for(int i = 0; i < 16; i++)
         sprintf((char *)&mdString[i*2], "%02x", (unsigned int)digest[i]);

    //printf("md5 digest: %s\n", mdString);
    return mdString;
}


void get_chunk_id(string file_path) //for b
{
	FILE *iptr;	 //fstream---iostream
	size_t bytesread=0;
	int count=0;
	
	char input_buffer[BLOCK_SIZE];	   
	iptr=fopen(file_path.c_str(),"rb");
	if(iptr==NULL)
		printf("Can not open input file \n");
	else
	{	
		memset(input_buffer,0x0,sizeof(input_buffer));	        
    	while((bytesread = fread(input_buffer, 1, sizeof(input_buffer), iptr)) > 0) //3rd parameter represent count (how many times to read) 
        {  
        	cout<<bytesread<<"\n"<<input_buffer; 
        	string md5_hash = calculate_md5(input_buffer,bytesread);
        	md_5.push_back(md5_hash);		//md5 hash of each chunk is stored in this vector
        	//cout<<"md5_hash returned is :"<<" "<<md5_hash<<endl;
        	memset(input_buffer,0x0,sizeof(input_buffer));
        	count++;
        	cout<<"count is "<<count<<endl;
        	cout<<"\n------------------------------------------------------------------\n";
        	//cout<<"------------------------------------------------------------------";
        } 
        fclose(iptr);
	}
}
// Divide original file into chunks
void get_successive_chunks(string file_path) // for A
{
	FILE *iptr;	 //fstream---iostream
	size_t bytesread=0;
	int count=0;
	vector<vector<string>> roll_check; //weak checksum
	vector<vector<string>> md_5; //strong checksum
	char input_buffer[BLOCK_SIZE];	   
	iptr=fopen(file_path.c_str(),"rb");
	if(iptr==NULL)
		printf("Can not open input file \n");
	else
	{		        
    	while((bytesread = fread(input_buffer, 1, sizeof(input_buffer), iptr)) > 0)
        { 
        	//cout<<bytesread<<endl; 
        	cout<<input_buffer<<endl; 
        	memset(input_buffer,0x0,sizeof(input_buffer));
        	fseek ( iptr , -(bytesread-1) , SEEK_CUR);
        	count++;
        	cout<<"\ncount is "<<count;
        	cout<<"\n------------------------------------------------------------------\n";
        } 
        fclose(iptr);
	}
}

// to find differences between file1,file2 using weak and strong
string compare_files() 
{

}
int main()
{
	get_chunk_id("a.txt");
	//get_successive_chunks("a.txt");
	cout<<"vector is :"<<endl;
	for(auto it = md_5.begin(); it!=md_5.end();it++)
	{
		cout<<*it<<endl;
	}
}


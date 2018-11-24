#include<bits/stdc++.h>
#include <openssl/md5.h>
using namespace std;
#define BLOCK_SIZE 512
#define M 65536

// sending from A and B
// Divide snapshot into fixed size chunks

vector<string> roll_check; //weak checksum
vector<string> md_5; //strong checksum

vector<string> roll_check_file; //weak checksum
vector<string> md_5_file; //strong checksum

vector<long long> calculate_rolling(char input_buffer[],size_t bytesread)
{
	int k=0,l=bytesread-1;
	long long a=0,b=0,s=0;
	vector<long long> roll_params;
	for(int i=k;i<=l;i++)
	{
		a+=input_buffer[i]%M;
	}
	a=a%M;
	for(int i=k;i<=l;i++)
	{
		b+=((l-i+1)*input_buffer[i])%M;
	}
	b=b%M;
	s=a+M*b;
	
	roll_params.push_back(s);
	roll_params.push_back(a);
	roll_params.push_back(b);
	return roll_params;
	
}

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
	vector<long long> roll_params;
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
        	
        	//string roll_hash=calculate_rolling(input_buffer,bytesread);
        	roll_params=calculate_rolling(input_buffer,bytesread);


        	ostringstream str1;
        	str1<<roll_params[0];
        	string roll_hash=str1.str();
        	cout<<"\nMD5 of chunk is :"<< " "<<md5_hash<<endl;
        	cout<<"Rolling hash of chunk is :"<<" "<<roll_hash<<endl;

        	roll_check.push_back(roll_hash);

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
	vector<long long> roll_params;
	char input_buffer[BLOCK_SIZE];	   
	iptr=fopen(file_path.c_str(),"rb");
	if(iptr==NULL)
		printf("Can not open input file \n");
	else
	{	
		memset(input_buffer,0x0,sizeof(input_buffer));	
		int flag=0; 
		string roll_hash;      
		char prev,curr;
		while((bytesread = fread(input_buffer, 1, sizeof(input_buffer), iptr)) > 0)
		{ 
        	cout<<bytesread<<"\n"<<input_buffer; 

			string md5_hash = calculate_md5(input_buffer,bytesread);
        	md_5_file.push_back(md5_hash);		//md5 hash of each chunk is stored in this vector
        	cout<<"\nMD5 of chunk is :"<< " "<<md5_hash<<endl;

			if(flag==0)
			{
				roll_params=calculate_rolling(input_buffer,bytesread);


				ostringstream str1;
				str1<<roll_params[0];
				string roll_hash=str1.str();
				cout<<"Rolling hash of chunk is :"<<" "<<roll_hash<<endl;

				roll_check_file.push_back(roll_hash);

				flag=1;
				prev=input_buffer[0];
			}
			else
			{
				long long a,b,s;
				a=roll_params[1];
				b=roll_params[2];

				int k=0,l=bytesread-1;

				curr=input_buffer[bytesread-1];

				a=(a%M-prev%M+curr%M)%M;
				b=(b%M -((l-k+1)*prev)%M+a%M)%M;

				s=a+M*b;

	
				ostringstream str1;
				str1<<s;
				string roll_hash=str1.str();
				cout<<"Rolling hash of chunk is :"<<" "<<roll_hash<<endl;

				roll_check_file.push_back(roll_hash);

				prev=input_buffer[0];
			}
			
        	

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
	// cout<<"SNAP is :"<<endl;
	// for(auto it = roll_check.begin(); it!=roll_check.end();it++)
	// {
	// 	cout<<*it<<endl;
	// }

	get_successive_chunks("a.txt");
	// cout<<"File is :"<<endl;
	// for(auto it = roll_check_file.begin(); it!=roll_check_file.end();it++)
	// {
	// 	cout<<*it<<endl;
	// }
}


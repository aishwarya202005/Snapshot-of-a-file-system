#include<bits/stdc++.h>
#include <openssl/md5.h>
using namespace std;
#define BLOCK_SIZE 2
#define M 65536

// sending from A and B
// Divide snapshot into fixed size chunks

vector<string> roll_check_snap; //weak checksum
vector<string> md_5_snap; //strong checksum
vector<string> roll_check_file; //weak checksum
vector<string> md_5_file; //strong checksum
vector<string> changes;
vector<int> vec_same_index; 
FILE *iptr;


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

//calculate_md5 for B file
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
        	md_5_snap.push_back(md5_hash);		//md5 hash of each chunk is stored in this vector
        	
        	//string roll_hash=calculate_rolling(input_buffer,bytesread);
        	roll_params=calculate_rolling(input_buffer,bytesread);


        	ostringstream str1;
        	str1<<roll_params[0];
        	string roll_hash=str1.str();
        	cout<<"\nMD5 of chunk is :"<< " "<<md5_hash<<endl;
        	cout<<"Rolling hash of chunk is :"<<" "<<roll_hash<<endl;

        	roll_check_snap.push_back(roll_hash);

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
			if(bytesread!=BLOCK_SIZE)
				break;
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
				if(a<0)
					a = a+M;
				b=(b%M -((l-k+1)*prev)%M+a%M)%M;
				if(b<0)
					b = b+M;

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
void compare_files(string file_path) 
{
	int flag,j;	
	for(int i=0;i<roll_check_file.size();i++)
	{
		flag=0;
		for( j=0;j<roll_check_snap.size();j++)
		{
			if(roll_check_file[i]==roll_check_snap[j])
			{				
				if(md_5_file[i]==md_5_snap[j])
				{
					flag=1;
					changes.push_back("@"); //take character which is very rare in file
					i+=BLOCK_SIZE-1;
					vec_same_index.push_back(j);
					break;
				}
				
			}
		}
		if(flag==0)
		{
			// char t=(char)i;
			cout<<i;
			changes.push_back(to_string(i));
		}		
	}	
	cout<<"\nmewwwwchanges vector is :"<<endl;
	for(auto it = changes.begin(); it!=changes.end();it++)
	{
		//cout<<*it<<endl;
		printf("%c====\n",(*it)[0] );
	}
	char from_file[5];
	memset(from_file,0x0,sizeof(from_file));
	iptr=fopen(file_path.c_str(),"rb");
	for(int i=0;i<changes.size();i++)
	{
		if(changes[i]!="@")
		{
			fseek ( iptr , changes[i][0]-'0' , SEEK_SET);
			fread(from_file, 1, 1, iptr);
			changes[i][0]=from_file[0];
			memset(from_file,0x0,sizeof(from_file));
		}

	}
	fclose(iptr);
}
// void restore(string file_path)
// {
// 	string file_data;
// 	char input_buffer[BLOCK_SIZE];
// 	int c=-1;
// 	iptr=fopen(file_path.c_str(),"rb");
// 	for(int i=0;i<changes.size();i++)
// 	{
// 		if(changes[i]!='@')
// 			file_data+=changes[i];
// 		else
// 		{
// 			memset(input_buffer,0x0,sizeof(input_buffer));
// 			c++;
// 			int f_index = vec_same_index[c];			
// 			fseek(iptr,f_index,SEEK_SET);
// 			fread(input_buffer, 1, sizeof(input_buffer), iptr);
// 			file_data+=(string)input_buffer;  //append char* to string
// 		}
// 	}
// 	fclose(iptr);
// 	iptr=fopen(file_path.c_str(),"w");
// 	fwrite((char*)file_data.c_str(),1,file_data.size(),iptr);
// 	fclose(iptr);
	
// }
int main()
{
	// get_chunk_id("a.txt");
	//get_successive_chunks("a.txt");
	roll_check_file.push_back("ab");
	roll_check_file.push_back("rt");
	roll_check_file.push_back("xy");
	roll_check_file.push_back("54");
	roll_check_file.push_back("dd");
	roll_check_file.push_back("pq");

	
	roll_check_snap.push_back("ab");
	roll_check_snap.push_back("xy");
	roll_check_snap.push_back("54");

	md_5_file.push_back("3");
	md_5_file.push_back("5");
	md_5_file.push_back("7");
	md_5_file.push_back("9");
	md_5_file.push_back("11");

	md_5_snap.push_back("3");
	md_5_snap.push_back("7");
	md_5_snap.push_back("9");

	compare_files("a.txt");
	// restore()
	cout<<"changes vector is :"<<endl;
	for(auto it = changes.begin(); it!=changes.end();it++)
	{
		cout<<*it<<endl;
	}
	cout<<"vec_same_index vector is :"<<endl;
	for(auto it = vec_same_index.begin(); it!=vec_same_index.end();it++)
	{
		cout<<*it<<endl;
	}
}


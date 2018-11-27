#include<bits/stdc++.h>
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <iostream>
#include <termios.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <errno.h>

using namespace std;
#define M 65536
#define BLOCK_SIZE 512

// sending from A and B
// Divide snapshot into fixed size chunks

vector<string> roll_check_snap; //weak checksum
vector<string> md_5_snap; //strong checksum
vector<string> roll_check_file; //weak checksum
vector<string> md_5_file; //strong checksum
vector<char> changes;
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
    	while((bytesread = fread(input_buffer, 1, sizeof(input_buffer), iptr)) > 0) //3rd parameter represents count (how many times to read) 
    	{  
    		// cout<<bytesread<<endl;
    		// cout<<input_buffer<<endl; 
    		string md5_hash = calculate_md5(input_buffer,bytesread);
        	md_5_snap.push_back(md5_hash);		//md5 hash of each chunk is stored in this vector
        	
        	//string roll_hash=calculate_rolling(input_buffer,bytesread);
        	roll_params=calculate_rolling(input_buffer,bytesread);


        	ostringstream str1;
        	str1<<roll_params[0];
        	string roll_hash=str1.str();
        	//cout<<"\nMD5 of chunk is :"<< " "<<md5_hash<<endl;
        	//cout<<"Rolling hash of chunk is :"<<" "<<roll_hash<<endl;

        	roll_check_snap.push_back(roll_hash);

        	memset(input_buffer,0x0,sizeof(input_buffer));
        	count++;        	
        } 
        fclose(iptr);
        cout<<"\n------------------------------------------------------------------\n";
        cout<<"count for B chunks "<<count<<endl;
        
        cout<<"------------------------------------------------------------------";
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
	int chunks_count=0;
	if(iptr==NULL)
		printf("Can not open input file \n");
	else
	{	
		memset(input_buffer,0x0,sizeof(input_buffer));	
		int flag=0; 
		string roll_hash; 
		long long a,b,s;

		char prev,curr;
		while((bytesread = fread(input_buffer, 1, sizeof(input_buffer), iptr)) > 0)
		{ 
			chunks_count++;
			
        	//cout<<bytesread<<" "<<input_buffer<<endl; 

			string md5_hash = calculate_md5(input_buffer,bytesread);
        	md_5_file.push_back(md5_hash);		//md5 hash of each chunk is stored in this vector
        	//cout<<"\nMD5 of chunk is :"<< " "<<md5_hash<<endl;

        	if(flag==0)
        	{
        		roll_params=calculate_rolling(input_buffer,bytesread);
        		a=roll_params[1];
        		b=roll_params[2];
        		ostringstream str1;
        		str1<<roll_params[0];
        		string roll_hash=str1.str();
        		roll_check_file.push_back(roll_hash);

        		flag=1;
        		prev=input_buffer[0];
        	}
        	else
        	{
        		string roll_hash;
        		if(bytesread<BLOCK_SIZE)
        		{
        			roll_params=calculate_rolling(input_buffer,bytesread);
        			ostringstream str1;
        			str1<<roll_params[0];
        			roll_hash=str1.str();
        		}
        		else
        		{
        			int k=0,l=bytesread-1;
        			curr=input_buffer[bytesread-1];
        			a=(a%M-prev%M+curr%M)%M;
        			if(a<0)
        				a = a+M;
        			b=(b%M -((l-k+1)*prev)%M+a%M)%M;
        			if(b<0)
        				b = b+M;
        			s=a+M*b;
        			prev=input_buffer[0];   

        			ostringstream str1;
        			str1<<s; 

        			roll_hash=str1.str();    			
        		}
        		
        		roll_check_file.push_back(roll_hash);
        	}
        	memset(input_buffer,0x0,sizeof(input_buffer));
        	fseek ( iptr , -(bytesread-1) , SEEK_CUR);
        	count++;			
        } 
        fclose(iptr);
        cout<<"\n-------------------------------------------------------------------------------------------------\n";
        cout<<"chunks for A is "<<chunks_count<<endl;
    }
}

// to find differences between file1,file2 using weak and strong
void compare_files(string file_path) 
{
	int flag,j;	
	char from_file[5];
	memset(from_file,0x0,sizeof(from_file));
	iptr=fopen(file_path.c_str(),"rb");
	if(iptr==NULL)
	{
		cout<<"File can not be opened";
		return;
	}
	for(int i=0;i<roll_check_file.size();i++)
	{
		flag=0;
		cout<<"value of i is "<<i<<endl;
		for( j=0;j<roll_check_snap.size();j++)
		{
			cout<<"in j\n";
			if(roll_check_file[i]==roll_check_snap[j])
			{				
				if(md_5_file[i]==md_5_snap[j])
				{
					flag=1;
					cout<<"matched......"<<endl;
					changes.push_back('~'); //take character which is very rare in file
					i+=BLOCK_SIZE-1;
					vec_same_index.push_back(j);
					cout<<"New value of i is-------- "<<i<<endl;
					break;
				}
				
			}
		}
		if(flag==0)
		{
			// char t=(char)i;
			//cout<<"i value\n";
			//cout<<i<<" ";
			fseek ( iptr , i , SEEK_SET);
			fread(from_file, 1, 1, iptr);				
			changes.push_back(from_file[0]);
			memset(from_file,0x0,sizeof(from_file));
		}		
	}	
	// cout<<"\nchanges vector is :"<<endl;
	// for(auto it = changes.begin(); it!=changes.end();it++)
	// {
	// 	//cout<<*it<<endl;
	// 	printf("%c==\n",(*it) );
	// }
	
	
	//iptr=fopen(file_path.c_str(),"rb");
	// for(int i=0;i<changes.size();i++)
	// {
	// 	if(changes[i]!="~")
	// 	{
	// 		fseek ( iptr , changes[i][0]-'0' , SEEK_SET);
	// 		fread(from_file, 1, 1, iptr);
	// 		changes[i][0]=from_file[0];
	// 		memset(from_file,0x0,sizeof(from_file));
	// 	}

	// }
	fclose(iptr);
}
void take_backup(string file_path)
{
	string file_data="";
	char input_buffer[BLOCK_SIZE];
	int c=-1,flg=0;
	

	if(changes.empty())
	{
		
		iptr=fopen(file_path.c_str(),"wb");
		fclose(iptr);
		cout<<"LLLLLLLLLLL";
		return;	
	}

	for(int i=0;i<changes.size();i++)
	{
		if(changes[i]!='~')
		{
			flg=1;
			break;
		}
	}

	if(flg==0)
		return;



	iptr=fopen(file_path.c_str(),"rb");
	for(int i=0;i<changes.size();i++)
	{
		if(changes[i]!='~')
		{
			//cout<<"\nCHANgS of i>>>";
			file_data+=changes[i];
			//cout<<changes[i]<<endl;
		}
		else
		{
			memset(input_buffer,0x0,sizeof(input_buffer));
			c++;
			int f_index = vec_same_index[c];			
			fseek(iptr,f_index*BLOCK_SIZE,SEEK_SET);
			int l=fread(input_buffer, 1, sizeof(input_buffer), iptr);
			
			for(int i=0;i<l;i++)
			{
				file_data+=input_buffer[i];
			}
			//string str1=(string) input_buffer;
			//str1 = str1.substr(0, str1.size()-1);
			//file_data+=str1;  //append char* to string
			// cout<<"\ninput_buffer->>"<<input_buffer<<endl;

			cout<<"<>file-data---------------------->>"<<file_data;
			cout<<"<>end file-data---------------------->>\n";
		}
	}
	fclose(iptr);
	iptr=fopen(file_path.c_str(),"w");
	//cout<<"\n--0-0-"<<file_data<<endl;
	//cout<<"--0-0-";
	fwrite((char*)file_data.c_str(),1,file_data.size(),iptr);
	fclose(iptr);
	
}

void copy_file(string sourceDir,string destinationDir)
{
	int fdDestination = open(destinationDir.c_str(), O_RDWR|O_CREAT, S_IRUSR | S_IWUSR);
	
  	get_chunk_id(destinationDir);
	get_successive_chunks(sourceDir);
	compare_files(sourceDir);
	take_backup(destinationDir);

	close(fdDestination);
}


void copy_dir(string full_dir,string destination_dir)
{
	DIR* thisDir;
	DIR* thisDir_dest;
	struct dirent* thisFile;
	char buf[512];
	struct stat st;
	string file_path;
	thisDir = opendir( full_dir.c_str() );
	thisDir_dest = opendir(destination_dir.c_str());
    //cout<<"shafiya2"<<endl;

	int flag_close=0;
	if (thisDir_dest)
	{	
		flag_close=1;
	}
	else if(ENOENT==errno)
	{
		mkdir(destination_dir.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);	
	}
	else
	{
		perror("Error ");
	}

	while((thisFile = readdir(thisDir))!= NULL)
	{
		cout<<"Inside Source Directory\n";
    	if((thisFile->d_name[0] == '.'))// || (thisFile->d_name == ".."))
    	{	
    		continue;
    	}
    	//cout<<"shafiya1"<<endl;
    	sprintf(buf, "%s", thisFile->d_name);
    	lstat((full_dir + "/" + thisFile->d_name).c_str(),&st);		//set file properties of the children of full_dir
        if (st.st_mode&S_IFDIR)  	//check file properties for a folder
        {	  //Is a folder
        	//mkdir((full_dir + "/" + thisFile->d_name).c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        	cout<<"Inside Inside Source Directory\n";
    	
        	copy_dir(full_dir + "/" + thisFile->d_name,destination_dir + "/" + thisFile->d_name);
        	

        }
        else if(st.st_mode&S_IFREG)		//check file properties for a file
        {	
        	cout<<"Inside Inside Source file\n";
        	copy_file(full_dir + "/" + thisFile->d_name,destination_dir + "/" + thisFile->d_name);

        		//full_dir = full_dir + "/" + thisFile->d_name;
        		//cout<<"full_dir is :"<<full_dir<<endl;		
        }
    }
    //rmdir(full_dir.c_str()); 
    if(flag_close)
    	closedir(thisDir_dest);

    closedir( thisDir );
}
int main()
{
	// get_chunk_id("b_snap.txt");
	// cout<<"\nsnap roll\n";
	// for(auto it = roll_check_snap.begin(); it!=roll_check_snap.end();it++)
	// {
	// 	cout<<*it<<endl;
	// }
	// cout<<"\nsnap md5\n";
	// for(auto it = md_5_snap.begin(); it!=md_5_snap.end();it++)
	// {
	// 	cout<<*it<<endl;
	// }
	// get_successive_chunks("a.txt");
	// cout<<"\nfile roll\n";
	// // cout<<roll_check_file[0]<<","<<roll_check_file[2]<<","<<roll_check_file[4]<<endl;
	// // cout<<md_5_file[0]<<","<<md_5_file[2]<<","<<md_5_file[4]<<endl;
	
	// cout<<roll_check_file[0]<<","<<roll_check_file[256]<<","<<roll_check_file[512]<<","<<roll_check_file[768]<<","<<roll_check_file[1024]<<endl;
	// cout<<md_5_file[0]<<","<<md_5_file[256]<<","<<md_5_file[512]<<","<<md_5_file[768]<<","<<md_5_file[1024]<<endl;
	// // for(auto it = roll_check_file.begin(); it!=roll_check_file.end();it++)
	// // {
	// // 	cout<<*it<<endl;
	// // }
	// // cout<<"\nfile md5\n";
	// // for(auto it = md_5_file.begin(); it!=md_5_file.end();it++)
	// // {
	// // 	cout<<*it<<endl;
	// // }
	// compare_files("a.txt");
	// take_backup("b_snap.txt");

	// cout<<"changes vector is :"<<endl;
	// for(auto it = changes.begin(); it!=changes.end();it++)
	// {
	// 	cout<<*it<<endl;
	// }
	// cout<<"vec_same_index vector is :"<<endl;
	// for(auto it = vec_same_index.begin(); it!=vec_same_index.end();it++)
	// {
	// 	cout<<*it<<endl;
	// }

	string source_dir,destination_dir;
	cout<<"Enter the source directory path :"<<endl;
	cin>>source_dir;
	cout<<"Enter the destination directory path :"<<endl;
	cin>>destination_dir;
	copy_dir(source_dir,destination_dir);

}

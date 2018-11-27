#include "headerfiles.h"
// sending from A and B
// Divide snapshot into fixed size chunks
vector<string> roll_check_snap; //weak checksum
vector<string> md_5_snap; //strong checksum
vector<string> roll_check_file; //weak checksum
vector<string> md_5_file; //strong checksum
vector<char> changes;
vector<int> vec_same_index; 
FILE *iptr;
int source_length=0,destination_length=0;
map<string,int> source_path;

vector<long long> calculate_rolling(char input_buffer[],size_t bytesread)
{
	//cout<<"inside calculate_rolling"<<endl;
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
	//cout<<"inside calculate_md5"<<endl;
	unsigned char digest[MD5_DIGEST_LENGTH];
	MD5((unsigned char*)input_buffer, bytesread, (unsigned char*)&digest);    
	char mdString[33];
	for(int i = 0; i < 16; i++)
		sprintf((char *)&mdString[i*2], "%02x", (unsigned int)digest[i]);
	return mdString;
}

void get_chunk_id(string file_path) //for B or snap
{
	cout<<"get_chunk_id"<<endl;
	roll_check_snap.clear();
	md_5_snap.clear();
	size_t bytesread=0;
	int count=0;
	vector<long long> roll_params;
	char input_buffer[BLOCK_SIZE];	   
	iptr=fopen(file_path.c_str(),"a+");
	if(iptr==NULL)
		printf("Can not open input file \n");
	else
	{	
		memset(input_buffer,0x0,sizeof(input_buffer));	        
    	while((bytesread = fread(input_buffer, 1, sizeof(input_buffer), iptr)) > 0) //3rd parameter represents count (how many times to read) 
    	{  
    		string md5_hash = calculate_md5(input_buffer,bytesread);
        	md_5_snap.push_back(md5_hash);		//md5 hash of each chunk is stored in this vector        	
        	roll_params=calculate_rolling(input_buffer,bytesread);
        	ostringstream str1;
        	str1<<roll_params[0];
        	string roll_hash=str1.str();        	
        	roll_check_snap.push_back(roll_hash);
        	memset(input_buffer,0x0,sizeof(input_buffer));
        	count++;        	
        } 
        fclose(iptr);
        // cout<<"\n------------------------------------------------------------------\n";
        // cout<<"count for B chunks "<<count<<endl;        
        // cout<<"------------------------------------------------------------------";
    }
}
void get_successive_chunks(string file_path) // for A
{
	cout<<"inside get_successive_chunks--------------"<<endl;
	roll_check_file.clear();
	md_5_file.clear();
	size_t bytesread=0;
	int count=0;
	vector<long long> roll_params;
	char input_buffer[BLOCK_SIZE];	   
	iptr=fopen(file_path.c_str(),"rb+");
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
			string md5_hash = calculate_md5(input_buffer,bytesread);
        	md_5_file.push_back(md5_hash);		//md5 hash of each chunk is stored in this vector
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
        // cout<<"\n-------------------------------------------------------------------------------------------------\n";
        // cout<<"chunks for A is "<<chunks_count<<endl;
    }
}

// to find differences between file1,file2 using weak and strong
void compare_files(string file_path) 
{
	cout<<"inside compare_files--------"<<endl;
	changes.clear();
	vec_same_index.clear();
	int flag,j;	
	char from_file[5];
	memset(from_file,0x0,sizeof(from_file));
	iptr=fopen(file_path.c_str(),"rb+");
	if(iptr==NULL)
	{
		cout<<"File can not be opened";
		return;
	}
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
					// cout<<"matched......"<<endl;
					changes.push_back('~'); //take character which is very rare in file
					i+=BLOCK_SIZE-1;
					vec_same_index.push_back(j);					
					break;
				}
				
			}
		}
		if(flag==0)
		{
			fseek ( iptr , i , SEEK_SET);
			fread(from_file, 1, 1, iptr);				
			changes.push_back(from_file[0]);
			memset(from_file,0x0,sizeof(from_file));
		}		
	}	
	
	fclose(iptr);
}
void take_backup(string file_path)
{
	cout<<"inside take_backup"<<endl;
	string file_data="";
	char input_buffer[BLOCK_SIZE];
	int c=-1,flg=0;
	

	if(changes.empty() && roll_check_snap.size()==0)
	{			
		return;	
	}
	else if(changes.empty())
	{
		
		iptr=fopen(file_path.c_str(),"wb+");
		fclose(iptr);		
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
	iptr=fopen(file_path.c_str(),"rb+");
	for(int i=0;i<changes.size();i++)
	{
		if(changes[i]!='~')
		{
			
			file_data+=changes[i];
			
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

		}
	}
	fclose(iptr);
	iptr=fopen(file_path.c_str(),"wb+");
	
	fwrite((char*)file_data.c_str(),1,file_data.size(),iptr);
	fclose(iptr);
	
}

void copy_file(string sourceDir,string destinationDir)
{
	cout<<"inside copy_file"<<endl;
	get_chunk_id(destinationDir);
	get_successive_chunks(sourceDir);
	compare_files(sourceDir);
	take_backup(destinationDir);

}

void copy_others(string sourceDir,string destinationDir)
{ 
	cout<<"inside copy_others"<<endl;
    FILE *fptr1, *fptr2; 
    
    fptr1 = fopen(sourceDir.c_str(), "rb"); 
    if (fptr1 == NULL) 
    { 
        printf("Cannot open file");
        return;
    } 
 
    fptr2 = fopen(destinationDir.c_str(), "wb"); 
    if (fptr2 == NULL) 
    { 
        printf("Cannot open file");
        return;
    } 
    int v;
    char buffer[BLOCK_SIZE];
    while((v=fread(buffer,1,BLOCK_SIZE,fptr1))>0)
    {
        int wSize=fwrite(buffer,1,v,fptr2);
                
        bzero(buffer,BLOCK_SIZE);
    }
  
    fclose(fptr1); 
    fclose(fptr2); 
    
}
void copy_non_text_file(string sourceDir,string destinationDir)
{
	cout<<"inside copy_non_text_file"<<endl;
	// get_chunk_id(destinationDir);
	iptr=fopen(destinationDir.c_str(),"rb");
	if(!iptr)
	{
		cout<<"inside if\n";
		copy_others(sourceDir,destinationDir);
	}
	else
	{
		cout<<"else======\n";
		struct stat st;
		cout<<"lstat\n";
		lstat((sourceDir).c_str(),&st);	
		cout<<"lstat cal-------\n";
		int size_source=st.st_size;
		cout<<size_source<<" ssss\n";
		lstat((destinationDir).c_str(),&st);
		cout<<"lstat cal-2------\n";	
		int size_dest=st.st_size;
		cout<<size_dest<<" dest---------\n";

		char buff1[BLOCK_SIZE];
		char buff2[BLOCK_SIZE];

		cout<<"before ------\n";
		iptr=fopen(sourceDir.c_str(),"rb");
		cout<<"source ope--------\n";
		string hash_src="";
		size_t bytesread;
		while((bytesread = fread(buff1, 1, sizeof(buff1), iptr)) > 0)
		{
			hash_src+=calculate_md5(buff1,bytesread);
		}
		// fread(buff1,1,size_source,iptr);
		// string hash_src=calculate_md5(buff1,size_source);
		fclose(iptr);

		iptr=fopen(destinationDir.c_str(),"rb");
		cout<<"dest ope--------\n";
		string hash_des="";
		while((bytesread = fread(buff2, 1, sizeof(buff2), iptr)) > 0)
		{
			hash_des+=calculate_md5(buff2,bytesread);
		}
		// fread(buff2,1,size_dest,iptr);
		// string hash_des=calculate_md5(buff2,size_dest);
		fclose(iptr);
		
		if(hash_src==hash_des)
			return;
		copy_others(sourceDir,destinationDir);
	}

}

void copy_dir(string full_dir,string destination_dir)
{
	cout<<"inside copy_dir"<<endl;
	DIR* thisDir;
	DIR* thisDir_dest;
	struct dirent* thisFile;
	char buf[BLOCK_SIZE];
	struct stat st;
	string file_path;
	thisDir = opendir( full_dir.c_str() );
	thisDir_dest = opendir(destination_dir.c_str());	
	if (thisDir_dest)
	{	
		closedir(thisDir_dest);
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
		//cout<<"Inside Source Directory\n";
    	if((thisFile->d_name[0] == '.'))// || (thisFile->d_name == ".."))
    	{	
    		continue;
    	}
    	//sprintf(buf, "%s", thisFile->d_name);



    	lstat((full_dir + "/" + thisFile->d_name).c_str(),&st);		//set file properties of the children of full_dir

    	string fullPath=full_dir + "/" + thisFile->d_name;
    	//string.substr(start, end-start+1);
    	string subFullPath=fullPath.substr(source_length,fullPath.length()-source_length+1);
    	source_path[subFullPath]++;

        if (st.st_mode&S_IFDIR)  	//check file properties for a folder
        {	
        	cout<<thisFile->d_name<<"is dir "<<endl; 
        	copy_dir(full_dir + "/" + thisFile->d_name,destination_dir + "/" + thisFile->d_name);        	

        }
        else if(st.st_mode&S_IFREG)		//check file properties for a file
        {        	
        	string fname=thisFile->d_name;
        	int fl=0;
        	char *token = strtok((char*)fname.c_str(), "."); 	
        	printf("%s ->token aftr frst strtok\n", token);	        	  
		    char tmp[]="";
		    while (token != NULL) 
		    { 
		    	fl=1;
		        strcpy(tmp,token);
		        token = strtok(NULL, "."); 
		    } 
			printf("%s\n", tmp);
			//if no extension 
			if(fl==0)
			{
				cout<<"\nno extension";
				strcpy(tmp,"txt");
			}
			printf("%s->string extwnsion ki\n", tmp);
			string tt=(string) tmp;
			if(tt=="txt"||tt=="cpp"||tt=="h"||tt=="docx"||tt=="css"||tt=="js"||tt=="py"||tt=="xml"||tt=="sh"||tt=="c")
			{
				cout<<"this is text\n";
				copy_file(full_dir + "/" + thisFile->d_name,destination_dir + "/" + thisFile->d_name);
			}
			else
			{
				copy_non_text_file(full_dir + "/" + thisFile->d_name,destination_dir + "/" + thisFile->d_name);
			}

        	
        }
    }
    closedir( thisDir );
    // cout<<"SOURCE MAP\n";
    // for(auto k:source_path)
    // {
    // 	cout<<k.first<<"\n";
    // }
}

void delete_file(string file_path)
{
	cout<<"inside delete_file"<<endl;
	int flag;
	// string curr_dir = get_current_dir_name();
	// string full_dir;
	// full_dir = curr_dir + "/" + oldDir;
	flag = remove(file_path.c_str());
	// if(flag == 0)
	// 	cout<<"File successfully deleted"<<"\n";
	// else
	// 	cout<<"Cannot delete file"<<"\n";
}

void delete_subdir(string full_dir)
{
	cout<<"inside delete_subdir"<<endl;
	DIR* thisDir;
	struct dirent* thisFile;
	char buf[512];
	struct stat st;
	string file_path;
	thisDir = opendir( full_dir.c_str() );

	while((thisFile = readdir(thisDir))!= NULL)
	{
		if((thisFile->d_name[0] == '.'))
		{	
			continue;
		}
		sprintf(buf, "%s", thisFile->d_name);
		lstat((full_dir + "/" + thisFile->d_name).c_str(),&st);
		if (st.st_mode&S_IFDIR)
		{	  
			delete_subdir((full_dir + "/" + thisFile->d_name));


		}
		else if(st.st_mode&S_IFREG)
		{	

			remove((full_dir + "/" + thisFile->d_name).c_str());
		}
	}
	rmdir(full_dir.c_str());    
	closedir( thisDir );
}







void delete_dir(string destination_dir)
{
	cout<<"inside delete_dir"<<endl;
	// DIR* thisDir;
	DIR* thisDir_dest;
	struct dirent* thisFile;
	// char buf[BLOCK_SIZE];
	struct stat st;
	string fullPath;
	// thisDir = opendir( full_dir.c_str() );
	thisDir_dest = opendir(destination_dir.c_str());	
	while((thisFile = readdir(thisDir_dest))!= NULL)
	{
    	if((thisFile->d_name[0] == '.'))// || (thisFile->d_name == ".."))
    	{	
    		continue;
    	}

    	fullPath=destination_dir + "/" + thisFile->d_name;
    	// cout<<"fullPath is :"<<fullPath<<endl;
    	lstat(fullPath.c_str(),&st);		//set file properties of the children of full_dir
    	// cout<<"destination_length in delete_dir is :"<<destination_length<<endl;
    	// cout<<"fullPath length is :"<<fullPath<<endl;
    	// cout<<"second parameter is :"<<fullPath.length()-destination_length+1<<endl;
    	string subFullPath=fullPath.substr(destination_length,fullPath.length()-destination_length+1);
    	// cout<<"subFullPath is :"<<subFullPath<<endl;
    	
    	if(source_path[subFullPath])
    	{
    		if (st.st_mode&S_IFDIR)  	//check file properties for a folder
			{	  
				delete_dir(fullPath);
			}
			// else if(st.st_mode&S_IFREG)	
			// {
			// 	delete_file(fullPath);
			// }
    		
    	}   
    	else
    	{
			if (st.st_mode&S_IFDIR)  	//check file properties for a folder
			{	  
				delete_subdir(fullPath);
			}
			else if(st.st_mode&S_IFREG)	
			{
				delete_file(fullPath);
			}
		} 	

	}
	closedir( thisDir_dest );

}

void Snapshot(string source_dir,string destination_dir)
{
	cout<<"inside snapshot=="<<endl;
	source_length=source_dir.length();
	// cout<<"source_length is :"<<source_length<<endl;
	destination_length=destination_dir.length();
	// cout<<"destination_length is :"<<destination_length<<endl;
	copy_dir(source_dir,destination_dir);
	delete_dir(destination_dir);
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

	Snapshot(source_dir,destination_dir);
}
#include "Checksum.cpp"
// sending from A and B
// Divide snapshot into fixed size chunks


// calculate weak and strong checksum for snapshot files
void get_chunk_id(string file_path) //for B or snap
{

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

// calculate weak and strong checksum for original files
void get_successive_chunks(string file_path) // for A
{
	
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
       
    }
}

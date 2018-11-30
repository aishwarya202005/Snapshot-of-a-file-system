
// to find differences between file1,file2 using weak and strong vector
void compare_files(string file_path) 
{
	
	changes.clear();
	vec_same_index.clear();
	int flag,j;	
	char from_file[5];
	memset(from_file,0x0,sizeof(from_file));
	iptr=fopen(file_path.c_str(),"rb+");
	if(iptr==NULL)
	{
		cout<<"File can not be opened";return;
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

//copy in the file according to values in changes and index vector
void take_backup(string file_path)
{
	
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

#include "Chunks.cpp"
#include "rsync.cpp"

// function to compare text files
void copy_file(string sourceDir,string destinationDir)
{	
	get_chunk_id(destinationDir);
	get_successive_chunks(sourceDir);
	compare_files(sourceDir);
	take_backup(destinationDir);

}

//function to compare non text file
void copy_others(string sourceDir,string destinationDir)
{ 	
	FILE *fptr1, *fptr2; 

	fptr1 = fopen(sourceDir.c_str(), "rb"); 
	if (fptr1 == NULL) 
	{ 		
		return;
	} 

	fptr2 = fopen(destinationDir.c_str(), "wb"); 
	if (fptr2 == NULL) 
	{ 
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

//function to compare non text file like mp3 or mp4
void copy_non_text_file(string sourceDir,string destinationDir)
{
	get_chunk_id(destinationDir);
	iptr=fopen(destinationDir.c_str(),"rb");
	if(!iptr)
	{		
		copy_others(sourceDir,destinationDir);
	}
	else
	{		
		struct stat st;
		lstat((sourceDir).c_str(),&st);	
		int size_source=st.st_size;		
		lstat((destinationDir).c_str(),&st);			
		int size_dest=st.st_size;
		char buff1[BLOCK_SIZE];
		char buff2[BLOCK_SIZE];
		iptr=fopen(sourceDir.c_str(),"rb");		
		string hash_src="";
		size_t bytesread;
		while((bytesread = fread(buff1, 1, sizeof(buff1), iptr)) > 0)
		{
			hash_src+=calculate_md5(buff1,bytesread);
		}
		fclose(iptr);

		iptr=fopen(destinationDir.c_str(),"rb");
		string hash_des="";
		while((bytesread = fread(buff2, 1, sizeof(buff2), iptr)) > 0)
		{
			hash_des+=calculate_md5(buff2,bytesread);
		}
		fclose(iptr);
		
		if(hash_src==hash_des)
			return;
		copy_others(sourceDir,destinationDir);
	}

}


// first function to start snap from source to destination
void copy_dir(string full_dir,string destination_dir)
{	
	DIR* thisDir;
	DIR* thisDir_dest;
	struct dirent* thisFile;
	char buf[BLOCK_SIZE];
	struct stat st;
	struct stat st1;
	struct utimbuf puttime;
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
		if((strcmp(thisFile->d_name, ".") == 0) || (strcmp(thisFile->d_name, "..") == 0))
		{	
			continue;
		}    
    	lstat((full_dir + "/" + thisFile->d_name).c_str(),&st);		//set file properties of the children of full_dir

    	string fullPath=full_dir + "/" + thisFile->d_name;   
    	string subFullPath=fullPath.substr(source_length,fullPath.length()-source_length+1);
    	source_path[subFullPath]++;

        if (st.st_mode&S_IFDIR)  	//check file properties for a folder
        {        	
        	copy_dir(full_dir + "/" + thisFile->d_name,destination_dir + "/" + thisFile->d_name);   
        	lchown((destination_dir + "/" + thisFile->d_name).c_str(),st.st_uid,st.st_gid);
        	chmod((destination_dir + "/" + thisFile->d_name).c_str(),st.st_mode);
        	puttime.modtime = st.st_mtime;
        	puttime.actime = st.st_atime;
        	utime((destination_dir + "/" + thisFile->d_name).c_str(), &puttime);      	

        }
        else if(st.st_mode&S_IFREG)		//check file properties for a file
        {        	
        	iptr=fopen((destination_dir + "/" + thisFile->d_name).c_str(),"rb");
        	if(!iptr)
        	{		
        		copy_others(full_dir + "/" + thisFile->d_name,destination_dir + "/" + thisFile->d_name);
        		lchown((destination_dir + "/" + thisFile->d_name).c_str(),st.st_uid,st.st_gid);
        		chmod((destination_dir + "/" + thisFile->d_name).c_str(),st.st_mode);
        		puttime.modtime = st.st_mtime;
        		puttime.actime = st.st_atime;
        		utime((destination_dir + "/" + thisFile->d_name).c_str(), &puttime); 
        	}

        	else
        	{
        		lstat((destination_dir + "/" + thisFile->d_name).c_str(),&st1);	
        		
        		// printf("Modification time of source = %s\n", ctime(&st.st_mtime));
        		// printf("Modification time of destination = %s\n", ctime(&st1.st_mtime));

        		char stime[20];
        		strcpy(stime,ctime(&st.st_mtime));
        		char destime[20];
        		strcpy(destime,ctime(&st1.st_mtime));
    			//comparing time of modification
        		if(strcmp(stime,destime) == 0)
        		{
        			continue;
        		}
        		// cout<<"not same\n";
        		string fname=thisFile->d_name;

        		char *token = strtok((char*)fname.c_str(), ".");	        	  
        		char tmp[100]="";
        		while (token != NULL) 
        		{ 
        			strcpy(tmp,token);
        			token = strtok(NULL, "."); 
        		} 

        		string tt=(string) tmp;
        		if(tt=="txt"||tt=="cpp"||tt=="h"||tt=="docx"||tt=="css"||tt=="js"||tt=="py"||tt=="xml"||tt=="sh"||tt=="c"||tt=="html")
        		{
        			copy_file(full_dir + "/" + thisFile->d_name,destination_dir + "/" + thisFile->d_name);
        			lchown((destination_dir + "/" + thisFile->d_name).c_str(),st.st_uid,st.st_gid);
        			chmod((destination_dir + "/" + thisFile->d_name).c_str(),st.st_mode);
        			puttime.modtime = st.st_mtime;
        			puttime.actime = st.st_atime;
        			utime((destination_dir + "/" + thisFile->d_name).c_str(), &puttime); 
        		}
        		else
        		{
        			copy_non_text_file(full_dir + "/" + thisFile->d_name,destination_dir + "/" + thisFile->d_name);
        			lchown((destination_dir + "/" + thisFile->d_name).c_str(),st.st_uid,st.st_gid);
        			chmod((destination_dir + "/" + thisFile->d_name).c_str(),st.st_mode);
        			puttime.modtime = st.st_mtime;
        			puttime.actime = st.st_atime;
        			utime((destination_dir + "/" + thisFile->d_name).c_str(), &puttime); 
        		}

        	}        	
        	
        }
    }
    closedir( thisDir );
}


//delete file if not present in source
void delete_file(string file_path)
{
	int flag;	
	flag = remove(file_path.c_str());	
}

// delete directory if not present in destination
void delete_subdir(string full_dir)
{
	DIR* thisDir;
	struct dirent* thisFile;
	char buf[512];
	struct stat st;
	string file_path;
	thisDir = opendir( full_dir.c_str() );

	while((thisFile = readdir(thisDir))!= NULL)
	{
		if((strcmp(thisFile->d_name, ".") == 0) || (strcmp(thisFile->d_name, "..") == 0))
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

// delete files and folders if not exist in source
void delete_dir(string destination_dir)
{

	DIR* thisDir_dest;
	struct dirent* thisFile;
	struct stat st;
	string fullPath;	
	thisDir_dest = opendir(destination_dir.c_str());	
	while((thisFile = readdir(thisDir_dest))!= NULL)
	{
		if((strcmp(thisFile->d_name, ".") == 0) || (strcmp(thisFile->d_name, "..") == 0))
		{	
			continue;
		}

		fullPath=destination_dir + "/" + thisFile->d_name;
    	lstat(fullPath.c_str(),&st);		//set file properties of the children of full_dir
    	string subFullPath=fullPath.substr(destination_length,fullPath.length()-destination_length+1);
    	
    	if(source_path[subFullPath])
    	{
    		if (st.st_mode&S_IFDIR)  	//check file properties for a folder
    		{	  
    			delete_dir(fullPath);
    		}

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
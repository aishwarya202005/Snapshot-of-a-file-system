#include<bits/stdc++.h>
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
#include <map>

using namespace std;

void copy_file(string sourceDir,string destinationDir)
{
	int fdSource = open(sourceDir.c_str(), O_RDONLY);
	int fdDestination = open(destinationDir.c_str(), O_RDWR|O_CREAT, S_IRUSR | S_IWUSR);
	// After successfully opening
	char buf[1024];
	ssize_t countSource;
	ssize_t countDestination;
	while((countSource = read(fdSource, buf, sizeof buf)) > 0)
	{
  		countDestination = write(fdDestination, buf, countSource);  
  	}
  	if(countDestination != -1)
  	{
  		cout<< " Copied file successfully "<<"\n";
  	}
  	
  	close(fdDestination);
	close(fdSource);
}



void copy_dir(string full_dir,string destination_dir)
{
	DIR* thisDir;
    struct dirent* thisFile;
    char buf[512];
    struct stat st;
    string file_path;
    //cout<<"shafiya3"<<endl;
    thisDir = opendir( full_dir.c_str() );
    //cout<<"shafiya2"<<endl;
 
    mkdir(destination_dir.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    while((thisFile = readdir(thisDir))!= NULL)
   	{
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
        	copy_dir(full_dir + "/" + thisFile->d_name,destination_dir + "/" + thisFile->d_name);
        	
           	
        }
        else if(st.st_mode&S_IFREG)		//check file properties for a file
        {	
        	copy_file(full_dir + "/" + thisFile->d_name,destination_dir + "/" + thisFile->d_name);

        		//full_dir = full_dir + "/" + thisFile->d_name;
        		//cout<<"full_dir is :"<<full_dir<<endl;		
        }
    }
    //rmdir(full_dir.c_str());    
	closedir( thisDir );
}

int main()
{
	string source_dir,destination_dir;
	cout<<"Enter the source directory path :"<<endl;
	cin>>source_dir;
	cout<<"Enter the destination directory path :"<<endl;
	cin>>destination_dir;
	copy_dir(source_dir,destination_dir);
	
	return 0;
}
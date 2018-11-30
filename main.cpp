#include "snapshot.cpp"

//snapshot function which calls functions for comparison of checksum
void Snapshot(string source_dir,string destination_dir)
{
	source_length=source_dir.length();
	destination_length=destination_dir.length();
	copy_dir(source_dir,destination_dir);
	delete_dir(destination_dir);
}

//main function which calls snapshot function
int main()
{
	string source_dir,destination_dir;
	cout<<"Enter the source directory path :"<<endl;
	cin>>source_dir;
	cout<<"Enter the destination directory path :"<<endl;
	cin>>destination_dir;
	
	if(destination_dir[0]!='.')
		destination_dir="."+destination_dir;

	// source_dir="/home/kopal/Documents/Subjects/OS/OS_PROJECT/demo";
	// destination_dir="/home/kopal/Documents/Subjects/OS/OS_PROJECT/.demo_snap";

	

	Snapshot(source_dir,destination_dir);
}
//All global variables and objects declared here
#include<bits/stdc++.h>
#include <openssl/md5.h>
#include <dirent.h>
#include <termios.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <sstream>
#include <errno.h>
#include <utime.h>
using namespace std;
#define M 65536
#define BLOCK_SIZE 512
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;


vector<string> roll_check_snap; //weak checksum
vector<string> md_5_snap; //strong checksum
vector<string> roll_check_file; //weak checksum
vector<string> md_5_file; //strong checksum
FILE *iptr;
vector<char> changes;
vector<int> vec_same_index; 

map<string,int> source_path;
int source_length=0,destination_length=0;

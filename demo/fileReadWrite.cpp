int main()
{
	
int fileread = open("/home/shafiya/Documents/IIITH_Monsoon_2018/OS/assignment1/check.c", O_RDONLY);
int filewrite = open("/home/shafiya/check_new1.c", O_RDWR|O_CREAT, S_IRUSR | S_IWUSR);
// After successfully opening ...
char buffer[1024];
ssize_t inlen;
ssize_t outlen;
while((inlen = read(fileread, buffer, sizeof buffer)) > 0)
{
  outlen = write(filewrite, buffer, inlen);  // Note use of inlen
  
}

close(filewrite);
close(fileread);
return 0;
}

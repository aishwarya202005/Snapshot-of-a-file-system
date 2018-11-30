#include "headerfiles.h"

//calculating rolling checksum ----- checksum of current chunk using previous chunk checksum 
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

//function to calculate md5 and return its value
string calculate_md5(char input_buffer[],size_t bytesread)
{
	unsigned char digest[MD5_DIGEST_LENGTH];
	MD5((unsigned char*)input_buffer, bytesread, (unsigned char*)&digest);    
	char mdString[33];
	for(int i = 0; i < 16; i++)
		sprintf((char *)&mdString[i*2], "%02x", (unsigned int)digest[i]);
	return mdString;
}
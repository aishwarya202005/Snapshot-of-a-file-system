#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>


 launching the lsd program.
    thedirectory = opendir("/home/shafiya/Documents/IIITH_Monsoon_2018/OS/assignment1");

    //If a file is found (readdir returns a NOT NULL value), the loop starts/keep going until it has listed all of them. 
    while((thefile = readdir(thedirectory)) != NULL) 
    {   
        //We sprint "thedirectory/thefile" which defines the path to our file 
        sprintf(buf, "%s", thefile->d_name);
        //Then we use stat function in order to retrieve information about the file
        stat(buf, &thestat);

        //Now, we can print a few things !
        // Here's the right order
        // [file type] [permissions] [number of hard links] [owner] [group] [size in bytes] [time of last modification] [filename]

        // [file type]
        //Let's start with the file type
        //The stat manual is pretty complete and gives details about st_mode and S_IFMT: http://manpagesfr.free.fr/man/man2/stat.2.html
        //
        switch (thestat.st_mode & S_IFMT) {
            case S_IFBLK:  printf("b "); break;
            case S_IFCHR:  printf("c "); break; 
            case S_IFDIR:  printf("d "); break; //It's a (sub)directory 
            case S_IFIFO:  printf("p "); break; //fifo
            case S_IFLNK:  printf("l "); break; //Sym link
            case S_IFSOCK: printf("s "); break;
            //Filetype isn't identified
            default:       printf("- "); break;
                }
        //[permissions]
        //Same for the permissions, we have to test the different rights
        //READ http://linux.die.net/man/2/chmod 
        printf( (thestat.st_mode & S_IRUSR) ? " r" : " -");
        printf( (thestat.st_mode & S_IWUSR) ? "w" : "-");
        printf( (thestat.st_mode & S_IXUSR) ? "x" : "-");
        printf( (thestat.st_mode & S_IRGRP) ? "r" : "-");
        printf( (thestat.st_mode & S_IWGRP) ? "w" : "-");
        printf( (thestat.st_mode & S_IXGRP) ? "x" : "-");
        printf( (thestat.st_mode & S_IROTH) ? "r" : "-");
        printf( (thestat.st_mode & S_IWOTH) ? "w" : "-");
        printf( (thestat.st_mode & S_IXOTH) ? "x" : "-");

        // [number of hard links] 
        // Quoting: http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html
        // "This count keeps track of how many directories have entries for this file. 
        // If the count is ever decremented to zero, then the file itself is discarded as soon as no process still holds it open."
        printf("\t%ld ", (long unsigned int)thestat.st_nlink);

        //[owner] 
        // http://linux.die.net/man/3/getpwuid
        tf = getpwuid(thestat.st_uid);
        printf("\t%s ", tf->pw_name);

        //[group]
        // http://linux.die.net/man/3/getgrgid
        gf = getgrgid(thestat.st_gid);
        printf("\t%s ", gf->gr_name);

        //And the easy-cheesy part
        //[size in bytes] [time of last modification] [filename]
        printf("%zu",thestat.st_size);
        printf(" %s", thefile->d_name);
        printf(" %s", ctime(&thestat.st_mtime));
    }
    closedir(thedirectory);
}
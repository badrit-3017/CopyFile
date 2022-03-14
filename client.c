/*====================================================================== 
|   
|
|   Name: client.c
|
|   Written by: Badrit Bin Imran- March 2021
|
|   Purpose: Client Code 
|  
|   Compile: gcc -o client client.c 
|
|
|   Description of parameters: Parameter 1: Filename to be copied from
|
|------------------------------------------------------------------*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#define SF "serv_fifo"
#define CF "cl_fifo"
int copyfile(const char *name1, const char *name2);

// struct data
struct data 
{
    int size;
    char text[1024];
};

/*====================================================================== 
|   
|   Name of method: main method
|
|   Parameters: None
|
|   return type: int
|   
|   Description: The main method handles all signals 
|   
|------------------------------------------------------------------*/
int main( int argc, char *argv[] )
{
    //File descriptors
    int fd, i,cd;
    char msgbuf[PIPE_BUF]; //char array for the message
    char filename[512]; //Our limit for filenames is 512 characters
    void make_msg( char mb[], char input[]);
    //Error checking for wrong format of arguments
    if( argc != 2 )
    {
        printf( "\t [Client] Usage: ./client filename_to_be_copied\n" );
        exit( 1 );
    }
    //Opening client -> server FIFO with error checks
    if( (fd = open( SF, O_WRONLY | O_NONBLOCK )) < 0)
    {
      perror( SF ); exit( 1 );          
    }
    //Too long filename
    if( strlen( argv[i] ) > 510 )
    printf( "\t [Client] Too long: %s\n", argv[i] );
    else
    {
        //Dump the filename into our array
        make_msg(filename,argv[1]);
        //Write the filename to the FIFO
        write( fd, filename, 512 ); 
        //Print off the filename
        printf("\t [Client] File to be uploaded to Server -> %s \n",filename);
        //Call our subroutine which executes the copying from client to server
        copyfile(argv[1],SF);
    }  
    //Closing the file descriptor
    close( fd );
    //Opening the server-> client FIFO with error checks
    if( (cd = open( CF, O_RDWR | O_NONBLOCK )) < 0)
    {
      perror( CF ); exit( 1 );          
    }
    //Waiting for the server to supply end of transfer message
    while(read( cd, msgbuf, PIPE_BUF )<=0);
    //Print message obtained from server
    printf("\t [Client] Message receieved from Server -{%s}. \n",msgbuf);
    printf("\t [Client] Disconnecting now... \n");
    //Close file descriptor
    close(cd);
    return 0;
} /* end main */

/*====================================================================== 
|   
|          
|   Name of method: copyfile
|
|   Written by: Badrit Bin Imran
|
|
|   Description of parameters: 
|               i) name1 = name of file to be copied from
|               ii) name1 = name of file to be copied to
|
|   Return Value: Returns total number of bytes copied.
|                 Returns negative for errors
|      
|------------------------------------------------------------------*/
int copyfile(const char *name1, const char *name2)
{
    FILE *fptr1, *fptr2;
    // Open one file for reading
    fptr1 = fopen(name1, "r");
    if (fptr1 == NULL)
    {
        printf("\t [Client] Error opening file %s \n", name1);
        exit(1);
    }
    //Open file for writing
    fptr2 = fopen(name2, "w");
    if (fptr2 == NULL)
    {
        printf("\t [Client] Error writing to FIFO %s \n", name2);
        exit(1);
    }
    //Size of data to be transferred
    size_t bytes;
    struct data input;
    //Iterating through the whole file and reading in 1028 bytes of data(size of struct = 1028)
    while (0 < (bytes = fread(&input, 1,sizeof(struct data),  fptr1)))
    {
        //Writing to FIFO
        input.size = bytes;
        fwrite (&input, 1,  bytes, fptr2);
    }
    //Closing file descriptors
    fclose(fptr1); fclose(fptr2);
}
/*====================================================================== 
|   
|          
|   Name of method: make_msg
|
|   Written by: Professor Jacques Beland
|
|   Modified by: Badrit Bin Imran
|
|   Description of parameters: 
|               i) mb[] - output array
|               ii) input[] - input array
|
|   Return Value: void
|      
|------------------------------------------------------------------*/
void make_msg( char mb[], char input[])
{
    int i;
    //We iterate through the loop and set all characters to blank
    for( i = 1; i < 511; i++ )
    mb[i] = ' ';
    mb[i] = '\0'; //setting last character to null
    i = 0;
    //Iterating and setting values
    while( input[i] != '\0' )
    {   
        mb[i] = input[i];
        i++;
    }
    //setting last character to null
    mb[i] = '\0';
} 

//End of File



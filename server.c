/*====================================================================== 
|   
|
|   Name: server.c
|
|   Written by: Badrit Bin Imran- March 2021
|
|   Purpose: Server Code 
|  
|   Compile: gcc -o server server.c
|
|
|   Description of parameters: No parameters required
|
|      
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
#define SF "serv_fifo"
#define CF "cl_fifo"

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
    //Declaring all integers
    int cd; //cd stores status after opening the Server-write FIFO
    char alldone[15] = "all done"; //Message to be written to server-write FIFO
    char* destination = "sourcefile_local_clone.txt"; //destination of file copy
    char* source = "serv_fifo"; //source to be copied from (namely server FIFO)
    int fd, i, fd2, done = 0,size =0; //fd,f2 are filedescriptors 
    char msgbuf[PIPE_BUF]; //Stores data transferred from the FIFO
    FILE *fptr1, *fptr2; //filepointers
    char filename[512];
    //Error checking to see if write file exits
    FILE * fptr;
    fptr = fopen(destination, "w");
    if(fptr==NULL)
    {
        printf("Error creating the write file \n");
        exit(1);
    }
    fclose(fptr);

    //Error checks for creating the Server to Client FIFO
    if(mkfifo(CF,0666) == -1)
        if(errno != EEXIST)
        {
        perror("receiver: mkfifo");
        exit( 1 ); 
        }

    //Error checks for creating the Client to Server FIFO
    if(mkfifo(SF,0666) == -1)
        if(errno != EEXIST)
        {
        perror("receiver: mkfifo");
        exit( 1 ); 
        }
    //File pointer for trying to open the Client to Server FIFO
    fptr1 = fopen(SF, "rw");
    if (fptr1 == NULL)
    {
        printf("Error opening file %s \n", SF);
        exit(1);
    }
    //Condition for checking if the filename is supplied
    int condition = 0;
    size_t bytes; //size of data transferred
    struct data input; //Struct for input

    //Opening the second FIFO    
    if( (cd = open( CF, O_WRONLY | O_NONBLOCK )) < 0 )
    { 
        perror( CF );
        exit( 1 ); 
    }
    
    //Error checks for opening the destination file
        fptr2 = fopen(destination, "w");
        if (fptr2 == NULL)
        {
            printf("Error writing to FIFO %s \n", destination);
            exit(1);
        }
    //While loop for continuosly checking from Clients
    while( !done )
    {
        //Nested while loop for each client
        while(!done)
        {
            //Checking if the filename has been supplied
            if(condition == 0)
            {
                //Reading the filename and printing it out
                fread(filename,1,512,fptr1);
                printf("[Server] File name recieved from client -> %s \n",filename);
                condition = 1;
            }
            //Reading each block of data equal to our struct
            bytes = fread(&input, 1, sizeof(struct data), fptr1);
            //Error checks for reading
            if( bytes < 0 )
            {
                perror( "read" );
                exit( 1 );
            }
            //Write if the size is greater than zero
            if(bytes>1) 
            {
                //If our filepointer is closed after a previous transfer, we re-open it
                if(fptr2 ==NULL) fptr2 = fopen(destination, "w");
                //We only write the text part of the struct
                fwrite(input.text, 1, bytes, fptr2);       

            }     
            if(bytes<1024 && bytes>1) 
            {
                //Case: We have reached the last block to be sent
                printf("[Server] Sending message that the file transfer is complete \n");     
                //Closing the filepointer        
                fclose( fptr2 );
                fptr2 = NULL;
                //Writing a message that we are all done
                write( cd, alldone, 15 ); 
                bytes=0;
                condition = 0;
                break;
            }   
        }         
    }  
    //Closing the file descriptors
    fclose (fptr1);      
 
return 0;
} 
//End file




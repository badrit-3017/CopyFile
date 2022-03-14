
# Copy File (With Pipes) ![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
This is an application that achieves inter process communication through pipes in C. The client reads the data block by block from the source file and forwards the data to the server. The server then writes the data to the destination.


## Compilation

You need a UNIX system to compile and run. First we start up the server:

```console
gcc -o server server.c
```
Then for the client:
```console
gcc -o client client.c source
```
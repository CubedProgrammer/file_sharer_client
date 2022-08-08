# file\_sharer\_client
Client for sharing files.
## Compilation and Installation
```
clang -O3 -c *.c
clang -o fileshare *.o
```
Then move the resulting executable to a directory such as /usr/local/bin.
```
mv fileshare /usr/local/bin
```
Windows is not natively supported.
However, as this is a command line program, WSL will work just fine, probably better actually.
## Usage
Run the program, the first argument is the address of the server to connect to.

Press c to create a virtual room to upload a file to, j to join a room and receive a file.

If you're joining, to receive, change the setting before pressing j, press s to save file after receiving, and d to display.
You will not be able to change it after you have joined the virtual room.

If you're uploading, type in the name of the file and hit enter, press tab to autocomplete.
If multiple files match, you will be shown the options.

Files are downloaded to the present working directory. The file will have extension .file\_share.

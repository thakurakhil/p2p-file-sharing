README :


The executables are produced in the respective folders of client and server with the name myserver and myclient
To Run: ./bash.sh
For running files:
----SERVER----
	gcc server/server.c -o server/myserver-lssl -lcrypto
    ./server path_to_folder_name_of_server_files
----CLIENT----
	gcc client/client.c path_of_folder_that_up_want_to_upload


COMMANDS SUPPORTED :

IndexGet --longlist 

IndexGet --shortlist <starting-time-stamp> <ending-time-stamp>

IndexGet --regex string_to_search_for_files

FileHash --verify Name-of-file

FileHash --checkall

FileDownload Path-For-Name-of-file

FileDownload --TCP Path-For-Name-of-file

FileDownload --UDP Path-For-Name-of-file
 
FileUpload Path-For-Name-of-file
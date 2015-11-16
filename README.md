# IsServerUp

Simple program to make sure servers (URLs) are returning HTTP 200 response codes.

## Build

You will need to make sure the CURL library is installed:

    CentOS:   sudo yum install libcurl-devel
    Ubuntu:   sudo apt-get install libcurl4-openssl-dev
    
Then build with:

    g++ IsServerup.cpp -o IsServerUp -lcurl

## Usage:

    IsServerUp server1.com server2.com serverX.com ref_server.com

The last parameter (the "reference server") is used to determine if there is a network connection. If the reference server cannot be "pinged", then no test is performed as network connectivity is considered non-existent/unreliable.

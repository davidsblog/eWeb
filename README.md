# eWeb
An experiment in monitoring entropy on Linux machines.

#Build

Build the code like this:

    git clone https://github.com/davidsblog/eWeb
    cd eWeb
    make

#Run

You can run the server like this:

    sudo ./eWeb 80

Will run the server on port 80.  If you already have a webserver running on port 80, you'll need to change to a different port number.
When the webserver is running, just use a browser to visit http://localhost:80 (or whatever port you have specified).

#NOTE
**You probably don't want to expose this server onto the public internet!**  It would tell everybody how much entropy your machine has and therefore indicates the quality of random numbers your machine can generate.

I am only using this for experiments and learning.

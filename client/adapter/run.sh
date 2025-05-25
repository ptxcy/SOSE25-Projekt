# simple to run the test adapter, just one to three lines, no arguments, if-statements, or loops or echos
#!/bin/bash
g++ -std=c++17 -Wall -I/usr/local/include -I/opt/homebrew/include -L/usr/local/lib -L/opt/homebrew/lib -o http_adapter http-adapter.cpp -lcpr
./http_adapter

#!/bin/bash
gcc user_tree.h utilities.h server.h client.h playground.h

gcc -o server server.c user_tree.c utilities.c server_functions.c -lpthread
gcc -o client client.c utilities.c client_functions.c

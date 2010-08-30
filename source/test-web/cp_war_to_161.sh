#!/bin/bash

SERVER=172.16.0.161

# rsync dictionary
#rsync -rvP /home/jun/research-data/jma-data/ jun.jiang@$SERVER:/home/jun.jiang/research-data/jma-data

# copy WAR package file
scp dist/ijma.war jun.jiang@$SERVER:/opt/apache-tomcat-5.5.28/webapps

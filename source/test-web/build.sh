#!/bin/bash

# default path
TOMCAT_APP_PATH=/usr/share/tomcat5.5/webapps
# the deployment file name
WAR_NAME=jmacomp.war

if [ -d $TOMCAT_APP_PATH ]; then
    echo ">>>>> $TOMCAT_APP_PATH exists."
    echo ">>>>> cleaning build files."
    ant clean

    echo ">>>>> removing deployment files."
    sudo rm $TOMCAT_APP_PATH/$WAR_NAME

    echo ">>>>> building."
    ant

    echo ">>>>> deploying."
    ant run
else
    echo "$TOMCAT_APP_PATH not exists."
    TOMCAT_APP_PATH=/home/wisenut/tomcat/apache-tomcat-5.5.27
    if [ -d $TOMCAT_APP_PATH ]; then
        echo "$TOMCAT_APP_PATH exists."
        echo ">>>>> cleaning build files."
        ant clean

        echo ">>>>> removing deployment files."
        rm $TOMCAT_APP_PATH/$WAR_NAME

        echo ">>>>> building."
        ant

        echo ">>>>> deploying."
        ant run
    else
        echo "$TOMCAT_APP_PATH not exists."
        echo "please set a correct tomcat app path in this script file."
        exit 1
    fi
fi

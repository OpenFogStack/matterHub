#!/bin/bash
BASEDIR=$(dirname "$0")
cd $BASEDIR;
source ./settings.sh
read -p "Do you want to clean up the esp32 and m5stack before flashing? Type in yes or click enter: " prompt
if [[ $prompt == "y" || $prompt == "Y" || $prompt == "yes" || $prompt == "Yes" ]]
then
	echo "Cleaning the esp32 and m5stack please wait..."
	sh ./cleanup.sh
fi
osascript -e 'tell app "Terminal" to do script "cd '$BASEDIR'; source '$BASEDIR'/settings.sh; sh ./hub_start.sh"' $BASEDIR
read -p "Wait until Hub is flashing and click enter " prompt
osascript -e 'tell app "Terminal" to do script "cd '$BASEDIR'; source '$BASEDIR'/settings.sh; sh ./enddevice_start.sh"' $BASEDIR
read -p "Wait until Enddevice is flashing and click enter " prompt
sh ./chiptool_start.sh
osascript -e 'tell app "Terminal" to do script "matter switch binding unicast 1 333 1" in window 3'
echo "autostart process is completed!"
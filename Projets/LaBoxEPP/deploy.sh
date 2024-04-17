#!/bin/bash

BASEDIR=$(dirname "$0")
TARGET_DIR=/etc/laboxepp/
DATA_DIR=/var/laboxepp/
LOG_DIR=/var/log/laboxepp/

# Create  dir
sudo mkdir $TARGET_DIR
sudo chmod -R 777 $TARGET_DIR
sudo mkdir $DATA_DIR
sudo chmod -R 777 $DATA_DIR
sudo mkdir $LOG_DIR
sudo chmod -R 777 $LOG_DIR

# copy necessary files
cp -r $BASEDIR/imagedisplay $TARGET_DIR
cp -r $BASEDIR/webserver $TARGET_DIR
cp -r $BASEDIR/cfg $TARGET_DIR
cp $BASEDIR/cfg/laboxepp.env $TARGET_DIR

# deploy necessary components
sudo apt-get install hostapd
sudo apt-get install dnsmasq
sudo apt-get install nginx

# web framework
pip install Flask
# pdf conversion
pip install pdf2image
pip install pypdfium2
# image display
pip install pygame

# configuration of dependencies
# -hostapd
sudo cp $BASEDIR/cfg/hostapd /etc/default/
sudo cp $BASEDIR/cfg/hostapd.conf /etc/hostapd
# -dhcpcd
sudo cp $BASEDIR/cfg/dhcpcd.conf_hotspot /etc/dhcpcd.conf
# -nginx
sudo cp $BASEDIR/cfg/labox_epp_flask_app /etc/nginx/sites-available/
sudo ln -s /etc/nginx/sites-available/laboxepp_flask_app /etc/nginx/sites-enabled/

# configuration of services
sudo cp $BASEDIR/cfg/eppdisplay.service /etc/systemd/system/
sudo cp $BASEDIR/cfg/eppweb.service /etc/systemd/system/

sudo systemctl daemon-reload
sudo systemctl enable eppdisplay
sudo systemctl start eppdisplay

sudo systemctl enable eppweb
sudo systemctl start eppweb

sudo systemctl restart dhcpcd
sudo systemctl start hostapd
sudo systemctl start dnsmasq

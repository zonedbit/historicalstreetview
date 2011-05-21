#!/bin/bash
#===============================================================================
#
#          FILE:  n900Adhoc.sh
# 
#         USAGE:  ./n900Adhoc.sh 
# 
#   DESCRIPTION:  Setting up the Ad-hoc mode and activating IPv4
#									forwarding. Before executing this script, should
#									the N900 configured to Ad-hoc mode with enabled
#									DHCP-Server. Easiest way is to install and use the
#									PC-Connectivity Manger on the N900.
# 
#       VERSION:  1.0
#       CREATED:  05/21/2011 11:34:21 AM CEST
#===============================================================================


#-------------------------------------------------------------------------------
#   Global vars
#-------------------------------------------------------------------------------
wifiInterface=wlan0
wifiAdhocName=devel_adhoc

interfaceOut=eth0

#-------------------------------------------------------------------------------
#   User has to be root
#-------------------------------------------------------------------------------
if [ `id -u` -ne 0 ] ; then
	echo "You are not root"
	exit 1;
fi

#-------------------------------------------------------------------------------
#  Stopping the KDE Network Manager, so a manual managing of
#  the interfaces is possible
#-------------------------------------------------------------------------------
pidof NetworkManager >/dev/null
if [ $? -eq 0 ] ; then
	echo "Stopping network-manager"
	initctl stop network-manager >/dev/null
fi

#-------------------------------------------------------------------------------
#   Setup the Wi-Fi interface in the Ad-hoc mode, here wlan0
#-------------------------------------------------------------------------------
echo "Configure the Wi-Fi interface $wifiInterface"
ifconfig $wifiInterface up												\
	&& iwconfig $wifiInterface mode Ad-Hoc					\
	&& iwconfig $wifiInterface essid $wifiAdhocName	\
	&& iwconfig $wifiInterface key off							\
	&& dhclient $wifiInterface


#-------------------------------------------------------------------------------
#   Setup IPv4 forwarding
#-------------------------------------------------------------------------------
echo "Activating IPv4 forwarding"
echo 1 > /proc/sys/net/ipv4/ip_forward

#-------------------------------------------------------------------------------
#   Setup iptables
#-------------------------------------------------------------------------------
echo "Configure iptables outgoing interface is: $interfaceOut"
modprobe iptable_nat>/dev/null	\
	&& iptables -F				\
	&& iptables -t nat -F	\
	&& iptables -t nat -A POSTROUTING -o $interfaceOut -j MASQUERADE

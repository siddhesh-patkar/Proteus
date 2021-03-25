#!/bin/sh
# Raspbian Configuration Script

hostname=raspberrypi

# Check if it's been configured already
if [ "$hostname" = "$(hostname)" ]; then
    ls /etc/init.d/iotbuilder >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
        exit 0
    fi
fi

if [ "$1" = "check" ]; then
    # Indicate that configuration required
    return 1
fi

echo "Initial board configuration. Don't close the window.\n"

# Set host name
if [ "$hostname" != "$(hostname)" ]; then
    echo "Changing host name to '$hostname'"
	sudo sed -i 's/'$(hostname)'/'$hostname'/g' /etc/hosts
    echo $hostname >/etc/hostname

    # Check if further configuration required
    ls /etc/init.d/iotbuilder >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
        echo "Configuration complete. Restarting..."
        shutdown -r now
    fi
fi

# Enable required hardware

# I2C
ls /dev/i2c-1 >/dev/null 2>&1
if [ "$?" -ne "0" ]; then
    sed -i 's/#dtparam=i2c_arm=on/dtparam=i2c_arm=on/g' /boot/config.txt
    grep 'i2c-bcm2708' /etc/modules >/dev/null 2>&1
    if [ "$?" -ne "0" ]; then
        echo "i2c-bcm2708" >> /etc/modules
    fi
    grep 'i2c-dev' /etc/modules >/dev/null 2>&1
    if [ "$?" -ne "0" ]; then
        echo "i2c-dev" >> /etc/modules
    fi
fi

#SPI
ls /dev/spidev0.0 >/dev/null 2>&1
if [ "$?" -ne "0" ]; then
    sed -i 's/#dtparam=i2c_arm=on/dtparam=i2c_arm=on/g' /boot/config.txt
fi
ls /dev/spidev0.1 >/dev/null 2>&1
if [ "$?" -ne "0" ]; then
    sed -i 's/#dtparam=spi=on/dtparam=spi=on/g' /boot/config.txt
fi


#Serial 0
ls /dev/serial0 >/dev/null 2>&1
if [ "$?" -ne "0" ]; then
    grep 'dtoverlay=pi3-miniuart-bt' /boot/config.txt >/dev/null 2>&1
    if [ "$?" -ne "0" ]; then
        echo "" >> /boot/config.txt
        echo "# Enable Serial 0" >> /boot/config.txt
        echo "dtoverlay=pi3-miniuart-bt" >> /boot/config.txt
    fi
    sed -i 's/console=serial0,115200//g' /boot/cmdline.txt
    sed -i 's/console=ttyAMA0,115200//g' /boot/cmdline.txt
fi

#Camera Enabling
if grep "start_x" /boot/config.txt; then
    if grep "start_x=0" /boot/config.txt; then
        sed -i "s/start_x=0/start_x=1/g" /boot/config.txt
    fi
else
    echo "start_x=1" >> /boot/config.txt
    echo "gpu_mem=128" >> /boot/config.txt
fi
   

# Install required software

# Update repository
apt-get update || exit 1

# Update all packages
apt-get -y dist-upgrade || exit 1

# Update repository (again)
apt-get update || exit 1

# Install required packages
apt-get -y install python3 python3-pip python3-smbus python3-serial python3-pygame python3-pil.imagetk avahi-daemon avahi-discover avahi-utils libnss-mdns mdns-scan libav-tools || exit 1

# Install Python 3 packages

# Install WiringPi
python3 -m pip install wiringpi || exit 1

# Install PRi.GPIO
python3 -m pip install RPi.GPIO || exit 1

# Install spidev
python3 -m pip install spidev || exit 1

# Install picamera
python3 -m pip install picamera || exit 1

# Install Twython
python3 -m pip install twython || exit 1

# Create AVAHI SSH service
SSH_SERVICE="/etc/avahi/services/ssh.service"
cat >$SSH_SERVICE << EOF
<?xml version='1.0' standalone='no'?>
<!DOCTYPE service-group SYSTEM 'avahi-service.dtd'>
<service-group>
 <name replace-wildcards='yes'>Secure Shell on %h</name>
 <service>
  <type>_ssh._tcp</type>
  <port>22</port>
 </service>
</service-group>
EOF
# Mark as non-executable
chmod 644 $SSH_SERVICE
# Start AVAHI
/etc/init.d/avahi-daemon start

# Create /vsm directory for the scripts
mkdir /vsm 2>/dev/null
chown pi:pi /vsm

# Sketch startup script
cp /boot/iotbuilder /etc/init.d/
chmod 775 /etc/init.d/iotbuilder
chown root:root /etc/init.d/iotbuilder
# Run sketch on boot
update-rc.d iotbuilder defaults

# Restart
echo "Configuration complete. Restarting..."
shutdown -r now

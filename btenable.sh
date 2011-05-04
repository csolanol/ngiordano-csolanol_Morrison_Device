#!/system/xbin/busybox sh
sleep 5
killall hciattach
bt_init /system/etc/bt_init.config &
sleep 7
chmod -R 0777 /sys/class/rfkill
chmod 0777 /sys/class/rfkill/rfkill*/state
chmod 0777 /sys/class/rfkill/rfkill*/type
chown -R bluetooth.bluetooth /sys/class/rfkill

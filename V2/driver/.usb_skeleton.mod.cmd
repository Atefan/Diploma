savedcmd_/home/stefan/driver/usb_skeleton.mod := printf '%s\n'   usb_skeleton.o | awk '!x[$$0]++ { print("/home/stefan/driver/"$$0) }' > /home/stefan/driver/usb_skeleton.mod

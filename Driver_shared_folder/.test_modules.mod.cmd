savedcmd_/media/sf_Driver_shared_folder/test_modules.mod := printf '%s\n'   main.o | awk '!x[$$0]++ { print("/media/sf_Driver_shared_folder/"$$0) }' > /media/sf_Driver_shared_folder/test_modules.mod

#!/bin/sh -x

# Function:     setup
#
# Description:  - Check if required commands exits
#               - Export global variables
#               - Check if required config files exits
#               - Create temporary files and directories
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
setup()
{
#TODO Total test case
export TST_TOTAL=1

export TCID="setup"
export TST_COUNT=0
RC=0

trap "cleanup" 0

#TODO add setup scripts
dbg=$(mount | grep debugfs | wc -l)
if [ $dbg -eq 0 ]; then
 mount -t debugfs none /sys/kernel/debug
fi

return $RC
}

# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup()
{
RC=0

#TODO add cleanup code here
echo "clean up..."

return $RC
}

#main
setup

platStr=`platfm.sh`

if [ "$platStr" = 'IMX6Q-Sabre-SD' ] || [ "$platStr" = 'IMX6DL-Sabre-SD' ]; then
	mmc=mmc0
else
	mmc=mmc1
fi

clock=$(cat /sys/kernel/debug/$mmc/ios | grep clock | awk '{print $2}')

[ $clock -eq 52000000 ] || RC=1

return $RC

#!/bin/bash

# Daemon script to limit access to the logs.
# Start with './logd.sh &' as a normal user and make sure
# the owner of codybot.log is the normal user.
# Then start the bot with './codybot -l log.fifo'

[ -e log.fifo ] || {
	mkfifo log.fifo
	chgrp codybot log.fifo
	chmod g+w log.fifo
}

tail -f log.fifo >> codybot.log


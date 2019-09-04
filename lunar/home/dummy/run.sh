#!/bin/bash

echo "#### UID $UID listening to run.fifo...####"
tail -f run.fifo |while read LINE; do
	timeout 30s $LINE &>cmd.output
	echo $? > cmd.ret
	echo "####$LINE####"
	cat cmd.output
	echo "####$LINE####"
done


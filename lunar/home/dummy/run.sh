#!/bin/bash

tail -f run.fifo |while read LINE; do
	timeout 30s $LINE &>cmd.output
	echo $? >cmd.ret
	echo "####$LINE####"
	cat cmd.output
done


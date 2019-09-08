#!/bin/bash

echo "#### UID $UID listening to run.fifo...####"
tail -f run.fifo | while read CMD; do
	echo "##start##$CMD####"
	timeout 10s bash -c "$CMD" &>cmd.output
	echo $? > cmd.ret
	[[ -e run.status ]] || mkfifo run.status
	if [[ "$(grep 124 cmd.ret)" = "124" ]]; then
		echo "sh: timedout" >cmd.output
		echo "timedout" > run.status
		echo "timedout"
	else
		[[ `stat --printf="%s" cmd.output` -eq 0 ]] &&
			echo "(no output)" >cmd.output
		echo "done" > run.status
		echo "done"
	fi
	echo -e "\e[00;36m`cat cmd.output`\e[00m"
	echo "##stop##$CMD####"
done


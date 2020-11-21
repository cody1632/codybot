# Codybot

20190824-20201120

## Overview

`codybot` is an IRC bot written with the C programming language, providing users
with fortune cookies, jokes, oneliner C compilation, shell access, text colorization,
ascii art, special characters and weather per city. It was inspired by candide on
Freenode, a great bot written in Perl (see https://github.com/pragma-/pbot/).

The bot can connect to `chat.freenode.net:6697` or any specified server and port;
it defaults to port 6697 with SSL.

## Compile and Run

In order to compile the source code into an executable, some dependencies are
required to be installed. On RPM-based systems, install `glibc-devel binutils
gcc make file-devel libopenssl-devel`. On DEB-based systems install `libc6-dev
binutils gcc make libmagic-dev libssl-dev`. On Arch install `make gcc`. 

To compile the program, just run `make` within the source directory, and run with `./codybot -n `_`YourBotNick`_.

(Personally I run the bot in a virtual machine to limit general filesystem access
via the `!sh` command; see the "chroot" and "docker" sections below.)

Run `./codybot --help` to see all program options.

There is no installation mechanism, you should use the source directory or move
the files where you want. The files `data-ascii.txt`, `data-chars.txt`, `data-fortunes.txt`,
`data-jokes.txt` and `stats` must be in the program's current working directory
otherwise the program will not run. Other control files (detailed below) must
also be in the current working directory.

Before running for the first time, you should also run  

    sudo chown -R root data-* Makefile codybot prog-* src

so something like `!sh rm importantfiles` won't be able to do any damage;
see https://github.com/cody1632/codybot/security/advisories.
(Instead of root you can also set the file owner to be any normal user _other than_ the codybot user.)

## Identifying

You should register and identify your bot nick with the IRC server's NickServ,
if it has one (not all IRC networks do). You can type `/msg NickServ help` in
your favorite connected IRC client (mIRC, irssi, weechat, hexchat, ...).
Otherwise you can identify to NickServ by typing `id `_`yourpasswordhere`_ or
`privmsg NickServ :identify `_`passhere`_ in the terminal running the codybot program.

Another console-only command you need to run is `join `_`#channel_name_here`_.
Note that bots are usually unwelcome on Freenode, especially in popular channels,
and uninvited bots are not permitted, so always get permission if you don't own
the channel. (If you created a channel by joining an empty one, you now own it,
so you can give yourself permission to run codybot.)

## Using the Bot

The bot responds to commands on its stdin (usually the terminal you run it from),
and in any channels it's joined. Commands in channels are only recognized if they
start with the command prefix, which defaults to exclamation point (`!`).
Do not include it when typing to its stdin. Some commands can only be used from stdin,
they are `exit`, `quit`, `curch`, `msg` and `id`.

In the following examples, "send _`!command`_" means send the command prefix and
_command_ as a message to a channel that the bot has joined. "type _command_" means
send _command_ to stdin of the running codybot process (usually by typing it into the terminal).
This differentiation may also be implied by the presence or absense of a leading prefix.

To see available commands, send `!help`.

To trigger a fortune cookie, send `!fortune`. The fortune cookie database file is `data-fortunes.txt`.
It's made of files in `/usr/share/games/fortunes` using the system-provided fortune package.
(see https://github.com/shlomif/fortune-mod)

To get a random joke send `!joke`. This database is hand written using https://www.funnyshortjokes.com
content and is far from containing all the site's jokes! There's 25 jokes as of 20200510.

To get weather report from https://wttr.in send `!weather `_`citynamehere`_; this should return
something like "_Montreal: Partly cloudy +2*C feels like -2*C 15km/h 0.0mm_"

To run a shell command from the chat onto the host of codybot, type `!sh `_`command and args`_
e.g. `!sh ls /home/codybot`. You can disable this by creating a file called `sh_disable`
(as always, in the program's current directory). You can also type `sh_enable` or `sh_disable`
in the console, or `!sh_enable` or `!sh_disable` in the channel.

## Running in a chroot

If you want to use the chroot mechanism, you have to download the minimal chroot archive
and extract it into the source directory. The latest chroot is available at  

* https://esselfe.ca/chroot.tar.xz  
or  
* https://esselfe.ca/chroot-aws.tar.xz  

To run all shell commands in a locked chroot, create a file called `sh_lock` or
type `!sh_lock` or `!sh_unlock`. You have to run as root:  

    chroot chroot /bin/bash  
    su - dummy  
    run.sh  

You must ensure that the codybot user can write to
`/home/dummy/run.fifo` inside the chroot. The files starting with
`cmd.` also need to be owned by the `dummy` user.
Note that the `dummy` user must exist with the same UID in both the host
and in the chroot.

## Running in Docker

Update 201002 - new docker image available!

You can now run codybot more safely by using a docker container.

Currently the image is 245MB download, 785MB installed, that's the downpart.
Install docker, make sure the services are started (containerd+docker),
and run those commands to fetch and start the image:

    docker pull esselfe/lunar-codybot
    docker run -it esselfe/lunar-codybot bash

Once in the container, run:

    exec su - codybot
    cd codybot
    ./codybot -f -n $NewNickHere -t '#'

Personally I made a 1GB partition on my host just for the bot and
its users' usage, then mount it (on the host) to `/mnt/codybot-data`,
then start the container this way instead:

    docker run -it -v /mnt/codybot-data:/home/user/codybot/tmp esselfe/lunar-codybot bash

To make `/home/user/tmp` the only possible location to write, run _inside_ the container:

    rm -rf /tmp
    ln -sv /home/user/codybot/tmp /tmp

Enjoy!

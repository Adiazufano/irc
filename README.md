*This project has been created as part of the 42 curriculum by aldiaz-u, danielji, and mparra-s*

# ft_irc

## Description

*Clearly presents the project, including its goal and a brief overview.*

## Instructions

*Contains any relevant information about compilation, installation, and/or execution.*

```sh
# Clone the repository:
$ git clone xxxxxx

# Compile:
$ make

# Run:
$ ./ircserv <port> <password>
# For example:
$ ./ircserv 6667 1234
```
### How to use HexChat

- Open HexChat, fill in the user information (**Nick name** and **User name**).
- Press **Add** to add a new network and press on the **Edit...** button.
- Change `newserver/6697` to the server's IP and port. (For example, `localhost/6667` or `127.0.0.1/6667`).
- Uncheck `Use SSL for all the servers on this network`.
- Set **Login method** to `Default` or `Server password (/PASS password)`.
- Close to apply the changes and click on **Connect**.

### How to use `nc`

You can use [Ncat](https://nmap.org/ncat/) or [Netcat](https://sectools.org/tool/netcat/) to connect to the IRC server. Run `nc` with `-C` option to use CRLF for end of line sequence. Specify the same host and port used in the server. For example:

```sh
nc -C localhost 6667
```

In order to register send the following commands in this order. Replace `<pass>` by the server's password and `<nick>` by any valid nickname of your choice.

```
CAP LS 302
PASS <pass>
NICK <nick>
USER <nick> 0 * :realname
CAP END
```

Upon registration you may now join to channels, send private messages, etc.

```
JOIN #mychannel
PRIVMSG #mychannel :Hello you all!
```

Type `Ctrl`+`C` to quit.

## Commands

### CAP

This command is used during user registration for capability negotiation between a server and a client.

```
CAP * LS 302

CAP END
```


### PASS

Set a connection password.

```
PASS 1234
```

### NICK

Give the client a nickname or change the previous one.

```
NICK homer89
```


### USER

The `USER` command is used at the beginning of a connection to specify the username and realname of a new user.

```
USER homer 0 * :Homer Simpson
```


### PING

The `PING` command is sent by either clients or servers to check the other side of the connection is still connected.

```
PING ...
```


### QUIT

Terminate a client’s connection to the server.

```
QUIT :Gone to evaluate someone's Libft
```


### JOIN

Join or create a channel. If the channel doesn't exist it will be created and the client will become a channel operator.

You may specify more than one channel.

Provide the optional key argument if the channel requires a password or the client wants to create a password-protected channel.

```
JOIN #born2code

JOIN #born2code 1234

JOIN #born2code,#42madrid 1234
```


### PART

Exit from the given channels.

```
PART #born2code

PART #born2code,#42madrid :Absorbed by the blackhole
```


### TOPIC

Change or view the topic of the given channel. If the protected topic mode is set on a channel, clients must have appropriate channel permissions to modify the topic of that channel.

```
TOPIC #catmemes

TOPIC #catmemes :Share your favorite cat memes
```


### NAMES

List the nicknames joined to a channel. Channel operators are prefixed with `@`.

```
NAMES #42madrid
```


### LIST

Get a list of channels along with some information about each channel.

```
LIST #catmemes,#born2code
```


### INVITE

Invite a user to a channel.

```
INVITE aldiaz-u #born2code
```


### KICK

Remove a user from a channel.

```
KICK #born2code mparra-s :Very noisy user
```


### PRIVMSG

Send private messages between users, and send messages to channels.

```
PRIVMSG rachel-g :How you doin?

PRIVMSG #born2code :Can someone please evaluate an Inception?
```


### NOTICE

Send notices between users, and send notices to channels.

> This command is very similar to `PRIVMSG` but it doesn't send automatic replies.

```
NOTICE ...

NOTICE ...
```

### WHO

Query a list of users from a channel.

```
WHO #42madrid
```

### MODE

Set (`+`) or unset (`-`) options from a channel.

- `+i`: Set channel to invite-only mode
- `+t`: Only channel operators can modify topic
- `+k`: Set a password to join channel
- `+l`: Set a user limit to a channel
- `+o`: Give operator privileges to a user

```
MODE #mychan

MODE #mychan +k 1234

MODE #mychan +l 100

MODE #mychan +ikl-t 1234 100
```

## (Additional sections)

*Additional sections may be required depending on the project (e.g., usage examples, feature list, technical choices, etc.).*

## Resources
<!-- Classic references related to the topic (documentation, articles, tutorials, etc.),
as well as a description of how AI was used —specifying for which tasks and which parts of the project. -->

- Jack Allnutt, Daniel Oaks, Val Lorentz: [Modern IRC Client Protocol](https://modern.ircdocs.horse/)
- [IRC Protocol Documentation](https://dd.ircdocs.horse/)
- The UChicago χ-Projects: [chirc](http://chi.cs.uchicago.edu/chirc/index.html)
- Chris Kanich: [The Linux socket API explained](https://www.youtube.com/watch?v=XXfdzwEsxFk)
- Brian “Beej Jorgensen” Hall: [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)

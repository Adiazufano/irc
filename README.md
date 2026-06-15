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

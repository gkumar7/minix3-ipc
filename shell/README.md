# Shell Implementation

#### Description
1. A PROFILE file is used by the shell to access the PATH and HOME environment variables. The user may also specify if the ALARM feature should be enabled.

2. Any programs may be executed which are defined in the /bin and /usr/bin directories or in other paths as specified by PATH.

3. Shell will set an alarm which fires 5 seconds after it has launched a command. After the alarm is fired, the user is asked if he/she wants to terminate the command and the shell will terminate the command if the user approves. The user can turn on and off this feature in the PROFILE file (ALARM).

4. Aliases for commands are supported. An alias may be defined as follows:
```sh
Alias listcontent=“ls”
```

5. If expressions in the following form are supported:
```sh
if command1; then command2; else command3 fi
```

#### Getting started
```sh
# Compile shell
clang main.c -o shell

# Run shell
./shell
```

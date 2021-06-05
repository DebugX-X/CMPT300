# Assignment 2
## Create a new shell named cshell
The OS command interpreter is the program that people interact with in order to launch and control
programs. On UNIX systems, the command interpreter is often called shell: a user-level program that
gives people a command-line interface to launching, suspending, and killing other programs. sh, ksh,
csh, tcsh, bash, ... are all examples of UNIX shells. You use a shell like this every time you log into a
Linux machine at a CS computer lab and bring up a terminal. It might be useful to look at the manual
pages of these shells, for example, type "man csh".
```sh
The most rudimentary shell is structured as the following loop:
Print out a prompt;
Read a line from the user;
Parse the line into the program name and an array of parameters;
Use the fork() system call to spawn a new child process;
The child process then uses the exec() system call (or one of its variants) to launch the specified
program;
The parent process (the shell) uses the wait() system call (or one of its variants) to wait for the child to
terminate;
Once the child (the launched program) finishes, the shell repeats the loop by jumping to 1.
Although most commands people type on the shell prompt are the names of other UNIX programs
(such as ps or cat), shells also recognize some special commands (called internal commands) that are
not program names. 
For example, the exit command terminates the shell, and the cd command changes
the current working directory. Shells directly make system calls to execute these commands, instead of
forking a child process to handle them.
```
### Requirements in detail:
Your job is to implement a very primitive shell that knows how to launch new programs in the
foreground and the background. It should also recognize a few internal commands. More specifically, it
should support the following features.
It should recognize the internal commands: exit, log, theme and print. 
- exit should use the `exit()` system call to terminate the shell. 
- log should print a list of entered commands.
- theme should change the color of output text
- print must print what was after print command and value of environment variable if any.

If the command line does not indicate any internal commands, it should be in the following form:
```sh
 <program name> /<arg1> /<arg2> .... /<argN>
```
  
Your shell should invoke the program, passing it the list of arguments in the command line. The shell
must wait until the started program completes unless the user runs it in the background.
To allow users to pass arguments you need to parse the input line into words separated by whitespace
(spaces and '\t' tab characters).

Your shell must allow for creation of environmental variables in the format `$varName=varValue`
  these enviromental values should be able print their value using the command `print $varName` and 
  update values if the format `$varName=newVarValue`
  
The shell must execute non built-in commands using two core system calls: fork() and execvp(). 
  In short, `fork()` creates an exact copy of the currently running process, and is used by
the shell to spawn a new process. The `execvp()` call is used to overload the currently running program
with a new program, which is how the shell turns a forked process into the program it wants to run. In
addition, the shell must wait until the previously started program completes unless the user runs it in
the background. This is done with the `wait()` system call or one of its variants (such as
`waitpid()`). All these system calls can fail due to unforeseen reasons.

You should check their return status and report errors if they occur.
No input the user gives should cause the shell to exit (except when the user types exit). This
means your shell should handle errors gracefully, no matter where they occur. Even if an error occurs in
the middle of a long pipeline, it should be reported accurately and your shell should recover gracefully.
In addition, your shell should not generate leaking open file descriptors. Hint: you can monitor the
current open file descriptors of the shell process through the /proc file system.
Your shell needs to support single pipes. Pipes allow the stdins and stdouts of a list of programs to be
concatenated in a chain. More specifically, the first program's stdout is directed to the stdin of the
second program; the second program's stdout is directed to the stdin of the third program; and so on so
forth.The `pipe()` and `dup2()` system calls will be useful.
Your compiled executable must be called cshell.

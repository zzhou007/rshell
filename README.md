# rshell
command shell written with cpp

## How to Build and Run
in rshell folder, type make to build and type /bin/rshell to run 

## Known Bugs
cd command does not work 

## Connectors 
Connectors are `&&` `||` and `;`.
They work form left to right.
<table>
	<tr>
    		<th>Connector</th>
	    	<th>Usage</th>
	</tr>
	<tr>
		<td>&&</td>
		<td>right of connector will run only if left succeds</td>
	</tr>
	</tr>
		<td>||</td>
		<td>right of connector will run only if left fails</td>
	</tr>
	<tr>
		<td>;</td>
		<td>right of connector will run no matter what</td>
	</tr>
</table>

## Quitting Program 
The program can be exited with the exit command.
It will work with connectors. 
If command has and characters trailing or leading it will not work. 

##Bugs
Program will always be in english.
Program does not tell you what commands you inputed when wrong.

##Edge
If nothing is put before the connector, if is assumed to be false.
|| [command] assuming command is a valid bash command, it will run. 
&& [command] command will not run 

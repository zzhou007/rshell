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


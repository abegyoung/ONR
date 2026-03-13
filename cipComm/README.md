# cipComm

Watches for a mux value change on gpio pin set by an incoming CSBF CIP command.

If there is a value change, the mux is read.

The command is translated to the appropriate server command and sent out with a log notation.


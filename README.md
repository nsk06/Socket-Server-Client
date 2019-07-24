##Client.c
- So in client.c for handling multiple clients we create connections in the while loop.
- Take input from the user in 'commands' buffer.
- First check for 'listall' command.
- Then check for 'send' command.
- Do required error handling for send commands as it involves files.

##Server.c
- Crete socket in while loop for handling multiple requests.
- Treat every client as seperate child
- First check for listall command.
- If present then take info of all files in a buffer and send to the client.
- Then check for send command(filename).
- Open the file(if possible) and copy its contents in a buffer and send to the client.
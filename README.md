# Network database
## Description
Server has local database with view like files with values by lines. Clients can connect to server by server's ip. Server will handle every connections, but it has just a limited number of threads for handling. Also server will close every client's connection due to the timeout setting in 30 seconds, if user will be no active.
## Clients can use commands like sql:
* Create
* Drop
* Truncate
* Select
* Insert
* Delete
* Update
/* $LastChangedDate: 2015-12-22 16:40:17 -0700 (Tue, 22 Dec 2015) $ */
/* $Rev: 117 $      $Author: peters $ */

#include <pthread.h>
#include "txServer.h"
#include "Commands.h"
#include "Logging.c"

int server_numClients;
int fd;
FILE *config; 			/* Configuration file */

sig_atomic_t pipe_deadflag = FALSE;

void *udpMonitor(void *arg);
void *pointing(void *arg);
void *hkMonitor(void *arg);

void pipe_handler (int signum) {
    pipe_deadflag = TRUE;
}


// ---------------------------------------------------------------- //
// print out help for ARG, or for all of the commands if no ARG     //
int Help (char *args) 
{
    register int i;
    int printed = 0;

    for (i = 0; commands [i].name; i++) 
      {
        if (!*args || (strcmp (args, commands [i].name) == 0)) 
	  {

            if (strlen (commands [i].name) < 8)
                TELLUSER ("%s\t\t%s.\n", commands [i].name, commands [i].doc)
            else
                TELLUSER ("%s\t%s.\n", commands [i].name, commands [i].doc)

            printed++;
	  }
      }
    if (!printed) 
      {
        TELLUSER ("No commands match `%s'.  Possibilties are:\n", args)

        for (i = 0; commands [i].name; i++) 
	  {
            // Print in six columns. //
            if (printed == 6) {

                printed = 0;
                TELLUSER ("\n")
	  }
        TELLUSER ("%s\t", commands[i].name)
	printed++;
      }

        if (printed)
            TELLUSER ("\n")
   }
   return 0;
}

/*
 * Look up NAME as the name of a command.
 * Return a pointer to that command, or a
 * NULL pointer if NAME is not found.
 */
COMMAND *find_command (char *name)
{
    register int i;

    for (i = 0; commands[i].name != NULL; i++)
        if (strcmp(name, commands[i].name) == 0)
            return (&commands[i]);

    return ((COMMAND *) NULL);
}

/*
 * Take a 'line', copy the first 'arg' into
 * a provided array, return the rest of it.
 */
char *one_argument (char *line, char *arg) {

    char *end;

    // skip over whitespace at the start //
    while (WHITESPACE (*line)) line++;

    // get the first argument off the front //
    while (!WHITESPACE (*line) && *line != '\0') {
        *arg = *line;
        line++;
        arg++;
    }
    *arg = '\0';

    // move to the next argument //
    while (WHITESPACE (*line)) line++;

    // remove any whitespace at the end //
    for (end = line + strlen (line) - 1; WHITESPACE (*end); end--);
    *(end + 1) = '\0';

    // return what's left (the 2nd+ arguments) //
    return line;
}

/*
 * Execute a command line.
 */
int execute_line (char *line)
{
    COMMAND      *command;
    char         arg [MAXLINE];

    // Isolate the command word. //
    line = one_argument (line, arg);
    // if the command doesn't match the list //
    if ((command = find_command (arg)) == NULL) 
      {
        TELLUSER ("%s: No such command!\n", arg)
        return _NO_ERROR; // not dead, just a bad arg
    }

    // Call the function. //
    return ((*(command->func)) (line));
}


// return: one request (char *)
//         NULL on error
char *read_request (int clientfd) {

    char *buffer = (char *) malloc (MAXLINE);
    int  n, i = 0;
    char c;

    assert (clientfd > 0);
    assert (buffer != NULL);

    do {
        n = read (clientfd, &c, 1);
        buffer [i++] = c;
    } while (n > 0  && c != '\n' && i < MAXLINE);

    if (n <= 0 || i <= 0) { 
        free (buffer);
        return NULL;
    }

    if (i == MAXLINE) i--;
    buffer [i] = 0;

    return buffer;
}



// return: int, 0 on success, 1 on failure
//         requests are " \0" terminated.
int process_client (int clientfd) {

  char *buffer;
  int retval = _NO_ERROR;

  assert (clientfd > 0);

  // get the request from the client //
  if ((buffer = read_request (clientfd)) == NULL)
    return _ERROR;

  ADDTOLOG ("(%d)->  %s", clientfd, buffer)
  retval = execute_line (buffer);

  free (buffer);
  return retval;
}



typedef struct clientFDList_ clientFDList;

struct clientFDList_ {
    clientFDList *next;
    int          clientFD;
    int          connected;
};

int manage_comm (int serverFD) {
    struct sockaddr_in cli_addr;
    int                clilen, newClientFD;
    fd_set             inFDSet;
    clientFDList       *topFDElement, *newFDElement,
                       *preFDElement, *curFDElement,
                       *nxtFDElement;
    struct timeval     delay;
    char outstr[256];

    // the client file descriptor list is empty
    topFDElement = NULL;

    // some preprocessing
    clilen = sizeof (cli_addr);

    // loop while alive
    while (!server_quit) {
        // clear the server and client incoming sets
        FD_ZERO (&inFDSet);

        // add the server to its set
        FD_SET (serverFD, &inFDSet);

        // add the clients to their set
        for (curFDElement = topFDElement;
             curFDElement != NULL;
             curFDElement = curFDElement->next)
            FD_SET (curFDElement->clientFD, &inFDSet);

        // set the timeout value
        delay.tv_sec  = DELAY_S;
        delay.tv_usec = DELAY_U;

        // wait for an event to occur, at 'delay' timeout
        if (select (FD_SETSIZE, &inFDSet, NULL, NULL, &delay)) {

            // a new client is connecting
            if (FD_ISSET (serverFD, &inFDSet) 
                &&  (newClientFD = accept (serverFD, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen)) > 0) {

                // if at max allowed number of clients, don't take another //
                if (server_numClients >= SERVER_MAXCLIENTS) {
                    server_curFD = newClientFD;

                    TELLUSER ("Currently at maximum allowed client load."
                              "Try back later.\nBye Bye!\n")
                    ADDTOLOG ("\t<Maximum load.. denied client.>\n")

                    close (newClientFD);

                } else {
                    newFDElement = (clientFDList *) malloc (sizeof (clientFDList));

                    newFDElement->next      = topFDElement;
                    newFDElement->clientFD  = newClientFD;
                    newFDElement->connected = TRUE;
                    topFDElement            = newFDElement;
                    // update number of clients //
                    server_numClients++;
                }
            }

            // client(s) have issued requests
            for (curFDElement = topFDElement;
                 curFDElement != NULL;
                 curFDElement = curFDElement->next)
                if (FD_ISSET (curFDElement->clientFD, &inFDSet)) {
                    // 1 if the client has died/disconnected
                    server_curFD = curFDElement->clientFD;
                    curFDElement->connected = !process_client (server_curFD);

                    // check if a SIGPIPE was raised, set connected false
                    if (pipe_deadflag) {
                        curFDElement->connected = FALSE;
                        pipe_deadflag           = FALSE;
                    }

                }
        }

        // free the dead clients from the client file descriptor list
        for (preFDElement  = NULL        , curFDElement = topFDElement;
             curFDElement != NULL;
             preFDElement  = curFDElement, curFDElement = nxtFDElement) {

            nxtFDElement = curFDElement->next;

            if (!curFDElement->connected) {

                if (preFDElement == NULL) topFDElement       = nxtFDElement;
                else                      preFDElement->next = nxtFDElement;

                close(curFDElement->clientFD);
                free (curFDElement);
		
		// If the top of the list was just deleted, set it to next one
		if (curFDElement == topFDElement) topFDElement = nxtFDElement;
                curFDElement = preFDElement;
                // update number of clients //
                server_numClients--;
            }
        }
    }

    return _NO_ERROR;
}


int main (int argc, char **argv) {

    int                sockfd, optval = 1;
    struct sockaddr_in serv_addr;
    struct sigaction   pipe_action;
    pthread_t udpThread, pointingThread, hkThread;
    pthread_attr_t attr;

    server_numClients=0;
    server_quit = FALSE;

    // Assign a signal handler for SIGPIPE, if a client dies.
    memset (&pipe_action, 0, sizeof (pipe_action));
    pipe_action.sa_handler = &pipe_handler;
    sigaction (SIGPIPE, &pipe_action, NULL);

    // Open a TCP socket (an Internet stream socket).
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0
    ||  setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR,
                    (void *) &optval, sizeof (int)) < 0) {
        ADDTOLOG ("Can't open stream socket.\n")
        return 1;
    }

    // Bind our local address so that the client can send to us.
    bzero ((char *) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    serv_addr.sin_port        = htons (SERV_PORT);

    if (bind (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        ADDTOLOG ("Can't bind local address.\n")
	printf("Can't bind to address\n");
        return 2;
    }

    // ask hardware for status of power supply bits
    server.powerstate=(unsigned char)i2c_read(DIO_ADDR, 0x01);
    server.freq = (float) 8650.0;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    /* Start the UDP thread */
    if (pthread_create(&udpThread, &attr, udpMonitor, NULL))
    {
      perror("pthread_create");
      exit(1);
    }

    /* Start the Pointing thread */
    if (pthread_create(&pointingThread, &attr, pointing, NULL))
    {
      perror("pthread_create");
      exit(1);
    }

    /* Start HK thread to keep system safe and provide telemetry data*/
    if (pthread_create(&hkThread, &attr, hkMonitor, NULL))
    {
      perror("pthread_create");
      exit(1);
    }

    ADDTOLOG ("Server up and running on TCP port %d!\n", SERV_PORT)

    listen      (sockfd, 5);
    manage_comm (sockfd);
    close       (sockfd);

    ADDTOLOG ("Server shutting down, watch out below!\n\n")
    return 0;
}

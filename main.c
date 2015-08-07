#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <tls.h>

//LICENSE: MIT
//Copyright: joseph@wfprod.com
//Maintainer: joseph@wfprod.com

//      ____          _                        _           _        _                                         
//     ||o o|     ___| |_ ___  _ __  _ __ ___ | |__   ___ | |_ __ _| |__  _   _ ___  ___   ___ ___  _ __ ___
//     ||===|    / __| __/ _ \| '_ \| '__/ _ \| '_ \ / _ \| __/ _` | '_ \| | | / __|/ _ \ / __/ _ \| '_ ` _ \
//   .-.`---'-.  \__ \ || (_) | |_) | | | (_) | |_) | (_) | || (_| | |_) | |_| \__ \  __/| (_| (_) | | | | | |
//   | | o .o |  |___/\__\___/| .__/|_|  \___/|_.__/ \___/ \__\__,_|_.__/ \__,_|___/\___(_)___\___/|_| |_| |_|
//   | | o:.o |               |_|                                                                             
//   | |      |
//   `-".-.-.-'    This code is officially robot abuse free.
//   _| | : |_
//  (oOoOo)_)_)

// Global Constants
#define MAX_BUFFER            (1000)

// Basic Error Handling
#define ECHOSERVER_ERROR(A)          fprintf(stderr, "ECHOSERVER: %s: %d (%s)\n", A, errno, strerror(errno)); exit(EXIT_FAILURE);
#define LIBTLS_NOCONTEXT_ERROR(A)    fprintf(stderr, "ECHOSERVER: LIBTLS: %s: %d (%s)\n", A, errno, strerror(errno)); exit(EXIT_FAILURE);
#define LIBTLS_CONTEXT_ERROR(A, B)   fprintf(stderr, "ECHOSERVER: LIBTLS: %s %s: %d (%s)\n", A, tls_error(B), errno, strerror(errno)); exit(EXIT_FAILURE);

int main(int argc, char *argv[]) {
    int       sock_listening;         // listening socket
    int       sock_connection;        // connection socket
    int       int_port;               // port number
    struct addrinfo hints, *res;      // socket stuff
    char      str_buffer[MAX_BUFFER]; // character buffer
    char     *ptr_end;                // for strtol()
	
    // Get port number from the command line
    if (argc == 2) {
		int_port = strtol(argv[1], &ptr_end, 0);
		if (*ptr_end) {
			ECHOSERVER_ERROR("Invalid port number");
		}
		
	// If no arguments were given, set to a default port
    } else if (argc < 2) {
		int_port = 1234;
		
    } else {
		ECHOSERVER_ERROR("Invalid arguments");
    }
	
    // Create the listening socket
    if ((sock_listening = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		ECHOSERVER_ERROR("socket() failed");
    }
	
    // Get the address info
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
	char str_port[20];
	sprintf(str_port, "%d", int_port);
    if (getaddrinfo(NULL, str_port, &hints, &res) != 0) {
		ECHOSERVER_ERROR("getaddrinfo() failed");
    }
	
    // Create the socket
    sock_listening = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock_listening == -1) {
		ECHOSERVER_ERROR("socket() failed");
    }
	
    // Enable the socket to reuse the address
    int reuseaddr = 1;                // True
    if (setsockopt(sock_listening, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1) {
		ECHOSERVER_ERROR("setsockopt() failed");
    }
	
    // Bind to the address
    if (bind(sock_listening, res->ai_addr, res->ai_addrlen) == -1) {
		ECHOSERVER_ERROR("bind() failed");
    }
	
    // Listen
    if (listen(sock_listening, 128) == -1) {
		ECHOSERVER_ERROR("listen() failed");
    }
	
	//// Libtls stuff here
	
	//HERE BE DRAGONS
	//Maintainer: joseph
	//@@@@@@@@@@@@@@@@@@@@@**^^""~~~"^@@^*@*@@**@@@@@@@@@
	//@@@@@@@@@@@@@*^^'"~   , - ' '; ,@@b. '  -e@@@@@@@@@
	//@@@@@@@@*^"~      . '     . ' ,@@@@(  e@*@@@@@@@@@@
	//@@@@@^~         .       .   ' @@@@@@, ~^@@@@@@@@@@@
	//@@@~ ,e**@@*e,  ,e**e, .    ' '@@@@@@e,  "*@@@@@'^@
	//@',e@@@@@@@@@@ e@@@@@@       ' '*@@@@@@    @@@'   0
	//@@@@@@@@@@@@@@@@@@@@@',e,     ;  ~^*^'    ;^~   ' 0
	//@@@@@@@@@@@@@@@^""^@@e@@@   .'           ,'   .'  @
	//@@@@@@@@@@@@@@'    '@@@@@ '         ,  ,e'  .    ;@
	//@@@@@@@@@@@@@' ,&&,  ^@*'     ,  .  i^"@e, ,e@e  @@
	//@@@@@@@@@@@@' ,@@@@,          ;  ,& !,,@@@e@@@@ e@@
	//@@@@@,~*@@*' ,@@@@@@e,   ',   e^~^@,   ~'@@@@@@,@@@
	//@@@@@@, ~" ,e@@@@@@@@@*e*@*  ,@e  @@""@e,,@@@@@@@@@
	//@@@@@@@@ee@@@@@@@@@@@@@@@" ,e@' ,e@' e@@@@@@@@@@@@@
	//@@@@@@@@@@@@@@@@@@@@@@@@" ,@" ,e@@e,,@@@@@@@@@@@@@@
	//@@@@@@@@@@@@@@@@@@@@@@@~ ,@@@,,0@@@@@@@@@@@@@@@@@@@
	//@@@@@@@@@@@@@@@@@@@@@@@@,,@@@@@@@@@@@@@@@@@@@@@@@@@
	//"""""""""""""""""""""""""""""""""""""""""""""""""""
	
	// Initialize libtls
	tls_init();
	
	// Initialize a configuration context
	struct tls_config *tls_sun_config = tls_config_new();
	if (tls_sun_config == NULL) {
		LIBTLS_NOCONTEXT_ERROR("tls_config_new() failed");
	}
	
	// Set the certificate file in the configuration context
	if (tls_config_set_cert_file(tls_sun_config, "server.crt") != 0) {
		LIBTLS_NOCONTEXT_ERROR("tls_config_set_cert_file() failed");
	}
	
	// Set the key file in the configuration context
	if (tls_config_set_key_file(tls_sun_config, "server.key") != 0) {
		LIBTLS_NOCONTEXT_ERROR("tls_config_set_key_file() failed");
	}
	
	// Set the allowed ciphers in the configuration context
	if (tls_config_set_ciphers(tls_sun_config, "EECDH+AESGCM:EDH+AESGCM:AES256+EECDH:AES256+EDH") != 0) {
		LIBTLS_NOCONTEXT_ERROR("tls_config_set_ciphers() failed");
	}
	
	// Create a server context
	struct tls *tls_sun_context = tls_server();
	if (tls_sun_context == NULL) {
		LIBTLS_CONTEXT_ERROR("tls_config_set_ciphers() failed", tls_sun_context);
	}
	
	// Attach the configuration context to the server context
	if (tls_configure(tls_sun_context, tls_sun_config) != 0) {
		LIBTLS_CONTEXT_ERROR("tls_configure() failed", tls_sun_context);
	}
	
    // Enter an infinite loop to respond to client requests and echo input
	struct tls *tls_sun_io_context;
    while (1) {
		// tls_accept_socket requires tls_sun_io_context to be set to null
		// HARK YE ONLOOKER: make sure you do this for every request,
		// if you don't set to null, the address from the previous request
		// will be sent to tls_accept_socket, and there will be an error
		tls_sun_io_context = NULL;
		
		// accept() waits for connection
		if ((sock_connection = accept(sock_listening, NULL, NULL)) < 0) {
			ECHOSERVER_ERROR("accept() failed");
		}
		
		// The tls accept command takes the socket you send and makes a tls I/O context out of it
		int int_status = tls_accept_socket(tls_sun_context, &tls_sun_io_context, sock_connection);
		if (int_status != 0) {
			LIBTLS_CONTEXT_ERROR("tls_accept_socket() failed", tls_sun_context);
		}
		
		// Read from the socket
		size_t int_out_length;
		if (tls_read(tls_sun_io_context, str_buffer, MAX_BUFFER - 1, &int_out_length) != 0) {
			LIBTLS_CONTEXT_ERROR("tls_read() failed", tls_sun_io_context);
		}
		
		// Write to the socket
		if (tls_write(tls_sun_io_context, str_buffer, int_out_length, &int_out_length) != 0) {
			LIBTLS_CONTEXT_ERROR("tls_write() failed", tls_sun_io_context);
		}
		
		// Close the tls I/O context
		if (tls_close(tls_sun_io_context) != 0) {
			LIBTLS_CONTEXT_ERROR("tls_close() failed", tls_sun_io_context);
		}
		
		//// HARK YE ONLOOKER: DO NOT UNCOMMENT LINES FOLLOWING
		// tls_close() HAS ALREADY CLOSED THE SOCKET, THESE LINES WILL ERROR!
		// Close the connection socket
		//if (close(sock_connection) != 0) {
		//	ECHOSERVER_ERROR("close() failed");
		//}
		
		// Free the tls I/O context
		tls_free(tls_sun_io_context);
    }
}

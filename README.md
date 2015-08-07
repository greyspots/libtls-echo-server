# libtls-echo-server
A simple Echo Server using the new library `libtls` from LibreSSL.

## Create ssl cert/key
In order to use the Echo Server, you must have a certificate and a key. If you already have one to test with, skip forward to the install section.

#### Check your openssl version
LibreSSL retains the command line `openssl` tool.
```
joseph@glock:~/Downloads/libtls-echo-server
$ openssl version
LibreSSL 2.2.2
```

#### Generate private key
You will need a pass phrase of four characters. The pass phrase will be removed later.
```
joseph@glock:~/Downloads/libtls-echo-server
$ openssl genrsa -des3 -out server.key 1024
Generating RSA private key, 1024 bit long modulus
..........++++++
...............++++++
e is 65537 (0x10001)
Enter pass phrase for server.key:
Verifying - Enter pass phrase for server.key:
```

#### Create csr file out of the private key
Enter in the pass phrase.
Enter in your company info. (If you don't have a company, fake one. It's just a echo server.)
Leave the challenge password blank.
Make sure the you enter the `*.` part of your domain name.
```
joseph@glock:~/Downloads/libtls-echo-server
$ openssl req -new -key server.key -out server.csr
Enter pass phrase for server.key:
You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank.
-----
Country Name (2 letter code) []:US
State or Province Name (full name) []:Texas
Locality Name (eg, city) []:North Richland Hills
Organization Name (eg, company) []:Greyspots
Organizational Unit Name (eg, section) []:IT    
Common Name (eg, fully qualified host name) []:*.envelope.xyz
Email Address []:indeed817@gmail.com

Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:
```

#### Create the organization file
```
joseph@glock:~/Downloads/libtls-echo-server
$ cp server.key server.org
```

#### Create certificate file
You will need to enter the pass phrase again.
```
joseph@glock:~/Downloads/libtls-echo-server
$ openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt
Signature ok
subject=/C=US/ST=Texas/L=North Richland Hills/O=Greyspots/OU=IT/CN=*.envelope.xyz/emailAddress=indeed817@gmail.com
Getting Private key
Enter pass phrase for server.key:
```

#### Remove pass phrase
You will need to enter the pass phrase one last time.
```
joseph@glock:~/Downloads/libtls-echo-server
$ openssl rsa -in server.org -out server.key
Enter pass phrase for server.org:
writing RSA key
```

#### Adjust permissions on your files
```
joseph@glock:~/Downloads
$ chmod 600 server.*

joseph@glock:~/Downloads
$ chmod 400 server.key
```
You are now have the certificate files to test with.

## Installation
#### Compile
Run `make` to compile.
```
joseph@glock:~/Downloads/libtls-echo-server
$ make
gcc -o main.o main.c -c -pedantic -Wall
gcc -o echoserver main.o -Wall -l tls
```

#### Usage
Run the binary.
```
joseph@glock:~/Downloads/libtls-echo-server
$ ./echoserver &
```

#### Test
You can use `curl` to test.
The `-k` flag turns off certificate verification. If you are using a verified certificate you can remove that flag.
Notice the echo server returns the entire request back to curl.
```
joseph@glock:~/Downloads/libtls-echo-server
$ curl -k https://127.0.0.1:1234/index.html
GET /index.html HTTP/1.1
User-Agent: curl/7.37.1
Host: 127.0.0.1:1234
Accept: */*
```
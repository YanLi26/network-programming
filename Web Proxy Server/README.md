#Web Proxy
##Run
###proxy
1. gcc webproxy.c -lssl -lcrypto -o webproxy
2. ./webproxy <prot> <timeout seconds>

###client
1. telnet localhost <port>
2. GET URL HTTP/1.0

##Implementation
1. get request to get page from server
2. encryption file Name
3. test timeout
4. multithread
5. check if the page is in proxy or not. If it is not in proxy, send get request or return page to client directly

##Test
Message: GET http://www.google.com/ HTTP/1.0

Requst type is: GET
Requst page is: /
Requst HTTP version is: HTTP/1.0
Requst host is: www.google.com
File name with MD5 : e8e7327d181704b5c49d51e5cc10b313
216.58.217.36

HTTP/1.0 200 OK
Date: Sun, 11 Dec 2016 02:24:52 GMT
Expires: -1
Cache-Control: private, max-age=0
Content-Type: text/html; charset=ISO-8859-1
P3P: CP="This is not a P3P policy! See https://www.google.com/support/accounts/answer/151657?hl=en for more iwww.google.comnfo."
Server: gws
X-XSS-Protection: 1; mode=block
X-Frame-Options: SAMEORIGIN
Set-Cookie: NID=91=ge9la0mJTXZEF06EUWsP3wPxZVJBv5TXyf5D4xRxNOGHj8v8T8KwPNJO5lFPEsPP0lF9OlXyN_8mfoqNnMTZcNbJ8gssLR4gjAbR4cQh_LZtkGKU0KpA7Nrixhs85wXOzadAwNKg3P62mw; expires=Monwww.google.com, 12-Jun-2017 02:24:52 GMT; path=/; domain=.google.com; HttpOnly
Accept-Ranges: none
Vary: Accept-Encoding

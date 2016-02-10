import BaseHTTPServer
import CGIHTTPServer

import cgitb; cgitb.enable()

SERVER_PORT = 3636

def runServer():
  server = BaseHTTPServer.HTTPServer
  handler = CGIHTTPServer.CGIHTTPRequestHandler
  server_address = ("", SERVER_PORT)
  handler.cgi_directories = ["/cgi"]
   
  httpd = server(server_address, handler)
  httpd.serve_forever()
#coding=utf-8
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import socket
import json

HOST_IP = "127.0.0.1"
PORT = 2333
HTTP_PORT = 8888
SIMPLE_TOKEN = 'abc'
class Singleton(object):
    _instance = None
    def __new__(class_, *args, **kwargs):
        if not isinstance(class_._instance, class_):
            class_._instance = object.__new__(class_, *args, **kwargs)
        return class_._instance

class HtmlContentProvider(Singleton):
    def __init__(self):
        fd = open('static/index.html')
        self.m_html = fd.read()
        fd.close()
        

class MyHandler(BaseHTTPRequestHandler):


    def jsonCheck(self, json_str):
        try:
            data = json.loads(json_str)
            clientToken = data['token']
            if clientToken == SIMPLE_TOKEN:
                return 0
            else:
                return 3
        except Exception, e:
            print Exception, '----------', e
            return 2

    def do_GET(self):
        # body = self.rfile.read(int(self.headers['content-length']))
        # so = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # so.connect((HOST_IP, PORT))
        # so.sendall(body)
        # response_body = so.recv(8096)
        if (self.path) == '/config':
            self.send_response(200)
            self.send_header('Content-type','text/html')
            self.end_headers()
            so = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            so.connect((HOST_IP, PORT))
            so.sendall('{"opt":1}')
            response_body = so.recv(8096)
            self.wfile.write(response_body)
            return

        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.end_headers()
        send_main = True
        if send_main:
            self.wfile.write(HtmlContentProvider().m_html)
        else:
            self.wfile.write('{"error_code":1}')
        return

    def do_POST(self):
        self.send_response(200)
        self.send_header('Content-type','application/json')
        self.end_headers()
        body = self.rfile.read(int(self.headers['content-length']))
        if not body:
            self.wfile.write('{"error_code":1}')

        error_code = self.jsonCheck(body)
        if (error_code > 0):
            self.wfile.write('{"error_code":%d}'%error_code)

        so = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        so.connect((HOST_IP, PORT))
        so.sendall(body)
        response_body = so.recv(80960)
        
        self.wfile.write(response_body)

        return


def main():
    try:
        server = HTTPServer(('', HTTP_PORT), MyHandler)
        print 'Welcome to the machine...'
        server.serve_forever()
    except KeyboardInterrupt:
        print '^C received, shutting down server'
        server.socket.close()

if __name__ == '__main__':
    main()



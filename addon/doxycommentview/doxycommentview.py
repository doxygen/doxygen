#!/usr/bin/env python3
#
# python3 helper script to start a web server that can do life rendering of doxygen comments.
#
# Copyright (C) 1997-2024 by Dimitri van Heesch.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation under the terms of the GNU General Public License is hereby
# granted. No representations are made about the suitability of this software
# for any purpose. It is provided "as is" without express or implied warranty.
# See the GNU General Public License for more details.
#
# Documents produced by Doxygen are derivative works derived from the
# input used in their production; they are not affected by this license.
#

import http.server
import socketserver
import json
import subprocess
import argparse
import signal
import threading
import html

def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description="Runs the doxygen comment viewer HTTP server.")
    parser.add_argument('--port', type=int, default=8000, help='Port number to run the server on')
    parser.add_argument('--doxygen', type=str, default='doxygen', help='Path to doxygen executable')
    parser.add_argument('--doxyfile', type=str, default='Doxyfile', help='Path to Doxyfile to use')
    args = parser.parse_args()

    PORT = args.port
    DOXYGEN = args.doxygen
    DOXYFILE = args.doxyfile
    VERSION_STR = subprocess.run([DOXYGEN, '-v'], capture_output=True, text=True, encoding="utf-8").stdout

    class RequestHandler(http.server.SimpleHTTPRequestHandler):
        def do_POST(self):
            if self.path == '/process':
                content_length = int(self.headers['Content-Length'])
                post_data = self.rfile.read(content_length)
                data = json.loads(post_data)
                input_text = data['input']

                # Run doxygen in single comment mode, reading from stdin and writing to stdout and stderr
                result = subprocess.run([DOXYGEN, '-c', '-', DOXYFILE], \
                                        input=input_text, capture_output=True, text=True, encoding="utf-8")

                # Prepare the response
                response = json.dumps({
                  'html_output': result.stdout,
                  'error_output': "<b>Doxygen version "+html.escape(VERSION_STR)+"</b><pre>"+html.escape(result.stderr)+"</pre>"
                })

                # Send the result to the requesting HTML page
                self.send_response(200)
                self.send_header('Content-type', 'text/html')
                self.end_headers()
                self.wfile.write(response.encode())

    httpd = socketserver.TCPServer(("", PORT), RequestHandler)

    def signal_handler(sig, frame):
        print('Shutting down the web server...')
        threading.Thread(target=httpd.shutdown).start()

    signal.signal(signal.SIGINT, signal_handler)

    print("Running web server on port", PORT)
    httpd.serve_forever()

if __name__ == '__main__':
    main()

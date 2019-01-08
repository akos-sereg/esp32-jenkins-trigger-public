#!/bin/bash

openssl s_client -showcerts -connect asereghelpdesk.ngrok.io:443 </dev/null > server_root_cert.pem
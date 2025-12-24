#!/bin/sh
set -e

ln -s /mnt/usr/local/bin/* /usr/local/bin/

exec "$@"

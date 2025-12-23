#!/bin/bash

while getopts "bx" option; do
    case "${option}" in
        b)
            docker-compose up --build -d
            ;;
        x)
            docker-compose exec app /bin/bash
            ;;
        *)
            echo "Usage: $0 -b | -x"
            exit 1
            ;;
    esac
done

if [ "$OPTIND" -eq 1 ]; then
    echo "Usage: $0 -b | -x"
    exit 1
fi
set -eo pipefail

cpplint --extensions=ino src/* lib/Config/* lib/HttpLibrary/* lib/Output/*


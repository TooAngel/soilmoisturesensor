#!/bin/bash
set -eo pipefail

cpplint --filter=-build/include_what_you_use --extensions=ino,cpp,h src/* lib/Config/* lib/HttpLibrary/*

#!/bin/bash
# Wrapper script for running PanDelos-plus with docker run
# Usage: ./run-docker.sh -i input/file.pdi -o output/result

docker run --rm \
  -v "$(pwd)/input:/home/pdp/PanDelos-plus/input" \
  -v "$(pwd)/output:/home/pdp/PanDelos-plus/output" \
  pandelosplus "$@"

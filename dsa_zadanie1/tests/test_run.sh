#!/usr/bin/env bash

bin/zadanie1 &
EXEC_PID=$!

sleep 2
kill $EXEC_PID

# Optional: Wait for process to fully terminate
wait $EXEC_PID 2>/dev/null
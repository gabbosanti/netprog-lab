#!/bin/bash

# Function to print usage
usage() {
    echo "Usage: $0 [--all] [--128] [--160] [--192] [--224] [--256] [--rtt] [--ppt] [--deq] [--no-encryption] [--tls] [--noterminals]"
    echo "Options:"
    echo "  --all               Run all configurations"
    echo "  --no-encryption     Run test without encryption functionality"
    echo "  --tls               Run test with Modbus using TLS encryption"
    echo "  --128               Run 128-bit configuration"
    echo "  --160               Run 160-bit configuration"
    echo "  --192               Run 192-bit configuration"
    echo "  --224               Run 224-bit configuration"
    echo "  --256               Run 256-bit configuration"
    echo "Measurement options:"
    echo "  --rtt               Measure Round Trip Time"
    echo "  --ppt               Measure Packet Processing Time"
    echo "  --deq               Measure Packet Dequeuing Timedelta"
    echo "Additional option:"
    echo "  --noterminals       Pass --noterminals flag to kathara lstart"
    echo "Note: If no measurement options are selected, all measurements will be performed"
    exit 1
}


# EXTENSIBILITY 
# Set the command to execute the client
default_client="python modbus_client.py"
# Set the command to execute the server
default_server="python server.py"

export client="$default_client"
export server="$default_server"

# KEYS=( "128" "160" "192" "224" "256" )

# Function to be executed on cleanup
cleanup() {
    echo "Performing cleanup..."
    # Add your cleanup tasks here
    kathara lclean
    rm -f shared/startup.temp
    exit
}

# Function to check if a container is ready
wait_for_container() {
    local container=$1
    local max_attempts=30
    local attempt=1
    local delay=2

    echo "Waiting for $container to be ready..."
    
    while [ $attempt -le $max_attempts ]; do
        # Check if startup is complete by looking for a marker in shared/startup.temp
        if grep -q "$container" shared/startup.temp 2&>/dev/null; then
            echo "$container is ready!"
            return 0
        fi
        
        echo "Attempt $attempt/$max_attempts: $container not ready yet, waiting..."
        sleep $delay
        attempt=$((attempt + 1))
    done

    echo "Timeout waiting for $container to be ready"
    return 1
}

wait_for_modbus_service() {
    local max_attempts=20
    local attempt=1
    local delay=1

    echo "Waiting for Modbus service (200.1.1.7:502) to be reachable..."

    while [ $attempt -le $max_attempts ]; do
        if kathara exec modbusclient "python -c 'import socket,sys; s=socket.socket(); s.settimeout(1); rc=s.connect_ex((\"200.1.1.7\",502)); s.close(); sys.exit(0 if rc==0 else 1)'" >/dev/null 2>&1; then
            echo "Modbus service is reachable!"
            return 0
        fi

        echo "Attempt $attempt/$max_attempts: Modbus service not reachable yet, waiting..."
        sleep $delay
        attempt=$((attempt + 1))
    done

    echo "Timeout waiting for Modbus service"
    return 1
}

# Initialize all flags to 0
ALL=0
NO_ENCRYPTION=0
TLS=0
BIT_128=0
BIT_160=0
BIT_192=0
BIT_224=0
BIT_256=0
MEASURE_RTT=0
MEASURE_PPT=0
MEASURE_DEQ=0

# Parse command line arguments (added --noterminals in the getopt list)
TEMP=$(getopt -o '' --long all,no-encryption,tls,128,160,192,224,256,rtt,ppt,deq,noterminals -n "$0" -- "$@")
if [ $? -ne 0 ]; then
    usage
fi


eval set -- "$TEMP"

# Extract options
# Extract options
while true; do
    case "$1" in
        --all)
            ALL=1
            shift
            ;;
        --no-encryption)
            NO_ENCRYPTION=1
            shift
            ;;
        --tls)
            TLS=1
            shift
            ;;
        --128)
            BIT_128=1
            shift
            ;;
        --160)
            BIT_160=1
            shift
            ;;
        --192)
            BIT_192=1
            shift
            ;;
        --224)
            BIT_224=1
            shift
            ;;
        --256)
            BIT_256=1
            shift
            ;;
        --rtt)
            MEASURE_RTT=1
            shift
            ;;
        --ppt)
            MEASURE_PPT=1
            shift
            ;;
        --deq)
            MEASURE_DEQ=1
            shift
            ;;
        --noterminals)
            KATHARA_OPTIONS="--noterminals"
            shift
            ;;
        --)
            shift
            break
            ;;
        *)
            usage
            ;;
    esac
done

# If --all is specified, set all configuration flags to 1
if [ $ALL -eq 1 ]; then
    NO_ENCRYPTION=1
    TLS=1
    BIT_128=1
    BIT_160=1
    BIT_192=1
    BIT_224=1
    BIT_256=1
fi

# Check if at least one configuration option was selected
if [ $ALL -eq 0 ] && [ $NO_ENCRYPTION -eq 0 ] && [ $TLS -eq 0 ] && [ $BIT_128 -eq 0 ] && [ $BIT_160 -eq 0 ] && [ $BIT_192 -eq 0 ] && [ $BIT_224 -eq 0 ] && [ $BIT_256 -eq 0 ]; then
    echo "Error: At least one configuration option must be selected"
    usage
fi

# If no measurement options are selected, enable all measurements
if [ $MEASURE_RTT -eq 0 ] && [ $MEASURE_PPT -eq 0 ] && [ $MEASURE_DEQ -eq 0 ]; then
    echo "No specific measurements selected. Enabling all measurements..."
    MEASURE_RTT=1
    MEASURE_PPT=1
    MEASURE_DEQ=1
fi

# function to print the mode
detect_mode(){
    if [ "$MODE" = "no-encryption" ] || [ "$MODE" = "tls" ]; then
        CUR_MODE="$MODE"
    else CUR_MODE="$MODE"-bit
    fi
}


measure_ppt(){
    echo "Measuring Packet Processing Time for $CUR_MODE ($1) configuration"
    if [ "$1" = "write" ]; then
        # retrieve info from the switches
        #s1
        echo "Retreiving info from s1"
        if ! kathara exec s1 "./retrieve_info.sh --ppt ${MODE} -w" >/dev/tty 2>&1; then
            echo "Error: PPT info retrieve failed in s1"
            exit
        fi

        #s2
        echo "Retreiving info from s2"
        if ! kathara exec s2 "./retrieve_info.sh --ppt ${MODE} -w" >/dev/tty 2>&1; then
            echo "Error: PPT info retrieve failed in s2"
            exit
        fi
    fi

    if [ "$1" = "read" ]; then
        # retrieve info from the switches
        #s1
        echo "Retreiving info from s1"
        if ! kathara exec s1 "./retrieve_info.sh --ppt ${MODE} -r" >/dev/tty 2>&1; then
            echo "Error: PPT info retrieve failed in s1"
            exit
        fi

        #s2
        echo "Retreiving info from s2"
        if ! kathara exec s2 "./retrieve_info.sh --ppt ${MODE} -r" >/dev/tty 2>&1; then
            echo "Error: PPT info retrieve failed in s2"
            exit
        fi  
    fi
}

measure_deq(){
    echo "Measuring Packet Dequeuing Timedelta for $CUR_MODE ($1) configuration"
    if [ "$1" = "write" ]; then
        # retrieve info from the switches
        #s1
        echo "Retreiving info from s1"
        if ! kathara exec s1 "./retrieve_info.sh --deq ${MODE} -w" >/dev/tty 2>&1; then
            echo "Error: PPT info retrieve failed in s1"
            exit
        fi

        #s2
        echo "Retreiving info from s2"
        if ! kathara exec s2 "./retrieve_info.sh --deq ${MODE} -w" >/dev/tty 2>&1; then
            echo "Error: PPT info retrieve failed in s2"
            exit
        fi
    fi

    if [ "$1" = "read" ]; then
        # retrieve info from the switches
        #s1
        echo "Retreiving info from s1"
        if ! kathara exec s1 "./retrieve_info.sh --deq ${MODE} -r" >/dev/tty 2>&1; then
            echo "Error: PPT info retrieve failed in s1"
            exit
        fi

        #s2
        echo "Retreiving info from s2"
        if ! kathara exec s2 "./retrieve_info.sh --deq ${MODE} -r" >/dev/tty 2>&1; then
            echo "Error: PPT info retrieve failed in s2"
            exit
        fi  
    fi
}

run_configuration_mode(){ #@note run_configuration_mode
    if [ ! "$1" = "write" ] && [ ! "$1" = "read" ]; then
        exit 1
    fi

    # Clean any previous Kathara instance
    kathara lclean
    
    # Start Kathara with specific configuration
    if ! kathara lstart $KATHARA_OPTIONS
    then
        echo "Error: Failed to start Kathara for $CUR_MODE ($1) configuration"
        kathara lclean
        return 1
    fi

    echo "Kathara started successfully for $CUR_MODE ($1) configuration!"
    
    # Wait for containers to be ready
    if ! wait_for_container "modbusclient"; then
        echo "Error: modbusclient failed to start properly"
        kathara lclean
        return 1
    fi

    if ! wait_for_container "modbusserver"; then
        echo "Error: modbusserver failed to start properly"
        kathara lclean
        return 1
    fi

    if ! wait_for_container "s1"; then
        echo "Error: s1 failed to start properly"
        kathara lclean
        return 1
    fi

    if ! wait_for_container "s2"; then
        echo "Error: s2 failed to start properly"
        kathara lclean
        return 1
    fi

    if ! wait_for_modbus_service; then
        echo "Error: Modbus service is not reachable"
        kathara lclean
        return 1
    fi

    echo "Processing $CUR_MODE ($1) configuration..."

    # RTT - Round Trip Time
    if [ $MEASURE_RTT -eq 1 ]; then
        echo "Measuring Round Trip Time..."

        echo "Starting client in modbusclient..."
        [[ "$MODE" = "tls" ]] && client_mode="" || client_mode=$MODE
        if ! kathara exec modbusclient "$client --test-rtt-$1 $client_mode" >/dev/tty 2>&1; then
            echo "Error: RTT measurement failed"
            exit
        fi

        if [ $MEASURE_PPT -eq 1 ]; then
            measure_ppt "$1"
        fi

        if [ $MEASURE_DEQ -eq 1 ]; then
            measure_deq "$1"
        fi
    fi

    # PPT - Packet Prcessing Time
    if [ $MEASURE_PPT -eq 1 ] && [ $MEASURE_RTT -ne 1 ]; then
        echo "Measuring Packet Processing Time..."

        #modbusclient
        echo "Starting client in modbusclient..."
        [[ "$MODE" = "tls" ]] && client_mode="" || client_mode=$MODE
        if ! kathara exec modbusclient "$client --test-$1 $client_mode" >/dev/tty 2>&1; then
            echo "Error: PPT measurement failed | error in modbus client on modbusclient"
            exit
        fi
        

        measure_ppt "$1"

        if [ $MEASURE_DEQ -eq 1 ]; then
            measure_deq "$1"
        fi
    fi

    # DEQ - Packet Dequeuing Timedelta
    if [ $MEASURE_DEQ -eq 1 ] && [ $MEASURE_RTT -ne 1 ] && [ $MEASURE_PPT -ne 1 ]; then
        echo "Measuring Dequeuing Timedelta..."

        #modbusclient
        echo "Starting client in modbusclient..."
        [[ "$MODE" = "tls" ]] && client_mode="" || client_mode=$MODE
        if ! kathara exec modbusclient "$client --test-$1 $client_mode" >/dev/tty 2>&1; then
            echo "Error: PPT measurement failed | error in modbus client on modbusclient"
            kathara lclean
        fi

        measure_deq "$1" 
    fi
    
    # After processing, clean up
    echo "Cleaning up $CUR_MODE ($1) configuration..."
    kathara lclean
    rm -f shared/startup.temp
} ## run_configuration_mode


# Function to run a specific configuration
run_configuration() { #@note run_configuration
    local MODE=$1

    if [ "$MODE" = "tls" ];then 
        export client="python tls_client.py"
        export server="python server_tls.py"
    fi

    detect_mode

    echo "Starting Kathara for $CUR_MODE configuration..."

    # create commands files with basic commands
    echo "" > s1/auto_test_commands.txt
    echo "" > s2/auto_test_commands.txt
    echo "table_set_default ipv4_lpm drop
table_add ipv4_lpm ipv4_forward 200.1.1.7/32 => 00:00:00:00:00:03 2
table_add ipv4_lpm ipv4_forward 200.1.1.8/32 => 00:00:00:00:00:03 2
table_add ipv4_lpm ipv4_forward 200.1.1.9/32 => 00:00:00:00:00:03 2
table_add ipv4_lpm ipv4_forward 195.11.14.5/32 =>  00:00:0a:00:01:01 1
table_add ipv4_lpm ipv4_forward 195.11.14.6/32 =>  00:00:0a:00:02:01 1
table_add ipv4_lpm ipv4_forward 195.11.14.7/32 =>  00:00:0a:00:02:01 1" >> s1/auto_test_commands.txt
    echo "table_set_default ipv4_lpm drop
table_add ipv4_lpm ipv4_forward 195.11.14.5/32 =>  00:00:00:00:00:02 2
table_add ipv4_lpm ipv4_forward 195.11.14.6/32 =>  00:00:00:00:00:02 2
table_add ipv4_lpm ipv4_forward 195.11.14.7/32 =>  00:00:00:00:00:02 2
table_add ipv4_lpm ipv4_forward 200.1.1.7/32 =>  00:00:0a:00:01:02 1
table_add ipv4_lpm ipv4_forward 200.1.1.8/32 =>  00:00:0a:00:02:02 1
table_add ipv4_lpm ipv4_forward 200.1.1.9/32 =>  00:00:0a:00:02:02 1" >> s2/auto_test_commands.txt

    # append rule in auto_test_commands.txt in s1 and s2 according to the current mode
    case $MODE in
        no-encryption|tls)
            # do nothing
        ;;

        128)
            # set the register from 4 up to 7 to 0 in commands.txt in s1
            echo "register_write keys 0 729683222
register_write keys 1 682545830
register_write keys 2 2885096840
register_write keys 3 164581180
register_write keys 4 0
register_write keys 5 0
register_write keys 6 0
register_write keys 7 0
table_add modbus_sec decipher 1 =>
table_add modbus_sec cipher 2 =>
EOF" >> s1/auto_test_commands.txt

            echo "register_write keys 0 729683222
register_write keys 1 682545830
register_write keys 2 2885096840
register_write keys 3 164581180
register_write keys 4 0
register_write keys 5 0
register_write keys 6 0
register_write keys 7 0
table_add modbus_sec decipher 1 =>
table_add modbus_sec cipher 2 =>
EOF" >> s2/auto_test_commands.txt
        ;;

        160)
            echo "register_write keys 0 729683222
register_write keys 1 682545830
register_write keys 2 2885096840
register_write keys 3 164581180
register_write keys 4 102358694
register_write keys 5 0
register_write keys 6 0
register_write keys 7 0
table_add modbus_sec decipher 1 =>
table_add modbus_sec cipher 2 =>
EOF" >> s1/auto_test_commands.txt
            echo "register_write keys 0 729683222
register_write keys 1 682545830
register_write keys 2 2885096840
register_write keys 3 164581180
register_write keys 4 102358694
register_write keys 5 0
register_write keys 6 0
register_write keys 7 0
table_add modbus_sec decipher 1 =>
table_add modbus_sec cipher 2 =>
EOF" >> s2/auto_test_commands.txt
        ;;

        192)
            echo "register_write keys 0 729683222
register_write keys 1 682545830
register_write keys 2 2885096840
register_write keys 3 164581180
register_write keys 4 102358694
register_write keys 5 259174683
register_write keys 6 0
register_write keys 7 0
table_add modbus_sec decipher 1 =>
table_add modbus_sec cipher 2 =>
EOF" >> s1/auto_test_commands.txt
            echo "register_write keys 0 729683222
register_write keys 1 682545830
register_write keys 2 2885096840
register_write keys 3 164581180
register_write keys 4 102358694
register_write keys 5 259174683
register_write keys 6 0
register_write keys 7 0
table_add modbus_sec decipher 1 =>
table_add modbus_sec cipher 2 =>
EOF" >> s2/auto_test_commands.txt


        ;;

        224)
            echo "register_write keys 0 729683222
register_write keys 1 682545830
register_write keys 2 2885096840
register_write keys 3 164581180
register_write keys 4 102358694
register_write keys 5 259174683
register_write keys 6 243695780
register_write keys 7 0
table_add modbus_sec decipher 1 =>
table_add modbus_sec cipher 2 =>
EOF" >> s1/auto_test_commands.txt
            echo "register_write keys 0 729683222
register_write keys 1 682545830
register_write keys 2 2885096840
register_write keys 3 164581180
register_write keys 4 102358694
register_write keys 5 259174683
register_write keys 6 243695780
register_write keys 7 0
table_add modbus_sec decipher 1 =>
table_add modbus_sec cipher 2 =>
EOF" >> s2/auto_test_commands.txt
        ;;

        256)
            echo "register_write keys 0 729683222
register_write keys 1 682545830
register_write keys 2 2885096840
register_write keys 3 164581180
register_write keys 4 102358694
register_write keys 5 259174683
register_write keys 6 243695780
register_write keys 7 096548217
table_add modbus_sec decipher 1 =>
table_add modbus_sec cipher 2 =>
EOF" >> s1/auto_test_commands.txt
            echo "register_write keys 0 729683222
register_write keys 1 682545830
register_write keys 2 2885096840
register_write keys 3 164581180
register_write keys 4 102358694
register_write keys 5 259174683
register_write keys 6 243695780
register_write keys 7 096548217
table_add modbus_sec decipher 1 =>
table_add modbus_sec cipher 2 =>
EOF" >> s2/auto_test_commands.txt

        ;;

        *)
            # The wrong first argument.
            echo 'Expected "no-encryption", "tls", "128", "160", "192", "224" or "256"' >&2
            exit 1
    esac
    
    run_configuration_mode "write"
    run_configuration_mode "read"

    # restore the default client and server
    if [ "$MODE" = "tls" ];then 
        export client="$default_client"
        export server="$default_server"
    fi
} ## run_configuration()


# Set up trap for both SIGINT and EXIT
#trap cleanup SIGINT EXIT

# Run kathara wipe just to be sure all machines are brand new
kathara wipe -f

# Process each selected configuration
if [ $NO_ENCRYPTION -eq 1 ]; then
    run_configuration "no-encryption"
fi

if [ $TLS -eq 1 ]; then
    run_configuration "tls"
fi

if [ $BIT_128 -eq 1 ]; then
    run_configuration "128"
fi

if [ $BIT_160 -eq 1 ]; then
    run_configuration "160"
fi

if [ $BIT_192 -eq 1 ]; then
    run_configuration "192"
fi

if [ $BIT_224 -eq 1 ]; then
    run_configuration "224"
fi

if [ $BIT_256 -eq 1 ]; then
    run_configuration "256"
fi

rm -f s1/auto_test_commands.txt
rm -f s2/auto_test_commands.txt

# move the results to the appropriate folders
[[ $TLS -eq 1 ]] && mv -f shared/*tls* results/mul_key/modbus_tls
[[ $NO_ENCRYPTION -eq 1 ]] && mv -f shared/*no_cipher* results/mul_key/no_cipher
if [ $BIT_128 -eq 1 ] || [ $BIT_160 -eq 1 ] || [ $BIT_192 -eq 1 ] || [ $BIT_224 -eq 1 ] || [ $BIT_256 -eq 1 ]; then
    mv -f shared/*cipher* results/mul_key/cipher
fi

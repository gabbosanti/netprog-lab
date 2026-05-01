#!/bin/bash

# Function to display usage
usage() {
    echo "Usage: $0 [--ppt MODE] [--deq MODE] [-r|--read] [-w|--write]"
    echo "Options:"
    echo "  --ppt MODE    Retrieve Packet Processing Time with mode (no-encryption/tls/128/160/192/224/256)"
    echo "  --deq MODE    Retrieve Packet Dequeuing Time with mode (no-encryption/tls/128/160/192/224/256)"
    echo "  -r, --read          Read option"
    echo "  -w, --write         Write option"
    exit 1
}

# Function to validate mode
validate_mode() {
    local mode=$1
    case "$mode" in
    no-encryption | tls | 128 | 160 | 192 | 224 | 256) return 0 ;;
    *) return 1 ;;
    esac
}

# Initialize variables
PPT_FLAG=false
DEQ_FLAG=false
READ_FLAG=false
WRITE_FLAG=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
    --ppt)
        PPT_FLAG=true
        shift
        # If next argument exists and does not look like an option, validate it.
        if [[ $# -gt 0 && $1 != -* ]]; then
            if validate_mode "$1"; then
                MODE="$1"
                shift
            else
                echo "Error: --ppt option requires a valid mode (no-encryption/tls/128/160/192/224/256) if provided."
                usage
            fi
        fi
        ;;
    --deq)
        DEQ_FLAG=true
        shift
        if [[ $# -gt 0 && $1 != -* ]]; then
            if validate_mode "$1"; then
                MODE="$1"
                shift
            else
                echo "Error: --deq option requires a valid mode (no-encryption/tls/128/160/192/224/256) if provided."
                usage
            fi
        fi
        ;;
    -r | --read)
        READ_FLAG=true
        shift
        ;;
    -w | --write)
        WRITE_FLAG=true
        shift
        ;;
    *)
        echo "Error: Unknown option $1"
        usage
        ;;
    esac
done

if [ "$READ_FLAG" = true ]; then
    OPTION="read"
fi

if [ "$WRITE_FLAG" = true ]; then
    OPTION="write"
fi

if [ "$PPT_FLAG" = true ]; then
    echo "s2: PPT option selected"
    if [  -n "$MODE"  ]; then
        if [ "$MODE" = "no-encryption" ]; then FILE=/shared/results_s2_no_cipher_"$OPTION"_packet_processing_time.txt
        elif [ "$MODE" = "tls" ]; then FILE=/shared/results_s2_tls_"$OPTION"_packet_processing_time.txt
        else FILE=/shared/results_s2_cipher_"$OPTION"_packet_processing_time_"${MODE}"-bit.txt
        fi
    else FILE=/shared/results_s2_"$OPTION"_packet_processing_time.txt; fi
    echo "register_read packet_processing_time_array" | simple_switch_CLI >>"$FILE"
    sed -i -n '4{s/.*= //; s/, /\n/g; p}' "$FILE"
    sed -i 's/$/.0/' "$FILE"
fi

if [ "$DEQ_FLAG" = true ]; then
    echo "s2: DEQ option selected"
    if [  -n "$MODE"  ]; then
        if [ "$MODE" = "no-encryption" ]; then FILE=/shared/results_s2_no_cipher_"$OPTION"_packet_dequeuing_timedelta.txt
        elif [ "$MODE" = "tls" ]; then FILE=/shared/results_s2_tls_"$OPTION"_packet_dequeuing_timedelta.txt
        else FILE=/shared/results_s2_cipher_"$OPTION"_packet_dequeuing_timedelta_"${MODE}"-bit.txt
        fi
    else FILE=/shared/results_s2_"$OPTION"_packet_dequeuing_timedelta.txt; fi
    echo "register_read packet_dequeuing_timedelta_array" | simple_switch_CLI >>"$FILE"
    sed -i -n '4{s/.*= //; s/, /\n/g; p}' "$FILE"
    sed -i 's/$/.0/' "$FILE"
fi

# If no options were provided, show usage
if [ "$PPT_FLAG" = false ] && [ "$DEQ_FLAG" = false ]; then
    usage
fi

# echo "packet_processing_time_array: " > /shared/results_s2.txt
# echo "register_read packet_processing_time_array" | simple_switch_CLI >> /shared/results_s2.txt

# echo "" >> /shared/results_s2.txt
# echo "packet_dequeuing_timedelta_array: " >> /shared/results_s2.txt
# echo "register_read packet_dequeuing_timedelta_array" | simple_switch_CLI >> /shared/results_s2.txt

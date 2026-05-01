# This script changes the value of the registers in which the words of the key
# are stored, so there is no need to restart the switch or apply the modifications

# Here we select the different key sizes
if [[ $# -ne 1 ]]; then
    echo 'Too many/few arguments, expecting one' >&2
    exit 1
fi

# delete any line that starts with "register_write keys 4|5|6|7"
    sed -i -E '/^register_write keys (4|5|6|7)/d' commands.txt

case $1 in
    128)
        # set the register from 4 up to 7 to 0
        sed -i '/^register_write keys 3/ a\
register_write keys 4 0\
register_write keys 5 0\
register_write keys 6 0\
register_write keys 7 0' commands.txt
    ;;

    160)
        # set the register from 5 up to 7 to 0
        sed -i '/^register_write keys 3/ a\
register_write keys 4 102358694\
register_write keys 5 0\
register_write keys 6 0\
register_write keys 7 0' commands.txt

    ;;

    192)
        # set the register from 5 up to 7 to 0
        sed -i '/^register_write keys 3/ a\
register_write keys 4 102358694\
register_write keys 5 259174683\
register_write keys 6 0\
register_write keys 7 0' commands.txt

    ;;

    224)
        # set the register from 5 up to 7 to 0
        sed -i '/^register_write keys 3/ a\
register_write keys 4 102358694\
register_write keys 5 259174683\
register_write keys 6 243695780\
register_write keys 7 0' commands.txt

    ;;

    256)
        # set the register from 5 up to 7 to 0
        sed -i '/^register_write keys 3/ a\
register_write keys 4 102358694\
register_write keys 5 259174683\
register_write keys 6 243695780\
register_write keys 7 096548217' commands.txt
    ;;

    *)
        # The wrong first argument.
        echo 'Expected "128", "160", "192", "224" or "256"' >&2
        exit 1
esac

simple_switch_CLI <<< $(cat commands.txt)
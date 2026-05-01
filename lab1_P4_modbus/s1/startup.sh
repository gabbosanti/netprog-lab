simple_switch -i 1@eth0 -i 2@eth1 industrial_aes.json --log-console -- --load-modules=extern_lib/definition.so > /dev/tty 2>&1 &
while [[ $(pgrep simple_switch) -eq 0 ]]; do sleep 1; done
until simple_switch_CLI <<< "help"; do sleep 1; done

simple_switch_CLI <<< $(cat commands.txt)
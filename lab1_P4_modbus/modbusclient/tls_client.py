from pymodbus.client import AsyncModbusTlsClient
import asyncio
import time
import argparse
import random

parser = argparse.ArgumentParser(description="ModBus sample client")
parser.add_argument(
    "--test-rtt-write",
    help="Test the Round Trip Time (write) for 100000 times",
    action="store_true",
    required=False
)
parser.add_argument(
    "--test-rtt-read",
    help="Test the Round Trip Time (read) for 100000 times",
    action="store_true",
    required=False
)
parser.add_argument(
    "--test-read",
    help="Test read from a register for 100000 times",
    action="store_true",
    required=False
)
parser.add_argument(
    "--test-write",
    help="Test write from a register for 100000 times",
    action="store_true",
    required=False
)


args = parser.parse_args()


async def test_rtt_write():
    client = AsyncModbusTlsClient("200.1.1.7", 5020, certfile="./cert.crt", keyfile="./key.key")
    await client.connect()
    register_id = 0
    new_value = 45

    ## WRITE
    with open(f"/shared/results_10*10000_tls_write.txt", "w") as results_file:
        for j in range(10):
            for i in range(10000):
                new_value = random.randint(0, 100)
                time_start = time.time()
                await client.write_register(register_id, new_value, unit=0x01) 
                time_end = time.time()
                results_file.write("%s\n" % (time_end - time_start))
                print(f"RTT - Value written into the register {register_id}: {new_value} - {i} - Test for tls")
    client.close()

async def test_rtt_read():
    client = AsyncModbusTlsClient("200.1.1.7", 5020, certfile="./cert.crt", keyfile="./key.key")
    await client.connect()
    register_id = 0
    new_value = 45
    

    ## READ
    with open(f"/shared/results_10*10000_tls_read.txt", "w") as results_file:
        for j in range(10):
            for i in range(10000):
                time_start = time.time()
                response = await client.read_holding_registers(register_id, 1, unit=0x01)
                time_end = time.time()
                results_file.write("%s\n" % (time_end - time_start))
                print(f"RTT - Value read from the register {register_id}: {response.registers[0]} - {i} - Test for tls")
    client.close()

async def test_read():
    client = AsyncModbusTlsClient("200.1.1.7", 5020, certfile="./cert.crt", keyfile="./key.key")
    await client.connect()
    register_id = 0
    new_value = 45
    

    for j in range(10):
        for i in range(10000):
            response = await client.read_holding_registers(register_id, 1, unit=0x01)
            print(f"PPT-DEQ - Value read from the register {register_id}: {response.registers[0]} - {i} - Test for tls")
    client.close()

async def test_write():
    client = AsyncModbusTlsClient("200.1.1.7", 5020, certfile="./cert.crt", keyfile="./key.key")
    await client.connect()
    register_id = 0
    new_value = 45
    
    
    for j in range(10):
        for i in range(10000):
            new_value = random.randint(0, 100)
            await client.write_register(register_id, new_value, unit=0x01)
            print(f"PPT-DEQ - Value written into the register {register_id}: {new_value} - {i} - Test for tls")
    client.close()
    
# async def main():
#     client = AsyncModbusTlsClient("200.1.1.7", 5020, certfile="./cert.crt", keyfile="./key.key")
#     await client.connect()
#     register_id = 0
#     new_value = 45
#     with open("/shared/results_tls_read.txt", "w") as results_file:
#         for j in range(10):
#             for i in range(10000):
#                 #new_value = random.randint(0, 100)
#                 time_start = time.time()
#                 #await client.write_register(register_id, new_value, unit=0x01) 
#                 await client.read_holding_registers(register_id, 1, unit=0x01) #response = await client.read_holding_registers(register_id, 1, unit=0x01)
#                 time_end = time.time()
#                 results_file.write("%s\n" % (time_end - time_start))
#                 print(f"Valore letto dal registro  - {i}")
#                 #print(f"Valore scritto nel registro {register_id}: {new_value} - {i}")
#     client.close()

# asyncio.run(main())


###### TEST OPEN CONNECTION ######
# async def main():
#     client = AsyncModbusTlsClient("200.1.1.7", 5020, certfile="./cert.crt", keyfile="./key.key")
#     with open("/shared/results_conn_tls.txt", "w") as results_file:
#         for i in range(10000):
#             time_start = time.time()
#             await client.connect()
#             time_end = time.time()
#             results_file.write("%s\n" % (time_end - time_start))
#             client.close()
#             print(f"Test connection number {i}")
# asyncio.run(main())


if args.test_rtt_write:
    asyncio.run(test_rtt_write())
elif args.test_rtt_read:
    asyncio.run(test_rtt_read())
elif args.test_read:
    asyncio.run(test_read())
elif args.test_write:
    asyncio.run(test_write())
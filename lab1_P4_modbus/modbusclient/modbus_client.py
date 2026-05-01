from pymodbus.client import AsyncModbusTcpClient
import asyncio
import time
import argparse
import sys
import random

def validate_mode(value):
    valid_modes = ['no-encryption', '128', '160', '192', '224', '256']
    if value not in valid_modes:
        raise argparse.ArgumentTypeError(
            f'Invalid mode. Must be one of: {", ".join(valid_modes)}'
        )
    return value

parser = argparse.ArgumentParser(description="ModBus sample client")
parser.add_argument(
    "--write", help="If set writes to a register", action="store_true", required=False
)
parser.add_argument(
    "--read", help="If set reads from a register", action="store_true", required=False
)
parser.add_argument(
    "--rw",
    help="If set writes to a register and then reads from a register",
    action="store_true",
    required=False,
)
parser.add_argument(
    "--connect",
    help="If set connect only to the server",
    action="store_true",
    required=False,
)
parser.add_argument(
    "--connect-times",
    help="Thrift server port for table updates",
    action="store",
    default=1,
)
parser.add_argument(
    "--test-rtt-write",
    help="Test the Round Trip Time (write) for 100000 times. Key size must be: 128, 160, 192, 224, or 256",
    type=validate_mode,
    metavar="MODE",
)
parser.add_argument(
    "--test-rtt-read",
    help="Test the Round Trip Time (read) for 100000 times. Key size must be: 128, 160, 192, 224, or 256",
    type=validate_mode,
    metavar="MODE",
)
parser.add_argument(
    "--test-read",
    help="Test read from a register for 100000 times. Key size must be: 128, 160, 192, 224, or 256",
    type=validate_mode,
    metavar="MODE",
)
parser.add_argument(
    "--test-write",
    help="Test write from a register for 100000 times. Key size must be: 128, 160, 192, 224, or 256",
    type=validate_mode,
    metavar="MODE",
)


args = parser.parse_args()

##ORIGINAL
###### TEST AVG TIME ######
# async def main():
#     client = AsyncModbusTcpClient("200.1.1.7", 502)
#     await client.connect()
#     register_id = 0
#     new_value = 45

#     # WRITE
#     with open("/shared/results_10*10000_no_cipher_write.txt", "w") as results_file:
#         for j in range(10):
#             for i in range(10000):
#                 time_start = time.time()
#                 new_value = random.randint(0, 100)
#                 await client.write_register(register_id, new_value, unit=0x01) 
#                 #await client.read_holding_registers(register_id, 1, unit=0x01) #response = await client.read_holding_registers(register_id, 1, unit=0x01)
#                 time_end = time.time()
#                 results_file.write("%s\n" % (time_end - time_start))
#                 #print(f"Valore letto dal registro {register_id} - {i}") #print(f"Valore letto dal registro {register_id}: {response.registers[0]} - {i}")
#                 print(f"Valore scritto nel registro {register_id}: {new_value} - {i}")

#     # READ
#     with open("/shared/results_10*10000_no_cipher_read.txt", "w") as results_file:
#         for j in range(10):
#             for i in range(10000):
#                 time_start = time.time()
#                 response = await client.read_holding_registers(register_id, 1, unit=0x01)
#                 time_end = time.time()
#                 print(f"Valore letto dal registro {register_id}: {response.registers[0]} - {i}")
#     client.close()

# asyncio.run(main())

###### TEST PPT E DEQ in Switch ######
# async def main():
#     client = AsyncModbusTcpClient("200.1.1.7", 502)
#     await client.connect()
#     register_id = 0
#     new_value = 45
#     for j in range(10):
#         for i in range(10000):
#             #await client.write_register(register_id, new_value, unit=0x01)
#             #print(f"Valore scritto nel registro {register_id}: {new_value} - {i}")
#             await client.read_holding_registers(register_id, 1, unit=0x01) #response = await client.read_holding_registers(register_id, 1, unit=0x01)
#             print(f"Valore letto dal registro - {i}")
#     client.close()

# asyncio.run(main())


###### TEST OPEN CONNECTION ######
async def connect():
    client = AsyncModbusTcpClient("200.1.1.7", 502)
    with open("/shared/results_conn.txt", "w") as results_file:
        for i in range(args.connect_times):
            time_start = time.time()
            await client.connect()
            time_end = time.time()
            results_file.write("%s\n" % (time_end - time_start))
            client.close()
            print(f"Test connection number {i}")


async def write_register():
    client = AsyncModbusTcpClient("200.1.1.7", 502, retries=0)
    await client.connect()
    register_id = 0
    new_value = 45
    await client.write_register(register_id, new_value, unit=0x01)
    print(f"Valore scritto nel registro {register_id}: {new_value}")
    client.close()


async def read_register():
    client = AsyncModbusTcpClient("200.1.1.7", 502, retries=0)
    await client.connect()
    register_id = 0
    response = await client.read_holding_registers(register_id, 1, unit=0x01)
    print(f"Valore letto dal registro {register_id}: {response.registers[0]}")
    client.close()

async def test_rtt_write():
    client = AsyncModbusTcpClient("200.1.1.7", 502)
    await client.connect()
    register_id = 0
    new_value = 45
    mode = sys.argv[2]
    
    mode if mode=="no-encryption" else mode +" key"
    
    if (mode=="no-encryption"):
        file=f"/shared/results_10*10000_no_cipher_write.txt"
        cur_mode = mode
    else: 
        file = f"/shared/results_10*10000_cipher_write_{mode}_key.txt"
        cur_mode = mode + "-bit key"

    ## WRITE
    with open(file, "w") as results_file:
        for j in range(10):
            for i in range(10000):
                new_value = random.randint(0, 100)
                time_start = time.time()
                await client.write_register(register_id, new_value, unit=0x01) 
                time_end = time.time()
                results_file.write("%s\n" % (time_end - time_start))
                print(f"RTT - Value written into the register {register_id}: {new_value} - {i} - Test for " + cur_mode )
    client.close()

async def test_rtt_read():
    client = AsyncModbusTcpClient("200.1.1.7", 502)
    await client.connect()
    register_id = 0
    new_value = 45
    mode = sys.argv[2]
    
    if (mode=="no-encryption"):
        file=f"/shared/results_10*10000_no_cipher_read.txt"
        cur_mode = mode
    else: 
        file = f"/shared/results_10*10000_cipher_read_{mode}_key.txt"
        cur_mode = mode + "-bit key"

    ## READ
    with open(file, "w") as results_file:
        for j in range(10):
            for i in range(10000):
                time_start = time.time()
                response = await client.read_holding_registers(register_id, 1, unit=0x01)
                time_end = time.time()
                results_file.write("%s\n" % (time_end - time_start))
                print(f"RTT - Value read from the register {register_id}: {response.registers[0]} - {i} - Test for " + cur_mode)
    client.close()

async def test_read():
    client = AsyncModbusTcpClient("200.1.1.7", 502)
    await client.connect()
    register_id = 0
    new_value = 45
    mode = sys.argv[2]
    
    if (mode=="no-encryption"):
        cur_mode = mode
    else:
        cur_mode = mode + "-bit key"

    for j in range(10):
        for i in range(10000):
            response = await client.read_holding_registers(register_id, 1, unit=0x01)
            print(f"PPT-DEQ - Value read from the register {register_id}: {response.registers[0]} - {i} - Test for " + cur_mode)
    client.close()

async def test_write():
    client = AsyncModbusTcpClient("200.1.1.7", 502)
    await client.connect()
    register_id = 0
    new_value = 45
    mode = sys.argv[2]
    
    if (mode=="no-encryption"):
        cur_mode = mode
    else:
        cur_mode = mode + "-bit key"
    
    for j in range(10):
        for i in range(10000):
            new_value = random.randint(0, 100)
            await client.write_register(register_id, new_value, unit=0x01)
            print(f"PPT-DEQ - Value written into the register {register_id}: {new_value} - {i} - Test for "+ cur_mode)
    client.close()


if args.write:
    asyncio.run(write_register())
elif args.read:
    asyncio.run(read_register())
elif args.rw:
    asyncio.run(read_register())
    asyncio.run(write_register())
    asyncio.run(read_register())
elif args.connect:
    asyncio.run(connect())
elif args.test_rtt_write:
    asyncio.run(test_rtt_write())
elif args.test_rtt_read:
    asyncio.run(test_rtt_read())
elif args.test_read:
    asyncio.run(test_read())
elif args.test_write:
    asyncio.run(test_write())

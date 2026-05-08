import asyncio

from pymodbus.datastore import ModbusSequentialDataBlock, ModbusServerContext, ModbusSlaveContext
from pymodbus.server import StartAsyncTcpServer
async def run_async_server():
    datablock = ModbusSequentialDataBlock.create()
    context = ModbusSlaveContext(
        di=datablock, co=datablock, hr=datablock, ir=datablock
    )
    single = True
    context = ModbusServerContext(slaves=context, single=single)
    address = ("200.1.1.7", 502)
    print('starting server...')
    server = await StartAsyncTcpServer(context=context, address=address)
    server.write
    register_id = 0
    new_value = 43
    await server.write_register(register_id, new_value, unit=0x01)
    return server

async def main():
    await run_async_server()
    
if __name__ == "__main__":
    asyncio.run(main(),debug=True)
#!/usr/bin/env python3
"""
CoAP客户端示例
需要安装: pip install aiocoap
"""
import asyncio
from aiocoap import Context, Message, GET, PUT, POST


async def get_request(uri):
    """发送GET请求"""
    context = await Context.create_client_context()
    request = Message(code=GET, uri=uri)
    
    try:
        response = await context.request(request).response
        print(f"GET {uri}")
        print(f"Response: {response.payload.decode('utf-8')}\n")
    except Exception as e:
        print(f"Failed to fetch resource: {e}\n")


async def put_request(uri, payload):
    """发送PUT请求"""
    context = await Context.create_client_context()
    request = Message(code=PUT, uri=uri, payload=payload.encode('utf-8'))
    
    try:
        response = await context.request(request).response
        print(f"PUT {uri}")
        print(f"Payload: {payload}")
        print(f"Response: {response.payload.decode('utf-8')}\n")
    except Exception as e:
        print(f"Failed to update resource: {e}\n")


async def post_request(uri, payload):
    """发送POST请求"""
    context = await Context.create_client_context()
    request = Message(code=POST, uri=uri, payload=payload.encode('utf-8'))
    
    try:
        response = await context.request(request).response
        print(f"POST {uri}")
        print(f"Payload: {payload}")
        print(f"Response: {response.payload.decode('utf-8')}\n")
    except Exception as e:
        print(f"Failed to post to resource: {e}\n")


async def main():
    print("=== CoAP Client Demo ===\n")
    
    # 1. GET请求 - 获取hello消息
    await get_request('coap://localhost:5683/hello')
    
    # 2. GET请求 - 读取温度
    await get_request('coap://localhost:5683/sensor/temperature')
    
    # 3. PUT请求 - 更新温度
    await put_request('coap://localhost:5683/sensor/temperature', '28.5')
    
    # 4. GET请求 - 验证温度更新
    await get_request('coap://localhost:5683/sensor/temperature')
    
    # 5. POST请求 - 发送控制命令
    await post_request('coap://localhost:5683/control', 'START_MOTOR')
    await post_request('coap://localhost:5683/control', 'STOP_MOTOR')


if __name__ == "__main__":
    asyncio.run(main())

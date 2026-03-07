#!/usr/bin/env python3
"""
CoAP Observe模式示例 - 订阅资源变化
需要安装: pip install aiocoap
"""
import asyncio
from aiocoap import Context, Message, GET


async def observe_resource(uri):
    """订阅资源变化"""
    context = await Context.create_client_context()
    
    # 创建observe请求
    request = Message(code=GET, uri=uri, observe=0)
    
    print(f"Observing {uri}...")
    print("Press Ctrl+C to stop\n")
    
    try:
        # 发送请求并获取响应流
        protocol_request = context.request(request)
        response = await protocol_request.response
        
        print(f"Initial value: {response.payload.decode('utf-8')}")
        
        # 持续接收更新
        async for response in protocol_request.observation:
            print(f"Updated value: {response.payload.decode('utf-8')}")
            
    except KeyboardInterrupt:
        print("\nStopped observing")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    # 订阅温度传感器
    asyncio.run(observe_resource('coap://localhost:5683/sensor/temperature'))

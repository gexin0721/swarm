#!/usr/bin/env python3
"""
CoAP服务端示例
需要安装: pip install aiocoap
"""
import asyncio
import aiocoap
import aiocoap.resource as resource


class HelloResource(resource.Resource):
    """简单的GET资源"""
    async def render_get(self, request):
        return aiocoap.Message(payload=b"Hello from CoAP Server!")


class TemperatureResource(resource.Resource):
    """模拟温度传感器资源"""
    def __init__(self):
        super().__init__()
        self.temperature = 25.0
    
    async def render_get(self, request):
        payload = f"Temperature: {self.temperature}°C".encode('utf-8')
        return aiocoap.Message(payload=payload)
    
    async def render_put(self, request):
        try:
            self.temperature = float(request.payload.decode('utf-8'))
            return aiocoap.Message(code=aiocoap.CHANGED, payload=b"Temperature updated")
        except ValueError:
            return aiocoap.Message(code=aiocoap.BAD_REQUEST, payload=b"Invalid value")


class ControlResource(resource.Resource):
    """设备控制资源 - 支持POST"""
    async def render_post(self, request):
        command = request.payload.decode('utf-8')
        print(f"Received command: {command}")
        response = f"Executed: {command}".encode('utf-8')
        return aiocoap.Message(code=aiocoap.CHANGED, payload=response)


async def main():
    # 创建根资源
    root = resource.Site()
    
    # 添加资源
    root.add_resource(['hello'], HelloResource())
    root.add_resource(['sensor', 'temperature'], TemperatureResource())
    root.add_resource(['control'], ControlResource())
    
    # 启动服务器
    await aiocoap.Context.create_server_context(root, bind=('localhost', 5683))
    
    print("CoAP Server running on coap://localhost:5683")
    print("Resources:")
    print("  - coap://localhost:5683/hello")
    print("  - coap://localhost:5683/sensor/temperature")
    print("  - coap://localhost:5683/control")
    
    # 保持运行
    await asyncio.get_running_loop().create_future()


if __name__ == "__main__":
    asyncio.run(main())

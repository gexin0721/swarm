# 通义千问大模型API封装类
import json
from transformers import AutoTokenizer

class Qwen:
    def __init__(self, message: dict):
        self.api_key = message.get("key")
        self.base_url = message.get("params").get("base_url")
        self.model = message.get("params").get("model")
        # ================ 可选参数设置 ================
        self.stream_options = True # 是否启用深度思考模式
        self.temperature = 0.8 # 温度参数，控制生成文本的随机性，越大越随机
        self.max_tokens = message.get("params").get("max_tokens", 8000)  # 从配置读取，默认8000
        self.top_k = 5 # 从k个候选中随机选择一个
        self.auditing = "default" # 审核设置

        # ================ 模型名称到HuggingFace tokenizer路径的映射 ================
        # API模型名称到HuggingFace tokenizer路径的映射
        tokenizer_map = {
            "qwen-turbo": "Qwen/Qwen-7B-Chat",
            "qwen-plus": "Qwen/Qwen-14B-Chat",
            "qwen-max": "Qwen/Qwen-72B-Chat",
            "qwen-max-longcontext": "Qwen/Qwen-72B-Chat",
        }
        # 如果model在映射表中，使用映射的路径；否则假定model本身就是HuggingFace路径
        tokenizer_path = tokenizer_map.get(self.model, "Qwen/Qwen-7B-Chat")
        self.tokenizer = AutoTokenizer.from_pretrained(tokenizer_path, trust_remote_code=True)



        
    def set_api_key(self, api_key: str):
        self.api_key = api_key

    def set_base_url(self, base_url: str):
        self.base_url = base_url

    def set_model(self, model: str):
        self.model = model

    #  ============ 生成链接参数 ============
    def gen_params(self):
        return {
            "api_key": self.api_key,
            "base_url": self.base_url,
        }
    #  ============ 生成请求参数 ============
    def gen_request(self, messages: list):
        """
        生成请求参数，仿照星火 gen_params 方法
        会读取和保存对话历史，并生成请求体
        """
        # 生成请求参数
        return {
            "model": self.model,
            "messages": messages,  # Qwen/ChatGLM等API参数名通常为messages
        }
        
    #  ============ 生成请求参数(流式) ============
    def gen_params_stream(self, messages: list):
        """
        生成流式请求参数
        """
        return {
            "model": self.model,
            "messages": messages,  # Qwen/ChatGLM等API参数名通常为messages
            "stream": True,
            "stream_options": {"include_usage": True}
        }

    #  ============ 判断流式是否结束 ============
    def is_stream_end(self, stream_options: dict) -> bool:
        """
        判断流式是否结束
        usage 字段为 None 表示还在传输数据，否则为结束
        """
        # usage 字段为 None，则说明还在传输，非None视为结束
        usage = stream_options.get("usage", None)
        return usage is not None
        
    #  ============ 提取流式信息数据 ============
    def extract_stream_info(self, stream_options: dict) -> str:
        """
        提取流式信息数据, 提取delta.content和delta.thinking字段
        优先返回thinking（思考过程），如果没有则返回content（最终答案）
        """
        # 'choices' 应为一个list
        choices = stream_options.get('choices', [])
        if not choices or not isinstance(choices, list):
            return None
        choice = choices[0]
        delta = choice.get('delta', {}) if isinstance(choice, dict) else {}
        
        # 优先提取thinking字段（深度思考模式）
        thinking = delta.get('thinking', None)
        if thinking:
            return f"💭 {thinking}"  # 用特殊标记区分思考过程
        
        # 如果没有thinking，提取content（最终答案）
        return delta.get('content', None)

    #  ============ 计算token的回调函数 ============
    # 阿里（通义千问：Qwen 系列）
    # 工具：transformers库加载 Qwen 的 tokenizer（开源模型）或官方 API 的usage字段
    # 原理：基于 BPE，中文分词粒度较细（单字或词）。
    def token_callback(self, content: str) -> int:
            """计算通义千问模型的token数"""
            return len(self.tokenizer.encode(content, add_special_tokens=False))

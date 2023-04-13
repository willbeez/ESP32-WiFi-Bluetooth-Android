import os
from langchain.agents import load_tools
from langchain.agents import initialize_agent
from langchain.chat_models import ChatOpenAI

os.environ['OPENAI_API_KEY'] = "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"

llm = ChatOpenAI(temperature=0)

tool_names = ["terminal"]
tools = load_tools(tool_names, llm)

agent = initialize_agent(tools, llm, "zero-shot-react-description", verbose=False, max_iterations=3)
while True:
    print(agent.run(f"""
{input("User: ")}
"""))

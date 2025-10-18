# Task/Agent Tool Instructions

Launch a new agent/sub-task that has access to available tools. Use this when you need to delegate work to a specialized agent or perform complex multi-step operations autonomously.

## When to Use the Agent Tool

- If you are searching for a keyword like "config" or "logger", or for questions like "which file does X?", the Agent tool is strongly recommended
- When you need to perform open-ended searches that may require multiple rounds of searching and file reading
- For complex analysis tasks that require exploring multiple files and directories
- When the task would benefit from autonomous exploration without constant back-and-forth

## When NOT to Use the Agent Tool

- If you want to read a specific file path, use the Read or Glob tool instead of the Agent tool, to find the match more quickly
- If you are searching for a specific class definition like "class Foo", use the Glob tool instead, to find the match more quickly
- If you are searching for code within a specific file or set of 2-3 files, use the Read tool instead of the Agent tool, to find the match more quickly
- Writing code and running bash commands (use other tools for that)
- Other tasks that are not related to searching for a keyword or file

## Usage Notes

1. **Launch multiple agents concurrently** whenever possible, to maximize performance; to do that, use a single message with multiple tool uses
2. **Agent communication**: When the agent is done, it will return a single message back to you. The result returned by the agent is not visible to the user. To show the user the result, you should send a text message back to the user with a concise summary of the result
3. **Stateless execution**: Each agent invocation is stateless. You will not be able to send additional messages to the agent, nor will the agent be able to communicate with you outside of its final report. Therefore, your prompt should contain a highly detailed task description for the agent to perform autonomously and you should specify exactly what information the agent should return back to you in its final and only message to you
4. **Trust agent outputs**: The agent's outputs should generally be trusted
5. **Clarify expectations**: Clearly tell the agent whether you expect it to write code or just to do research (search, file reads, web fetches, etc.), since it is not aware of the user's intent
6. **Specify subagent type**: When using the Task tool, you must specify a subagent_type parameter to select which type of specialized agent to use (e.g., general-purpose, code-reviewer, etc.)

## Available Agent Types

Different systems may provide different specialized agent types. Common types include:
- **general-purpose**: General-purpose agent for researching complex questions, searching for code, and executing multi-step tasks
- **code-reviewer**: Agent specialized in reviewing code for quality, bugs, and best practices
- **test-runner**: Agent specialized in running tests and analyzing test results

## Best Practices

- Use agents for research and exploration tasks
- Provide detailed, comprehensive instructions in your agent prompt
- Specify exactly what information you need back from the agent
- Launch multiple agents in parallel when tasks are independent
- Don't use agents for simple, direct operations that can be done with basic tools

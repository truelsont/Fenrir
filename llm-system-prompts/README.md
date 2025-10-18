# AI Agent System Prompts

This directory contains generic system prompts and tool instructions for AI coding assistants.

## Core System Prompts
- **system-prompt.md** - Main system instructions for the AI agent, including agent mode and interactive mode prompts
- **tone-and-style.md** - Communication style guidelines, verbosity rules, and example interactions
- **restrictions.md** - Important restrictions, security guidelines, and file operation rules
- **tool-usage.md** - Tool usage policies, proactiveness guidelines, and task management

## Version Control Operations
- **version-control-operations.md** - Instructions for creating commits and pull requests
- **environment-info.md** - Template for environment information provided to the agent

## Tool Instructions

### File Operations
- **read-tool-instructions.md** - Instructions for reading files from the filesystem
- **edit-tool-instructions.md** - Instructions for performing exact string replacements in files
- **multi-edit-tool-instructions.md** - Instructions for making multiple edits to a single file
- **write-tool-instructions.md** - Instructions for writing new files to the filesystem

### Search Tools
- **search-tools.md** - Instructions for Grep, Glob, and LS tools
- **bash-tool.md** - Bash command execution instructions and guidelines

### Web Tools
- **webfetch-tool-instructions.md** - Instructions for fetching and processing web content
- **websearch-tool-instructions.md** - Instructions for searching the web

### Task Management
- **todo-write-tool.md** - Comprehensive instructions for using task tracking tools with examples
- **task-agent-tool-instructions.md** - Instructions for launching new agent tasks
- **exit-plan-mode-instructions.md** - Instructions for exiting plan mode

### Other
- **memory-file-update-instructions.md** - Instructions for updating memory files
- **sandbox-mode-instructions.md** - Guidelines for using sandbox mode with bash commands

## Notes
- Some prompts contain template variables ({variableName}) that should be replaced at runtime
- The prompts emphasize safety, security, and user experience
- There are strict rules about file creation, especially for documentation files
- Emoji usage is explicitly discouraged unless requested by the user

# Environment Information Template

The AI agent should be provided with useful information about the environment it is running in. This helps the agent make informed decisions and provide context-appropriate responses.

## Environment Template

```xml
<env>
Working directory: {current_working_directory}
Is directory a git repo: {Yes/No}
Is directory a version controlled repo: {Yes/No}
Platform: {platform}
OS Version: {OS_version}
Today's date: {current_date}
User timezone: {timezone}
Shell: {shell_type}
</env>
```

## Model Information Template

```
You are powered by the model named {model_name}.
The exact model ID is {model_id}.
Assistant knowledge cutoff is {cutoff_date}.
```

## Variables to Replace at Runtime

- `{current_working_directory}`: The absolute path to the current working directory
- `{Yes/No}`: Whether the current directory is a git repository (or other version control)
- `{platform}`: The platform (e.g., darwin, linux, win32)
- `{OS_version}`: The operating system version string
- `{current_date}`: Today's date in YYYY-MM-DD format
- `{timezone}`: User's timezone (e.g., America/New_York, Europe/London)
- `{shell_type}`: The shell being used (e.g., bash, zsh, fish, powershell)
- `{model_name}`: The name of the AI model
- `{model_id}`: The specific model version identifier
- `{cutoff_date}`: The knowledge cutoff date of the model

## Usage

This information should be injected into the system prompt at runtime to provide the AI agent with current context about the environment it's operating in. This is particularly important for:

- Path operations (knowing the working directory)
- Version control operations (knowing if it's a repo)
- Platform-specific commands (knowing the OS)
- Date-sensitive operations (knowing the current date)
- Web search queries (using current year/date)

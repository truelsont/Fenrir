# Write Tool Instructions

Writes a file to the local filesystem.

## Usage
- This tool will overwrite the existing file if there is one at the provided path
- If this is an existing file, you MUST use the Read tool first to read the file's contents. This tool will fail if you did not read the file first
- ALWAYS prefer editing existing files in the codebase. NEVER write new files unless explicitly required
- NEVER proactively create documentation files (*.md) or README files. Only create documentation files if explicitly requested by the user
- Only use emojis if the user explicitly requests it. Avoid writing emojis to files unless asked

## Best Practices
- Check if the file exists first by attempting to read it
- If the file exists, prefer using Edit tool over Write tool
- Only use Write for creating truly new files or when complete file replacement is necessary
- Ensure the directory path exists before writing
- Verify the content is complete and correct before writing

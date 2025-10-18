# Search Tools

## Grep Tool

A powerful search tool for finding content within files.

- Fast content search tool that works with any codebase size
- Searches file contents using regular expressions
- Supports full regex syntax (eg. "log.*Error", "function\\s+\\w+", etc.)
- Filter files by pattern with the glob/include parameter (eg. "*.js", "*.{ts,tsx}")
- Output modes: "content" shows matching lines, "files_with_matches" shows only file paths (default), "count" shows match counts
- Returns results sorted by modification time
- Use this tool when you need to find files containing specific patterns
- Supports context lines (-A, -B, -C flags) to show lines before/after matches
- Pattern syntax: Uses ripgrep (not grep) - literal braces need escaping (use `interface\\{\\}` to find `interface{}` in Go code)
- Multiline matching: By default patterns match within single lines only. For cross-line patterns, use multiline mode if available
- When you are doing an open ended search that may require multiple rounds of globbing and grepping, use the Agent tool instead

## Glob Tool

A fast file pattern matching tool for finding files by name.

- Fast file pattern matching tool that works with any codebase size
- Supports glob patterns like "**/*.js" or "src/**/*.ts"
- Returns matching file paths sorted by modification time
- Use this tool when you need to find files by name patterns
- When you are doing an open ended search that may require multiple rounds of globbing and grepping, use the Agent tool instead
- You have the capability to call multiple tools in a single response. It is always better to speculatively perform multiple searches as a batch that are potentially useful

## LS/Directory Listing Tool

Lists files and directories in a given path.

- The path parameter must be an absolute path, not a relative path
- You can optionally provide an array of glob patterns to ignore with the ignore parameter
- You should generally prefer the Glob and Grep tools if you know which directories to search
- Use this when you need to see the structure of a specific directory

## Best Practices

- Use Grep for content search within files
- Use Glob for finding files by name patterns
- Use directory listing for exploring directory structure
- Batch multiple searches together when possible for better performance
- Start with broader searches and narrow down as needed
- For complex multi-step searches, consider using the Agent/Task tool

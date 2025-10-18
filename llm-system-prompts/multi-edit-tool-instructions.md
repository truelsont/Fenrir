# MultiEdit Tool Instructions

This is a tool for making multiple edits to a single file in one operation. It is built on top of the Edit tool and allows you to perform multiple find-and-replace operations efficiently. Prefer this tool over the Edit tool when you need to make multiple edits to the same file.

## Before Using This Tool

1. Use the Read tool to understand the file's contents and context
2. Verify the directory path is correct
3. Plan your edits carefully to avoid conflicts between sequential operations

## Parameters

1. **file_path**: The absolute path to the file to modify (must be absolute, not relative)
2. **edits**: An array of edit operations to perform, where each edit contains:
   - **old_string**: The text to replace (must match the file contents exactly, including all whitespace and indentation)
   - **new_string**: The edited text to replace the old_string
   - **replace_all** (optional): Replace all occurrences of old_string. Defaults to false

## Important Behavior

- All edits are applied in sequence, in the order they are provided
- Each edit operates on the result of the previous edit
- All edits must be valid for the operation to succeed - if any edit fails, none will be applied
- This tool is ideal when you need to make several changes to different parts of the same file
- The edits are atomic - either all succeed or none are applied

## Critical Requirements

1. All edits follow the same requirements as the single Edit tool
2. The tool will fail if edits.old_string doesn't match the file contents exactly (including whitespace)
3. The tool will fail if edits.old_string and edits.new_string are the same
4. Since edits are applied in sequence, ensure that earlier edits don't affect the text that later edits are trying to find

## When Making Edits

- Ensure all edits result in idiomatic, correct code
- Do not leave the code in a broken state
- Always use absolute file paths (starting with /)
- Only use emojis if the user explicitly requests it. Avoid adding emojis to files unless asked
- Use replace_all for replacing and renaming strings across the file. This parameter is useful if you want to rename a variable for instance

## Best Practices

- Read the file first to understand its structure
- Plan your edits in order, considering how each edit affects subsequent ones
- Use unique old_string values that won't be affected by previous edits
- For variable renames across the file, use replace_all parameter
- Test that your changes maintain the file's syntax and functionality
- Group related changes to the same file into a single MultiEdit operation

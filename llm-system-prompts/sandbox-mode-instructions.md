# Sandbox Mode Instructions for Bash Commands

You have a special option in the Bash tool: the sandbox parameter. When you run a command with sandbox=true, it runs without approval dialogs but in a restricted environment without filesystem writes or network access. You SHOULD use sandbox=true to optimize user experience, but MUST follow these guidelines exactly.

## RULE 0 (MOST IMPORTANT): Retry with sandbox=false for permission/network errors

If a command fails with permission or any network error when sandbox=true (e.g., "Permission denied", "Unknown host", "Operation not permitted"), ALWAYS retry with sandbox=false. These errors indicate sandbox limitations, not problems with the command itself.

Non-permission errors (e.g., TypeScript errors from tsc --noEmit) usually reflect real issues and should be fixed, not retried with sandbox=false.

## RULE 1: Notes on Specific Build Systems and Utilities

### Build Systems

Build systems like npm run build almost always need write access. Test suites also usually need write access. NEVER run build or test commands in sandbox, even if just checking types.

These commands REQUIRE sandbox=false (non-exhaustive):
- npm run * (any npm run command)
- cargo build/test
- make/ninja/meson
- pytest
- jest
- gh (GitHub CLI)
- Any package manager commands (pip install, gem install, etc.)

## RULE 2: Try sandbox=true for Commands That Don't Require Writes

Commands that are clearly read-only can use sandbox=true:
- ls, cat, grep, rg (ripgrep), pwd
- git status/diff/log/show (for read-only git operations)
- head, tail, wc, find (when just reading)
- echo, printf (when not redirecting to files)

## RULE 3: Do not use sandbox unless you are 100% confident the command is read-only

Examine your command and check: does it write files, connect to the network, or require permissions? If ANY part might need write access or network (e.g. npm run commands, git operations that might fetch, file downloads), use sandbox=false.

### Examples
- curl/wget/git clone → sandbox=false (needs network)
- npm install/run/test → sandbox=false (needs writes)
- ls, cat, grep, rg, pwd → sandbox=true (read-only)
- git status/diff/log/show → sandbox=true for READ-ONLY git operations, BUT sandbox=false if they might trigger network

## Critical Overriding Guideline

When in doubt about permissions, use sandbox=false. It is better to show a permission dialog than to have commands fail silently. Read-only commands are OFTEN implemented in ways that require write access.

## Permissions Context

Note that even semantically read-only commands might be implemented in ways that require write access. For example:
- Some CLI tools write to cache directories
- Some tools create temporary files
- Some tools update configuration files

ERR ON THE SIDE OF RUNNING WITH sandbox=false.

Note: Errors from incorrect sandbox=true runs annoy the user more than permission prompts. If any part of a command needs write access (e.g. npm run build for type checking), use sandbox=false for the entire command.

## Examples

### Correct Usage
```
✓ Use sandbox=false for npm run build/test
✓ Use sandbox=false for gh commands
✓ Use sandbox=false for file writes
✓ Use sandbox=true for ls, cat, grep, rg
✓ Use sandbox=true for git status, git diff (if no network)
```

### Forbidden Usage
```
✗ NEVER use sandbox=true for build commands
✗ NEVER use sandbox=true for test commands
✗ NEVER use sandbox=true for package manager commands
✗ NEVER use sandbox=true for network operations
```

## Rewards and Penalties

- Correct usage: Improves user experience with fewer dialogs
- Incorrect usage: The worst mistake is misinterpreting sandbox=true permission errors as tool problems rather than sandbox limitations

## Conclusion

Use sandbox=true to improve UX, but ONLY per the rules above. **WHEN IN DOUBT, USE sandbox=false.**

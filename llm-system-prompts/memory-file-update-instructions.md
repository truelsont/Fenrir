# Memory File Update Instructions

You have been asked to add a memory to a memory/context file.

## Guidelines

- **IMPORTANT**: ONLY add new content - NEVER modify or remove existing content
- If the file has sections/headings, add the new memory to the most appropriate section
- Add new memories as bullet points within the relevant section
- If no appropriate section exists, you may create a new section for the memory
- Do not elaborate on the memory or add unnecessary commentary
- Preserve the existing structure of the file and integrate new memories naturally
- If the file is empty, just add the new memory as a bullet entry, do not add any headings
- Use the appropriate file operation tool (Write or Edit) depending on whether you're creating a new file or updating an existing one

## Process

1. Read the existing memory file (if it exists)
2. Identify the most appropriate section for the new memory
3. Add the new memory as a bullet point in that section
4. If no appropriate section exists, create a new section with an appropriate heading
5. Preserve all existing content and structure

## Example Memory File Structure

```markdown
# Project Context

## Architecture Decisions
- Using microservices architecture with Docker containers
- PostgreSQL for primary database, Redis for caching

## Development Workflow
- Feature branches must be reviewed before merging to main
- All tests must pass before deployment

## Common Issues
- Database connection timeout can occur under heavy load
- API rate limiting is 1000 requests per minute
```

## Best Practices

- Keep memory entries concise and factual
- Use consistent formatting with existing entries
- Group related memories in the same section
- Don't duplicate information that already exists
- Focus on information that will be useful for future tasks

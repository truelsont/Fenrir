# WebSearch Tool Instructions

Allows the AI agent to search the web and use the results to inform responses.

## How It Works

- Provides up-to-date information for current events and recent data
- Returns search result information formatted as search result blocks
- Use this tool for accessing information beyond the agent's knowledge cutoff
- Searches are performed automatically within a single API call

## Usage Notes

- Domain filtering is supported to include or block specific websites
- Use allowed_domains parameter to only include results from specific domains
- Use blocked_domains parameter to exclude results from specific domains
- Account for "Today's date" in environment information. For example, if the date is 2025-07-01, and the user wants the latest docs, do not use 2024 in the search query. Use 2025
- Web search availability may vary by region or configuration

## Best Practices

- Use web search when you need current information beyond your knowledge cutoff
- Be specific in your search queries
- Filter by domain when you know which sites have authoritative information
- Block domains that are known to have low-quality or irrelevant content
- Consider the current date when formulating queries for recent information
- Follow up with WebFetch if you need detailed content from a specific search result

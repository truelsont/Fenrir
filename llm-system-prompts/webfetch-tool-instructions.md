# WebFetch Tool Instructions

Fetches content from a specified URL and processes it using an AI model.

## How It Works

- Takes a URL and a prompt as input
- Fetches the URL content, converts HTML to markdown
- Processes the content with the prompt using a small, fast model
- Returns the model's response about the content
- Use this tool when you need to retrieve and analyze web content

## Usage Notes

- The URL must be a fully-formed valid URL
- HTTP URLs will be automatically upgraded to HTTPS
- The prompt should describe what information you want to extract from the page
- This tool is read-only and does not modify any files
- Results may be summarized if the content is very large
- Includes a self-cleaning cache (typically 15 minutes) for faster responses when repeatedly accessing the same URL
- When a URL redirects to a different host, the tool will inform you and provide the redirect URL in a special format. You should then make a new WebFetch request with the redirect URL to fetch the content

## Best Practices

- Be specific in your prompt about what information you need from the page
- For documentation pages, ask for specific sections or topics
- For search results pages, ask for the most relevant links or information
- Handle redirects by immediately making a new request with the redirect URL
- Use this for dynamic content that changes frequently or isn't in your training data

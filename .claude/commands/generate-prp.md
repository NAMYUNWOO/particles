# Create PRP

## Feature file: $ARGUMENTS

Generate a complete PRP for general feature implementation with thorough research. Ensure context is passed to the AI agent to enable self-validation and iterative refinement. Read the feature file first to understand what needs to be created, how the examples provided help, and any other considerations.

The AI agent only gets the context you are appending to the PRP and training data. Assuma the AI agent has access to the codebase and the same knowledge cutoff as you, so its important that your research findings are included or referenced in the PRP. The Agent has Websearch capabilities, so pass urls to documentation and examples.

## Research Process

1. **Codebase Analysis**
   - Use context7 mcp 
   - Search for similar features/patterns in the codebase
   - Search for features/patterns in `examples/`
   - Identify files to reference in PRP
   - Note existing conventions to follow
   - Check test patterns for validation approach


3. **External Research**
   - Raylib context7 latest doc:
   - Raylib documentation: https://github.com/raysan5/raylib/wiki/
   - Raylib Cheatsheet: https://www.raylib.com/cheatsheet/cheatsheet.html
   - Search for similar features/patterns online
   - Library documentation (include specific URLs)
   - Implementation examples (GitHub/StackOverflow/blogs)
   - Best practices and common pitfalls

4. **User Clarification** (if needed)
   - Specific patterns to mirror and where to find them?
   - Integration requirements and where to find them?



## 📖 Accessing Raylib Documentation via Context7

When you need up-to-date raylib documentation, code examples, or API references, use the Context7 MCP tools:

### Step 1: Resolve the Library ID (First Time Only)
```
mcp__context7__resolve-library-id raylib
```
This returns the Context7-compatible library ID: `/raysan5/raylib`

### Step 2: Fetch Documentation
```
mcp__context7__get-library-docs /raysan5/raylib
```

**Optional parameters:**
- `tokens`: Maximum documentation size (default: 5000, increase for more comprehensive docs)
- `topic`: Focus on specific areas (e.g., "particle systems", "collision detection", "shaders", "audio")

**Examples:**
```
# Get general raylib documentation
mcp__context7__get-library-docs /raysan5/raylib

# Get comprehensive documentation (more tokens)
mcp__context7__get-library-docs /raysan5/raylib tokens=10000

# Get focused documentation on specific topic
mcp__context7__get-library-docs /raysan5/raylib topic="collision detection"
```


## PRP Generation

Using PRPs/templates/prp_base.md as template:

### Critical Context to Include and pass to the AI agent as part of the PRP
- **Documentation**: URLs with specific sections
- **Code Examples**: Real snippets from codebase
- **Gotchas**: Library quirks, version issues
- **Patterns**: Existing approaches to follow

### Implementation Blueprint
- Start with pseudocode showing approach
- Reference real files for patterns
- Include error handling strategy
- list tasks to be completed to fullfill the PRP in the order they should be completed

### Validation Gates (Must be Executable) eg for C lang
```bash
# Syntax/Style
cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem src/
```

### Unit Test
- use `src/minunit/minunit.h`, refer example `examples/minunit/`

*** CRITICAL AFTER YOU ARE DONE RESEARCHING AND EXPLORING THE CODEBASE BEFORE YOU START WRITING THE PRP ***

*** ULTRATHINK ABOUT THE PRP AND PLAN YOUR APPROACH THEN START WRITING THE PRP ***

## Output
Save as: `PRPs/{feature-name}.md`

## Quality Checklist
- [ ] All necessary context included
- [ ] Validation gates are executable by AI
- [ ] Unit Tests are executable by AI, if needed.
- [ ] References existing patterns
- [ ] Clear implementation path
- [ ] Error handling documented

Score the PRP on a scale of 1-10 (confidence level to succeed in one-pass implementation using claude codes)

Remember: The goal is one-pass implementation success through comprehensive context.
# Commit messages guidance

### Basic suggestion

- Don't mess all changes in one commit, commit per change
- Write exact and understandable subject
- Provide more detailed description
- Avoid non linear commit history

### Structure

Commit should be made up with three parts:

```
<Subject>

<Description>

<Tags and External References>
```

#### Subject

```
<type>(optional scope): <subject-description>(50 characters max)

example:
chore: Update Python version to use newer libs
```

#### Description

```
More recent versions of important project libs no longer support Python
3.6. This has prevented us from using new features offered by such libs.
Add support for Python 3.12.
```

### Types table

| Type         | Description                                 |
| :----------- | :------------------------------------------ |
| **feat**     | Introduce a new feature to the codebase     |
| **fix**      | Fix a bug in the codebase                   |
| **docs**     | Create/update documentation                 |
| **style**    | Feature and updates related to styling      |
| **refactor** | Refactor a specific section of the codebase |
| **test**     | Add or update code related to testing       |
| **chore**    | Regular code maintenance                    |

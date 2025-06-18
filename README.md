# IRC

## Contents

* [Coding](#coding-%EF%B8%8F)
   * [General rules](#general-rules-)
   * [Header Organisation](#header-organisation)
   * [Comments](#comments)
   * [Norm](#norm)
* [Git Organisation](#git-organisation-)
   * [Commit Rules](#commit-rules)
   * [Branch](#branch)
      * [Rule](#rule)
      * [Commands](#commands)
   * [Log commands](#log-commands)
   * [Update a local branch](#update-a-local-branch)
   * [Update your branch with current main](#update-your-branch-with-current-main)
   * [Merge](#merge)
   * [Rebase](#rebase)
   * [Git Helper](#git-helper)
* [Links](#links)

## Coding ⌨️

### General rules 📏
- Class names must be written in UpperCamelCase.
- Classes must have a `.cpp` and `.hpp`.
- Functions in cpp file must be written in the read direction.
- Braces are alone on their own line for conditions and functions (can be on the same line for the rest).
- If an object is passed in a function and not modified by it, it must be declared with `const` and passed by reference.
- If a simple type (int, float, pointer, etc.) is passed to a function and not modified by it, it must be declared with `const`.
- If possible, not use the accessor and avoid as much as possible mutator.
- Use a private/protected function whenever is possible.
- Function's and variable's names must be written in camelCase.
- Members must be private or protected.
- Members in class must be finished with `_`.
- `&` and `*` must be attached to the type.

### Header Organisation
1) public
   - public types (typedef, using, enum)
   - Constructors, destructor
   - public methods
   - getters/setters
   - Overload operator

2) protected
    - protected methods
    - protected members

3) private
    - private methods
    - private members

### Comments
- Try to comment as much as possible on the functions.
- Comments should be in a format similar to :
```c++
/**
 * @brief Copy the first size-1 characters of a source to a destination and
 * finish with \0.
 * @details If the source size is less than size-1, copy the source to the destination.
 * @param dst the destination
 * @param src the source
 * @param size the number of character copied.
 * @return Source size.
 * @warning Destination size is not verified.
 */
int	ft_strlcpy(char *dst, const char *src, int size){}
```

### Norm
Meaningful names:
- Choose searchable names.
- Class names are defined by nouns or noun phrases.
- Function names are defined by verbs or verb phrases.
- Choose computer science/algorithm/pattern/mathematics/etc. names first, and if none exist, use technical terms related to the problem.

Functions:
- Try to make functions less than 20 lines long.
- A line should not exceed 150 characters.
- Maximum 2 nested loops/conditional structures.
- Try to use 1 statement per loop (function call).
- A function must do only one thing and cannot be broken down into sections.
- Try to use switch and if/else statements only once, used to create polymorphic objects, and hidden behind an inheritance relationship so that the rest of the system cannot see them.
- Try to have a maximum of 3 arguments per function.
- Prefer using function returns over argument modification.
- Avoid passing a boolean value to a function.
- Use classes or structures to reduce the number of arguments.
- Try to write function names that facilitate argument ordering (for example, assertExpectedEqualsActual(expected, actual) instead of assertEquals(expected, actual) because expected and actual have the same type and should not be reversed).
- Separate commands and requests.
- Prefer exceptions to error code returns.
- Extract try/catch blocks using throws to have a single statement in try/catch and so that the function starts with the try and ends with the catch.
- Rather than using enums or classes for all error codes, use multiple classes inheriting from the Exception class; this will avoid recompilation when adding errors.

## Git Organisation 📜

### Commit Rules

- Use `git config pull.rebase true` to define the rule of branch conflicts.
- Merge or rebase on the `main` branch as soon as a feature is done.
- Commit title must be formated like `KEY_WORD(feature or branch): commit title` with the followings key words: 
  - **ADD**
  - **UPDATE**
  - **REMOVE**
  - **FIX**
  - **TEST** --> Code relate to unit test.
  - **STYLE** --> Changing the formatting of the code, not the code itself (norm). 
  - **DOCS** --> Add or update docs, like readme or comments.
  - **PERF** --> Modify code to increase performance.
  - **REFACTO** --> Change code architecture.
  - **WIP** --> Work In Progress.
```bash
git commit -m 'KEY_WORD(feature or branch): commit title' -m 'commit description'
```

### Branch

#### Rule
A branch must be created for each feature.

#### Commands
See all branches in local (and what is the current branch)
```bash 
git branch
```
See all branches (and what is the current branch)
```bash 
git branch -a
```
Create a new branch `branch_name`
```bash
git branch <branch_name>
```
Remove a branch `branch_name` (before remove the local branch)
```bash
git push <remote> -d <branch_name>
```
Remove a local branch `branch_name` (think to switch on an other branch before)
```bash
git branch -d <branch_name>
```
Switch to a branch `branch_name`
```bash
git switch <branch_name>
```

### Log commands
Log
```bash
git log
```
Initialize the log at the begining of the project
```bash
git config --global alias.adog "log --all --decorate --oneline --graph"
```
Better log
```bash
git adog
```

### Update a local branch
```bash
git switch <branch>
git pull
```

### Update your branch with current `main`
1. Switch to your branch (if you are on another) and push to ensure your branch is up-to-date before merging
```bash
git switch <branch_name>
git push
```
2. Get the update version of main and add it in your branch
```bash
git pull <remote> main
```

### Merge
1. Switch to `main`
```bash
git switch main
```
2. Update the local `main`
```bash
git pull
```
3. Merge your work in `main`
```bash
git merge <branch_name>
```
4. Fix conflicts and check that everything is working properly

5. Add modifications
 ```bash
git status
git add <files>
``` 
6. Commit the modifications
```bash
git merge --continue
```
7. Push modification
```bash
git push
```

- To cancel a merge (impossible if the merge is committed)
```bash
git merge --abort
```

### Rebase
1. Switch to `main`
```bash
git switch main
```
2. Update the local `main`
```bash
git pull
```
3. Rebase your work in `main`
```bash
git rebase <branch_name>
```
4. Fix conflicts

5. Add modifications
 ```bash
git status
git add <files>
``` 
6. Pass to the next commit
```bash
git rebase --continue
```
7. Push modification
```bash
git push
```

- To cancel a rebase (cancel at the last `--continue`)
```bash
git rebase --abort
```

### Git Helper
Change the message of the last commit
```bash
git commit --amend
```
Cancel the last commit (go back just before validate the last commit)
```bash
git reset --soft HEAD~1
```
Remove the last commit (remove commit and modifications)
```bash
git reset --hard HEAD~1
```
View logs and hide logs
```bash
git reflog
```
Merge the last `n` commits
1. Define the number of last commits to change
```bash
git rebase -i HEAD~<n>
```
2. Change the `n-1` last commit from `pick` to `squash`
3. Quit Vim (Echap and :x)
4. Give name to the new commit which contains merge commits
5. **Only if commits was push**, force push to apply modifications
```bash
git push --force
```

## Links

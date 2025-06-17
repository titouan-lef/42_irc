# IRC

## General rules 📏
### Coding ⌨️

- Class name must be write in UpperCamelCase.
- All class must have a .cpp and .hpp.
- Function in cpp file must be write in read direction.
- Braces are alone on their own line for conditions and functions (can be on the same line for the rest).
- If an object is pass in a function and not modify by it, it must be declare with const and pass by reference.
- If a simple type (int, float, pointer, ect) is pass in a function and not modify by it, it must be declare with const.
- If possible, not use accessor and avoid as much as possible mutator.
- Use a private/protected function whenever is possible.
- Function's and varaible's name must be write in camelCase.
- All members must be private/protected.
- All members in class must be finish with `_`.
- `&` and `*` must be attached to the type.

#### Header Organisation
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

#### Comments
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

### Project Managment 📜
- Respect the [Git Organisation](#git-organisation)
- A branch must be created for each feature
- Merge on the `main` branch as soon as a feature is done
- Commit title must be formated like `KEY_WORD(feature or branch): commit title` with the followings key_words: 
  - **ADD**
  - **UPDATE**
  - **REMOVE**
  - **FIX**
  - **TEST**
  - **STYLE**
  - **DOCS**
  - **PERF**
  - **REFACTO**
  - **WIP** (Work In Progress)
```bash
git commit -m 'KEY_WORD: commit title' -m 'commit description'
```
## Git Organisation
### Work in your branch
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

## Link

# Changelog Entry for this MR

### Contributors

- author:
- code review:

### Added

### Changed

### Removed

### Fixed

# Checklist for MR Creator

- [ ] MR labeled (trivial vs. non-trivial)
- [ ] Run with `./configure.sh -check` performed to check for out-of-bounds access
- [ ] Global benchmark report against the last version (`/p/project/lpjml/benchmark_run_outputs/...`) created
- [ ] Bitwise comparison of outputs with `cmpbin` or `diff` done, if no changes are expected in the results
- [ ] Compliance with `stylesheet.md` checked
- [ ] Changelog entry added
- [ ] Man pages updated if needed
- [ ] In case of new feature development: If it can be switched off, confirm that results remain the same as in the version without the feature
- [ ] Reviewer 1 assigned (ensure a broad pool of reviewers)

# Checklist for First Reviewer

- [ ] Performed relevant review of code changes. Relevant aspects may include:
  - Understanding how the code changes fulfill the author's intent
  - Checking comments
  - Performing single-cell runs
  - Checking if the code compiles with your standard LPJmL configuration
  - Looking at the benchmark report
  - Running `lpjcheck`
  - If the MR refers to utilities, running the respective utilities
- [ ] Checked the completeness of the changelog. Does the changelog entry accurately reflect the code changes?
- [ ] Reviewer 2 assigned

# Checklist for Second Reviewer

- [ ] Performed relevant review of code changes. Relevant aspects may include:
  - Understanding how the code changes fulfill the author's intent
  - Checking comments
  - Performing single-cell runs
  - Checking if the code compiles with your standard LPJmL configuration
  - Looking at the benchmark report
  - Running `lpjcheck`
  - If the MR refers to utilities, running the respective utilities
- [ ] Checked the completeness of the changelog. Does the changelog entry accurately reflect the code changes?

# Checklist before merging

- [ ] Increment version number
- [ ] Reviewers listed in the changelog
- [ ] Add changelog date

# Notes on responsibilities of author and reviewers

- The reviewer opens discussion threads for any feedback
- The author resolves the discussion threads
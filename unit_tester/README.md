# Unit Tester Template
Unit test template infrastructure, aimed to be a base for other projects unit test toolchain.

The template is based on CppUTest (a C /C++ based unit xUnit test framework).

# Usage
It is suggested that other projects will add the repo using git-subtree and modify it per the project need.

Later on, a subtree merge may be issued from the UT Template to the individual projects.

# Install

- Install CppUTest: See http://cpputest.github.io 
  [Note: You will need to define a system shell variable pointing to the installed CPPUTEST root: CPPUTEST_HOME=~/workspace/cpputest]
- Add the repo to your project as a subtree:
git subtree add --prefix unit_tester https://github.com/EdDev/unit_tester_template.git master --squash
- Just enter the make command under ./unit_tester (after installing CppUtest on your host)

# Update
Projects may update their unit_tester by pulling changes from the template repo:

git subtree pull --prefix unit_tester https://github.com/EdDev/unit_tester_template.git master --squash

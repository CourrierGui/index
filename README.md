# Index

- `/usr/share/index/schema.sql`: path where the default schema is installed
- `/var/lib/index/<index name>.db`: path where the database for each index is stored

## TODO

- [ ] build abstraction layer on top of SQLite
- [ ] sanitizer
- [ ] clang and clang format
- [ ] find library to read data from PDF
- [ ] review NLP stuff

## Examples

```
# Read the content of foo bar and baz and add them to the reverse index
$ index add foo bar baz

# Set attribute k1 and k2 with value v1 and v2 respectively to file foo
# If k1 is already set, it will be updated
# The option --no-update will prevent this and display a warning in this case
$ index set-attr foo k1=v1,k2=v2

# Remove attributes
$ index del-attr foo k1

# List attributes of foo and bar
$ index get-attr foo bar

# Add a description to a file (read from stdin)
$ index desc foo

# Find files with attribute baz and foo. foo has the value bar.
# The files will contain de word toto with file type pdf.
$ index find -data toto -attr foo=bar,baz -type pdf
```

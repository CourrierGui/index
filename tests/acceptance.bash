#!/bin/bash

project_root=$1
db_path="$HOME/.local/share/index"
db_name=mytestdb
file=mytestfile

__index=$(PATH="$project_root/build:$PATH" which index)
function index()
{
    "$__index" "$@"
}

function error()
{
    echo "$@"
    exit 1
}

function test_cleanup()
{
    rm -f "${db_path}/${db_name}.db" "$file"
}

trap test_cleanup EXIT

index create "$db_name"
index list-db | grep "$db_name" ||
    error "Failed to create '$db_name'"

index select "$db_name"
index list-db | grep "$db_name (X)" ||
    error "Failed to create '$db_name'"

index get-attr unknown-file &&
    error "Getting attributes of non existing file should fail"

touch "$file"

index set-attr "$file" k1 v1 k2 v2 k3 v3
index get-attr "$file" | grep "$(realpath "$file"): k1, v1" ||
    error "Attributes '(k1, v1)' not found in '$file'"
index get-attr "$file" | grep "$(realpath "$file"): k2, v2" ||
    error "Attributes '(k2, v2)' not found in '$file'"
index get-attr "$file" | grep "$(realpath "$file"): k3, v3" ||
    error "Attributes '(k3, v3)' not found in '$file'"

index del-attr "$file" k1
index get-attr "$file" | grep "$(realpath "$file"): k1, v1" &&
    error "Attributes '(k1, v1)' found in '$file' but should have been deleted"

exit 0

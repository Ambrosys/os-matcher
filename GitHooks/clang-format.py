import os
import shlex
import subprocess
from subprocess import Popen, PIPE


def has_valid_extension(file_name):
    valid_extensions = ('.cpp', '.cxx', '.c', '.hpp', '.hxx', '.h')
    extension = os.path.splitext(file_name)[1]
    return extension in valid_extensions


def call(*cmd):
    if len(cmd)==1 and isinstance(*cmd, str):
        cmd = shlex.split(*cmd)
    p = subprocess.Popen( cmd, universal_newlines=True, stdin=PIPE, stdout=PIPE, stderr=PIPE )
    if p.returncode:
        raise RuntimeError("{} returned {}".format(sum(*cmd), p.returncode) )
    out = p.communicate()[0]
    return out.split()


def format_file_and_add(file_name):
    call('clang-format', '-i', file_name)
    call('git', 'add', file_name)


def main():
    changed_files = call('git diff-index --cached --name-only HEAD')
    for file_name in changed_files:
        if has_valid_extension(file_name):
            format_file_and_add(file_name)


if __name__ == '__main__':
    main()

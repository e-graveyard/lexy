import platform
from datetime import datetime

from os.path import join
from os.path import dirname
from os.path import abspath


def read_file(filepath):
    with open(filepath, 'r') as file:
        return file.read()


def write_file(filepath, data):
    with open(filepath, 'w') as file:
        file.write(data)


def main():
    now = datetime.now()

    compile_date = now.strftime('%Y-%m-%d')
    compile_time = now.strftime('%H:%M:%S')

    target_arch = platform.machine()
    target_kernel = platform.system()

    src_dir = abspath(join(dirname(__file__), '..', 'src'))
    src_file = lambda f: join(src_dir, f)

    meta_content = read_file(src_file('_meta.h'))
    meta_content = (
        '/* this file is auto-generated */'
        + '\n'
        + meta_content.format(
            kernel=target_kernel,
            arch=target_arch,
            cdate=compile_date,
            ctime=compile_time
        )
    )

    write_file(src_file('meta.h'), meta_content)


if __name__ == '__main__':
    main()

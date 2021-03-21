import platform
from datetime import datetime

from os.path import basename
from os.path import join
from os.path import dirname
from os.path import abspath

from textwrap import dedent


def read_file(filepath):
    with open(filepath, 'r') as file:
        return file.read()


def write_file(filepath, data):
    with open(filepath, 'w') as file:
        file.write(data)


def main():
    print('>>> {}'.format(basename(__file__)))
    now = datetime.now()

    compile_date = now.strftime('%Y-%m-%d')
    compile_time = now.strftime('%H:%M:%S')

    target_arch = platform.machine()
    target_kernel = platform.system()

    src_dir = abspath(join(dirname(__file__), '..', 'src'))

    meta_f = join(src_dir, '_meta.h')  # origin header file
    meta_t = join(src_dir, 'meta.h')   # destination header file

    print(
        dedent(
            '''
            * reading from:  {}
            * writing to:    {}

            * compile_date:  {}
            * compile_time:  {}
            * target_arch:   {}
            * target_kernel: {}
            '''.format(
                meta_f,
                meta_t,
                compile_date,
                compile_time,
                target_arch,
                target_kernel
            )
        )
    )

    meta_content = read_file(meta_f)
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

    write_file(meta_t, meta_content)


if __name__ == '__main__':
    main()

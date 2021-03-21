import platform
from datetime import datetime

from os.path import basename
from os.path import join
from os.path import dirname
from os.path import abspath

from subprocess import check_output
from textwrap import dedent


def read_file(filepath):
    with open(filepath, 'r') as file:
        return file.read()


def write_file(filepath, data):
    with open(filepath, 'w') as file:
        file.write(data)


def get_compiler_info():
    ret = check_output(['cc', '--version'])
    ret = ret.decode('utf-8')
    ret = ret.split('\n')

    return ret[0]


def main():
    print('>>> {}'.format(basename(__file__)))
    now = datetime.now()

    compilation_date = now.strftime('%Y-%m-%d')
    compilation_time = now.strftime('%H:%M:%S')

    target_arch = platform.machine()
    target_kernel = platform.system()

    compiler_info = get_compiler_info()

    src_dir = abspath(join(dirname(__file__), '..', 'src'))

    meta_f = join(src_dir, '_meta.h')  # origin header file
    meta_t = join(src_dir, 'meta.h')   # destination header file

    print(
        dedent(
            '''
            * reading from: {}
            * writing to:   {}

            * target_arch:      {}
            * target_kernel:    {}
            * compiled_with:    {}
            * compilation_date: {}
            * compilation_time: {}
            '''.format(
                meta_f,
                meta_t,
                target_arch,
                target_kernel,
                compiler_info,
                compilation_date,
                compilation_time,
            )
        )
    )

    meta_content = read_file(meta_f)
    meta_content = (
        '/* this file is auto-generated */'
        + '\n'
        + meta_content.format(
            arch=target_arch,
            kernel=target_kernel,
            compw=compiler_info,
            cdate=compilation_date,
            ctime=compilation_time
        )
    )

    write_file(meta_t, meta_content)


if __name__ == '__main__':
    main()

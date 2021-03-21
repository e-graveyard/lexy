set -e

HERE="$(pwd)/utils"
BIN="${HERE}/has-readline"

cc -o "$BIN" -x c - <<EOF

#if !__has_include(<editline/readline.h>)
#define __MISSING_READLINE
#endif

int main(int argc, char** argv)
{
    int code = 0;

#ifdef __MISSING_READLINE
    code = 1;
#endif

    return code;
}

EOF

"$BIN"
echo "$?"
rm -rf "$BIN"

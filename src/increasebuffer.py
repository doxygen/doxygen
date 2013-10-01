# Since the internal token buffer of a generated flex file is hardcoded
# to 16K, this script is used to increase the buffer size of a flex
# generated scanner to 256K.
import sys
sys.stdout.write(sys.stdin.read().
    replace('YY_BUF_SIZE 16384','YY_BUF_SIZE 262144').
    replace('YY_READ_BUF_SIZE 8192','YY_READ_BUF_SIZE 262144'))

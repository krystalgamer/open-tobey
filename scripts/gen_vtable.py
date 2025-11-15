import sys

def main():
    lines = None
    with open ('vtable.txt', 'r') as fp:
        lines = fp.readlines()

    lines = map(lambda x: x.strip(), lines)
    lines = filter(lambda x: x != '', lines)

    for i, line in enumerate(lines):
        func = line.split('__')[0]
        print(f'VALIDATE_VTABLE(entity, {func}, {i+1});')

    return 0

if __name__ == '__main__':
    sys.exit(main())

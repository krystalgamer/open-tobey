import sys

def main():
    lines = None
    with open ('vtable_item.txt', 'r') as fp:
        lines = fp.readlines()

    lines = map(lambda x: x.strip(), lines)
    lines = filter(lambda x: x != '', lines)

    for i, line in enumerate(lines):
        func = line.split('__')[0]

        func = func[func.rindex(' ')+1:]
        print(f'VALIDATE_VTABLE(item, {func}, {i});')

    return 0

if __name__ == '__main__':
    sys.exit(main())

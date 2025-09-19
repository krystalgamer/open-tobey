import sys

def main():

    lines = None
    with open('bink_exports.txt', 'r') as fp:
        lines = fp.readlines()


    for line in lines:
        x = line.strip()
        func = x.split(' ')[-1]
        print(f'#pragma comment(linker,"/export:{func}=binkw32_.{func}")')

    return 

if __name__ == '__main__':
    main()
    sys.exit(0)

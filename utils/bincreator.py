import argparse
import sys

def binary_to_bytes(binary_string):
    byte_list = [binary_string[i:i+8] for i in range(0, len(binary_string), 8)]
    return bytes(int(byte, 2) for byte in byte_list)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-f', '--file', required=True)
    
    args = parser.parse_args()

    print("enter bits, end input with Ctrl+D:")

    binary_data = ''
    try:
        for line in sys.stdin:
            binary_data += line.strip().replace(' ', '').replace('\n', '')
    except KeyboardInterrupt:
        pass

    if not all(bit in '01' for bit in binary_data):
        print("error: the string should only contain zeros and ones.")
        return

    byte_data = binary_to_bytes(binary_data)

    with open(args.file, 'wb') as file:
        file.write(byte_data)

    print(f"written to file: {args.file}")

if __name__ == '__main__':
    main()


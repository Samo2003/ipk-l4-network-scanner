import subprocess
import re

SCANNER = "./ipk-l4-scan"
NMAP = "nmap"
RED = "\033[91m"
GREEN = "\033[92m"
RESET = "\033[0m"

def load_tests(file_name):
    with open(file_name, 'r') as file:
        return [line.strip() for line in file.readlines() if line.strip()]
    
INVALID_TESTS = load_tests("test/invalid_inputs.txt")
VALID_TESTS = load_tests("test/valid_inputs.txt")
NMAP_INPUT = load_tests("test/nmap_input.txt")

total_tests = 0
successful_invalid_tests = 0
invalid_tests = 0
successful_valid_tests = 0
valid_tests = 0
interface_test = 0

def test_invalid_input(args):
    global total_tests, successful_invalid_tests, invalid_tests
    total_tests += 1
    invalid_tests += 1

    try:
        result = subprocess.run(["sudo", SCANNER] + args.split(), check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError:
        print(f"TEST {invalid_tests}: {GREEN}[PASS]{RESET}")
        successful_invalid_tests += 1
        return

    print(f"TEST {invalid_tests}: {RED}[FAIL]{RESET} args: {args}")


def test_interface_list():
    global interface_test

    result = subprocess.run(['ifconfig'], capture_output=True, text=True)
    interfaces = result.stdout.splitlines()
    active_interfaces = []
    
    for line in interfaces:
        if 'UP' in line:
            interface_name = line.split(':')[0].strip()
            active_interfaces.append(interface_name)
    
    scanner = subprocess.run([SCANNER], check=True, capture_output=True, text=True)
    scanner_interfaces = scanner.stdout.splitlines()[1:]
    if set(scanner_interfaces) == set(active_interfaces):
        print(f"TEST 1: {GREEN}[PASS]{RESET}")
        interface_test += 1
    else:
        print(f"TEST {valid_tests}: {RED}[FAIL]{RESET}")
    

def parse_output(output):
    ports = set()
    for line in output.splitlines():
        match = re.match(r"(\d+)/(tcp|udp)\s+(\w+)", line)
        if match:
            port, protocol, state = match.groups()
            ports.add(f"{port}/{protocol} {state}")
    return ports

def test_valid_input(args, nmap_args):
    global total_tests, successful_valid_tests, valid_tests
    total_tests += 1
    valid_tests += 1

    try:
        scanner_output = subprocess.run(["sudo", SCANNER] + args.split(), capture_output=True, text=True, check=True)
        nmap_output = subprocess.run(["sudo", NMAP] + nmap_args.split(), capture_output=True, text=True, check=True)
    except subprocess.CalledProcessError:
        print(f"TEST {valid_tests}: {RED}[FAIL]{RESET} scanner or nmap failed")
        return
    
    scanner_ports = parse_output(scanner_output.stdout)
    nmap_ports = parse_output(nmap_output.stdout)

    for port in scanner_ports:
        if port not in nmap_ports:
            print(f"TEST {valid_tests}: {RED}[FAIL]{RESET} args: {args}")
            return
        
    print(f"TEST {valid_tests}: {GREEN}[PASS]{RESET}")
    successful_valid_tests += 1
    
print("Testing invalid inputs:")
for invalid in INVALID_TESTS:
    test_invalid_input(invalid)

print("-----------------------------------------")

print("Testing active interface list:")
test_interface_list()
total_tests += 1

print("-----------------------------------------")

print("Testing valid inputs:")
for valid, nmap in zip(VALID_TESTS, NMAP_INPUT):
    test_valid_input(valid, nmap)

print("-----------------------------------------")
print("Summary:")
print(f"Invalid inputs: {successful_invalid_tests}/{invalid_tests}")
print(f"Interface test: {interface_test}/1")
print(f"Valid inputs: {successful_valid_tests}/{valid_tests}")
percentage = (successful_invalid_tests + successful_valid_tests + interface_test)/total_tests * 100
print(f"Total percents: {int(percentage)}%")
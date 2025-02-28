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

total_tests = 0
successful_invalid_tests = 0
failed_tests = 0
invalid_tests = 0

def test_invalid_input(args):
    global total_tests, successful_invalid_tests, invalid_tests
    total_tests += 1
    invalid_tests += 1

    try:
        subprocess.run([SCANNER] + args.split(), check=True, capture_output=True, text=True)
        print(f"TEST {invalid_tests}: {RED}[FAIL]{RESET} args: {args}")
    except subprocess.CalledProcessError:
        print(f"TEST {invalid_tests}: {GREEN}[PASS]{RESET}")
        successful_invalid_tests += 1


print("Running invalid tests:")
for invalid in INVALID_TESTS:
    test_invalid_input(invalid)


print("-----------------------------------------")
print("Summary:")
print(f"Invalid inputs: {successful_invalid_tests}/{invalid_tests}")